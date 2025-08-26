# Create Ground-Derived Channels in F Prime GDS

Ground-derived channels allow you to compute new values from incoming telemetry on the ground, using logic that runs entirely within the F Prime Ground Data System (GDS). These values can be published into the ground system as if they came from flight, enabling flexible monitoring, UI presentation, or conversions on the ground.

This guide walks through the process of creating a basic plugin that listens to incoming telemetry and publishes it back to F Prime.

> [!WARNING]
> Ground-derived channels are only supported when running with the `--no-zmq` option of the GDS. The ZeroMQ backend is fast but cannot allow arbitrary publishers.

> [!NOTE]
> **Prerequisite**  
> Before following this guide, you should first complete the [Develop GDS Plugins](../how-to/develop-gds-plugins.md) guide.  
> It explains the plugin system, registration process, and runtime behavior essential to this example.

---

## When to Use This

Use a ground-derived channel when you need additional channels on the ground, but do not wish to compute nor downlink these values in-flight. For example:

- You want to transform or scale telemetry (e.g., converting a raw sensor ADC measurement to engineering units)
- You want to compute a value based on multiple telemetry channels (e.g., battery differential)
- You want to normalize or rename telemetry values for downstream consumers

---

## Plugin Architecture

Ground-derived channels are implemented using a `DataHandlerPlugin`, one of the built-in plugin types in the F Prime GDS.

Our plugin will:

- **Receive decoded telemetry data** via the `data_callback` API
- **Apply your transformation logic**
- **Publish the result** using the standard pipeline

The plugin runs in the `CustomDataHandler` process, isolated from the core GDS.

---

## Basic Setup

To get started, create a Python file (e.g., `ground_derived_channels.py`) and define a plugin class that listens for telemetry. The initial plugin structure will look like this:

```python
from fprime_gds.common.handlers import DataHandlerPlugin
from fprime_gds.plugin.definitions import gds_plugin


@gds_plugin(DataHandlerPlugin)
class ExampleGroundDerivedChannel(DataHandlerPlugin):
    """ Example Ground derived channel
    """
  
    @classmethod
    def get_name(cls):
        """ Return the name of the plugin """
        return "example-ground-derived-channel"
    
    @classmethod
    def get_arguments(cls):
        """ No special arguments"""
        return {}

    def get_handled_descriptors(self):
        """ List descriptors of F Prime data types that this plugin can handle """
        # Inform the GDS we want to process telemetry channels
        return ["FW_PACKET_TELEM"]

    def data_callback(self, data, source):
        """ Handle channel objects
        """
        pass
```

The two critical functions are: `get_handled_descriptors`, and `data_callback`. `get_handled_descriptors` returns a list of descriptors to listen to, and `data_callback` will give a place to perform our calculations. In our case, we only want to subscribe to telemetry channels (i.e. `FW_PACKET_TELEM`).  When using the telemetry packetizer, users may alternatively subscribe to `FW_PACKET_PACKETIZED_TLM`.

## Defining New Channels

To publish new channels, you must define them. Create a new dictionary file (e.g. "MyGroundChannelsDictionary.json") and add the channels you need. This structure will look like the following:

```
{
  "metadata" : { 
    "deploymentName" : "GroundChannels",
    "projectVersion" : "<match your project>",
    "frameworkVersion" : "<match your project>",
    "libraryVersions" : [], 
    "dictionarySpecVersion" : "1.0.0"
  },  
  "telemetryChannels" : [ 
    {   
      "name" : "Examples(Ground).CommandCountMinus7",
      "type" : { 
        "name" : "I64",
        "kind" : "integer",
        "size" : 64
      },  
      "id" : 1000020224,
      "telemetryUpdate" : "always",
      "annotation" : "Output (derived) of command count less seven"
    }   
  ],
  


  "typeDefinitions" : [],
  "constants" : [], 
  "commands" : [], 
  "parameters" : [], 
  "events" : [], 
  "records" : [], 
  "containers" : [], 
  "telemetryPacketSets" : []
}
```

Make sure that the "name" and "id" fields are unique to your derived channel.

## Deriving Values and Time

Now that you have a plugin structure and dictionary, it is time to derive the channel. The sample code below produces a count from CommandsDispatched less seven.

```python
    def data_callback(self, data, source):
        """ Handle channel objects
        """
        # Filter out all channels that we do not need to create our derivation
        if not data.template.get_full_name().endswith("CommandsDispatched"):
            return
        # Operate on the channel's `val` field
        new_value = data.val - 7
        # Publish the new channel.
        self.publish_channel("Examples(Ground).CommandCountMinus7", new_value, data.time)
```

First this code filters out unwanted channels. Then it performs a translation on the data's value.  Then it publishes the new channel supplying name, value, and time.  In this case, we have reused the original time.

## Running It

Install the plugin as directed in the plugin development How-To.  Next we need to merge our dictionaries and run it.  This is accomplished by running the merge dictionary command, and then supplying the output to the `--dictionary` flag of the GDS.

```
fprime-merge-dictionary --permissive --output MergedDictionary.json \
    /path/to/flight/dictionary \
    ./MyGroundChannelsDictionary.json
fprime-gds --dictionary ./MergedDictionary.json --no-zmq
```
---

## See Also

- [DataHandlerPlugin Reference](../reference/gds-plugins/data-handler.md)
- [Plugin System Overview](../how-to/develop-gds-plugins.md)
- [Cosine Example](https://github.com/nasa/fprime-examples/tree/devel/GdsExamples/gds-plugins/src/ground_channels)
