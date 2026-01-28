# App Plugin

GDS App Plugins allow users to extend the functionality of the F Prime Ground Data System by running custom Python applications in a separate process. These plugins are launched and managed by the GDS infrastructure and can be used to integrate new behaviors, data processors, UIs, or bridges to external systems. There are two application types `GdsApp`, which is a general app plugin, and `GdsStandardApp`, which is an app plugin that launches the GDS standard pipeline.

Each GDS App runs independently and a may communicate with the rest of the GDS through inter-process communication. This separation helps isolate potentially blocking or experimental functionality from core GDS processes.

An example [`OpenMCT`](https://github.com/fprime-community/fprime-openmct/blob/devel/src/fprime_openmct/launch_plugin.py) shows how to use a GDS App plugin to launch another service (in this case `fprime-openmct-launch`, which is a script provided by the package).

GDS Apps are `FEATURE` plugins.  All will run unless individually disabled by the user.

## Usage

GDS App Plugins are used to build custom applications that run alongside the GDS. These may include:

- Custom dashboards or telemetry visualizers
- Background processors for specific telemetry or event types
- External data bridges (e.g., logging to a database, forwarding to a network endpoint)

The plugin must define a subclass of `GdsApp` or use the convenience subclass `GdsStandardApp`, and be decorated with `@gds_plugin(GdsApp)`.

## Considerations

- GDS App Plugins are launched in separate processes by the GDS framework.
- Misbehaving plugins may impact system monitoring if they crash, exit early, or consume excessive resources.
- Plugins do not automatically communicate with the GDS unless they explicitly use shared resources such as the standard pipeline.
- Use `GdsStandardApp` for most use cases where GDS data is consumed using `data_callback` methods

## Required Interface

There are two approaches to GDS App plugins.

## Option 1: Low-Level Interface (`GdsApp`)

To fully control how your plugin is launched, subclass the [`GdsApp`](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/executables/apps.py#L76) base class and implement the `get_process_invocation()` method. This lets you run any external process invoked through the command line.

> [!CAUTION]
> The `GdsApp` plugin gives implementors full control, but it does not automatically establish any communication with the GDS data network. If you do not need custom data setup, consider `GdsStandardApp` described below.

```python
from fprime_gds.executables.apps import GdsApp
from fprime_gds.plugin.definitions import gds_plugin

@gds_plugin(GdsApp)
class MyExternalProcessPlugin(GdsApp):
    """Launches a custom script in its own process."""

    def get_process_invocation(self, args) -> list[str]:
        return ["python3", "my_script.py", "--log=telemetry.txt"]
```

> [!TIP]
> `GdsApp` plugins may launch any process and are not limited to Python processes!

## Option 2: Recommended Interface (GdsStandardApp)

For most applications that want to interact with GDS data, subclassing [`GdsStandardApp`](https://github.com/fprime-community/fprime-gds/blob/devel/src/fprime_gds/executables/apps.py#L166) is recommended. This helper class handles:

- Wiring up the standard GDS data pipeline
- Parsing and forwarding common CLI arguments
- Invoking your application’s `start()` method inside a proper runtime environment

### Required Methods

You must implement:

    start(self, pipeline: StandardPipeline) -> None:
    Main logic of your application. Called after GDS is initialized and connected. Passed a StandardPipeline

You may optionally implement:

    init(self) -> None:
    Called before the pipeline is created. Useful for setting up internal state or environment configuration.

    get_additional_arguments(self) -> Dict[Tuple[str], Dict]:
    Return additional CLI arguments your app needs. These are injected into the GDS CLI parser.  It is a map of argument flags to Argparse add_argument keyword arguments.  These arguments are passed to the constructor.

### Example: Sending Commands

This example shows how to use standard pipeline functions. In this case, `pipeline.send_command` is used to emit a `CMD_NO_OP` at a custom interval.

```python
import time
from fprime_gds.executables.apps import GdsApp
from fprime_gds.executables.apps import GdsStandardApp
from fprime_gds.plugin.definitions import gds_plugin

@gds_plugin(GdsApp)
class MyCustomApp(GdsStandardApp):
    """ Custom application """

    def __init__(self, no_op_rate, **kwargs):
        super().__init__(**kwargs)
        self.no_op_rate = custom_rate

    def start(self, pipeline):
        print("App started: " + self.no_op_rate)
        while True:
            pipeline.send_command("cmdDisp.CMD_NO_OP", [])
            time.sleep(self.no_op_rate)
    
    def get_additional_arguments(self):
        from argparse import ArgumentParser
        return {("--no-op-rate",): {
            "type": int,
            "default": 10,
            "help": "Rate to emit no-ops"
        }
```

> [!NOTE]
> You do not need to implement get_process_invocation() when using GdsStandardApp. It is handled automatically.

### Example: Data Handlers

A process to run custom data handler plugins shows an example of a GdsStandardApp: [CustomDataHandlers](https://github.com/nasa/fprime-gds/blob/0b749b54b8ff8c6b5a379a6e0adb5acacc7a3d30/src/fprime_gds/executables/apps.py#L285-L323).  It uses `init` to set up the plugin system before parsing arguments, and uses `start` to attach data handlers to the standard pipeline.

### Summary

| Use Case                                               | Base Class        | Notes                                                                 |
|--------------------------------------------------------|-------------------|-----------------------------------------------------------------------|
| Launch an external process (e.g., script or binary)    | `GdsApp`          | Manually constructs a command via `get_process_invocation()`          |
| Create a headless service that does not use GDS data   | `GdsApp`          | Examples include network bridges, monitors, or external loggers       |
| Build an app that uses the GDS StandardPipeline        | `GdsStandardApp`  | Recommended for most plugins that process telemetry, events, etc.     |
| Add custom CLI arguments for your plugin               | `GdsStandardApp`  | Override `get_additional_arguments()`                                 |
| Perform setup before pipeline creation                 | `GdsStandardApp`  | Override `init()` to customize environment or internal state          |


