# Standard Pipeline

The Test API relies on the Standard Pipeline to interface with the GDS.

standard.py:

This file contains the necessary functions used to setup a standard pipeline that will pull data from the middle-ware
layer into the python system. This layer is designed to setup the base necessary to running Gds objects up-to the layer
of decoding. This enables users to generate add-in that function above the decoders, and run on a standard pipeline
below.


* **author**

    lestarch



#### class fprime_gds.common.pipeline.standard.StandardPipeline()
Bases: `object`

Class used to encapsulate all of the components of a standard pipeline. The life-cycle of this class follows the
basic steps:

> 1. setup: creates objects needed to read from middle-ware layer and provide data up the stack

> 1. register: consumers from this pipeline should register to receive decoder callbacks

> 1. run: this pipeline should either take over the standard thread of execution, or be executed on another thread

> 1. terminate: called to shutdown the pipeline

This class provides for basic log files as a fallback for storing events as well. These logs are stored in a given
directory, which is created on the initialization of this class.


#### __init__()
Set core variables to None


#### setup(config, dictionary, logging_prefix='/home/kevin', packet_spec=None)
Setup the standard pipeline for moving data from the middleware layer through the GDS layers using the standard
patterns. This allows just registering the consumers, and invoking ‘setup’ all other of the GDS support layer.
:param config: config object used when constructing the pipeline.
:param dictionary: dictionary path. Used to setup loading of dictionaries.
:param logging_prefix: logging prefix. Logs will be placed in a dated directory under this prefix
:param packet_spec: location of packetized telemetry XML specification.


#### setup_coders()
Sets up the encoder layer of the GDS pipeline.
:return:


#### setup_logging(prefix)
Setup logging based on the logging prefix supplied
:param prefix: logging prefix to use


#### setup_history()
Setup a set of history objects in order to store the events of the decoders.


#### load_dictionaries(dictionary, packet_spec)
Loads the dictionaries based on the dictionary path supplied
:param dictionary: dictionary path used for loading dictionaries
:param packet_spec: specification for packets, or None, for packetized telemetry


#### connect(address, port)
Connects to the middleware layer
:param address: address of middleware
:param port: port of middleware


#### send_command(command, args)
Sends commands to the encoder and history.
:param command: command id from dictionary to get command template
:param args: arguments to process


#### disconnect()
Disconnect from socket


#### get_event_id_dictionary()
Getter for event dictionary.
:return: event dictionary keyed by event IDs


#### get_event_name_dictionary()
Getter for event dictionary.
:return: event dictionary keyed by event mnemonics


#### get_channel_id_dictionary()
Getter for channel dictionary.
:return: channel dictionary keyed by channel IDs


#### get_channel_name_dictionary()
Getter for channel dictionary.
:return: channel dictionary keyed by channel mnemonics


#### get_command_id_dictionary()
Getter for command dictionary.
:return: command dictionary keyed by command IDs


#### get_command_name_dictionary()
Getter for command dictionary.
:return: command dictionary keyed by command mnemonics


#### get_event_history()
Getter for event history.
:return: event history


#### get_channel_history()
Getter for channel history.
:return: channel history


#### get_command_history()
Getter for command history.
:return: command history


#### register_event_consumer(history)
Registers a history with the event decoder.


#### remove_event_consumer(history)
Removes a history from the event decoder. Will raise an error if the history was not
previously registered.
Returns:

> a boolean indicating if the history was removed.


#### register_telemetry_consumer(history)
Registers a history with the telemetry decoder.


#### remove_telemetry_consumer(history)
Removes a history from the telemetry decoder. Will raise an error if the history was not
previously registered.
Returns:

> a boolean indicating if the history was removed.


#### register_command_consumer(history)
Registers a history with the standard pipeline.


#### remove_command_consumer(history)
Removes a history that is subscribed to command data. Will raise an error if the history
was not previously registered.
Returns:

> a boolean indicating if the history was removed.

|Quick Links|
|:----------|
|[Integration Test API User Guide](../user_guide.md)|
|[GDS Overview](https://github.com/fprime-community/fprime-gds)|
|[Integration Test API](integration_test_api.md)|
|[Histories](histories.md)|
|[Predicates](predicates.md)|
|[Test Logger](test_logger.md)|
|[Standard Pipeline](standard_pipeline.md)|
|[TimeType Serializable](time_type.md)|
