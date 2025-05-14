# GDS Function Plugin

GDS Function Plugins allow advanced users to inject custom logic directly into the GDS runtime process. These plugins are executed once at system startup and can be used for runtime introspection, dynamic registration, instrumentation, or one-time configuration.

Unlike GDS Apps, GDS Function Plugins do **not** run in a separate process. They execute inside the main GDS process and run once.

GDS Functions are `FEATURE` plugins.  All will run unless individually disabled by the user.


## Usage

This plugin is used to run custom one-time logic during the startup of the GDS. Typically, this is used to launch a process where the user needs more control than [GdsApp Plugins](./gds-app.md) allow.

## Considerations

- Runs during GDS startup, before the main loop begins
- Executes in the **same process** as the main GDS
- Blocking or long-running logic will delay GDS startup

## Required Interface

To create a custom GDS Function Plugin, subclass the [`GdsFunction`](https://github.com/nasa/fprime-gds/blob/devel/src/fprime_gds/executables/apps.py#L52) class and implement the `run()` method. This method will be called once, during GDS initialization.

```python
from fprime_gds.executables.apps import GdsFunction
from fprime_gds.plugin.definitions import gds_plugin

@gds_plugin(GdsFunction)
class ExampleStartupHook(GdsFunction):
    """Prints a message during GDS startup."""

    def run(self):
        print("Custom GDS function plugin initialized!")
```
