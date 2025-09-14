# Data Handler Plugin

Data Handler Plugins allow users to register custom consumers for decoded F Prime data types, such as telemetry, events, or channels. These plugins are useful for logging, transforming, forwarding, or visualizing data as it flows through the system.

Each handler is dynamically registered to specific data descriptors (e.g., telemetry, events) and all custom data handlers run in a single custom data handler process. This allows these plugins to process decoded data without interfering with the core GDS runtime.

An example [`OpenMCTPush`](https://github.com/fprime-community/fprime-openmct/blob/devel/src/fprime_openmct/data_push.py) plugin shows how to use a `DataHandler` to push data to another service (in this case via ZeroMQ).

Data Handler plugins are `FEATURE` plugins.  All will run unless individually disabled by the user.


## Usage

To use a Data Handler plugin, implement the `data_callback()` method, which is invoked whenever a matching decoded data item is received. You can register for specific descriptor types  by returning them in the `get_descriptor()` method.

Typical use cases include:

- Writing telemetry or events to an external database
- Sending selected data to a dashboard or network client
- Logging filtered data to files

## Considerations

- Data Handler plugins run in the `CustomDataHandler` process.
- Each handler is registered only for the descriptor types it advertises.
- The decoded `data` object passed to `data_callback()` is specific to the descriptor type.

## Required Interface

To create a Data Handler plugin, subclass the [`DataHandlerPlugin`](https://github.com/nasa/fprime-gds/blob/devel/src/fprime_gds/common/handlers.py#L33) base class and implement the following:

- `get_handled_descriptors() -> list[str]`:  
  Return a list of descriptor types this plugin wants to receive. Use the following to determine which descriptors to support:

| Descriptor String | Data                 |
|-------------------|----------------------|
| "FW_PACKET_TELEM" | F Prime channels     |
| "FW_PACKET_LOG"   | F Prime events       |
| "FW_PACKET_FILE"  | F Prime file packets |
| "FW_PACKET_PACKETIZED_TLM" | F Prime packets |


- `data_callback(data, source)`:  
  Handle incoming decoded data. The `data` type depends on the descriptor. The `source` argument is unused GDS plugins.

```python
from fprime_gds.common.handlers import DataHandlerPlugin
from fprime_gds.common.plugins import gds_plugin

@gds_plugin(DataHandlerPlugin)
class EventLogger(DataHandlerPlugin):
    """Logs all event data to a file."""

    def get_descriptor(self):
        return ["FW_PACKET_LOG"]

    def data_callback(self, data, source):
        with open("event_log.txt", "a") as f:
            f.write(f"{data}\n")
```

This plugin will be called for every decoded event received by the system.

> [!NOTE]
> The decoded data passed to data_callback() is an instance of the decoded object — for example, a ChannelTelemetry, Event, or CustomType depending on the descriptor.

## Helpers

The data handler plugin also supports some helper functionality.

```
self.publisher.publish_channel(name: str, value: Any, time: TimeType)
```

This call allows users to publish telemetry values by name. It is used in ground-processed channels.
