# Dictionary Capabilities

## Commands [FPP User Guide](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Commands)

Commands are specified via the FPP modeling language and can include the following options:

- `identifier` - The unique name of the command
- `command-kind` - Commands can be 'async', 'sync', or 'guarded'
    - `async` - Asynchronous commands are added to the message queue of the executing component and are executed in the component's thread 
    - `sync` - Synchronous commands invoked immediately in the thread of the caller
    - `guarded` - A synchronous command that is protected by a #TBD
- `opcode` - A unique numeric value for the command (integer)
- `param-list` - A list of arguments for the command
    - `identifier` - The unique name of the parameter/argument
    - `type` - Limited to "displayable types", and not arrays or structures. (INT, UINT, BOOL, ENUM, STRING)
- `priority` - The priority of the command if it is of command-kind 'async'
- `queue-full-behavior` - Specifies the behavior of the command when the queue is full. Only applicable to 'async' commands. 
    - `assert` - Results in a system assert if the queue is full
    - `drop` - Drops the command if the queue is full
    - `block` - Blocks the command if the queue is full
    - `hook` - Calls a hook function if the queue is full

## Telemetry [FPP User Guide](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Telemetry)

Telemetry are specified via the FPP modeling language and can include the following options:

- `identifier` - The unique name of the telemetry
- `type-name` - Limited to "displayable types"
- `id` - A unique numeric value for the telemetry (integer)
- `format` - Format string for the telemetry
- `telemetry-update` - Whether the telemetry is updated always or on change
- `telemetry-limit-sequence` - Provides low and high limits for the telemetry
    - Can be specified for red, orange, and yellow limits
    - Type of telemetry must be numeric

## Telemetry Packets [FPP User Guide](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Topologies_Telemetry-Packets)

Telemetry packets are specified via the FPP modeling language and can include the following options:

- `identifier` - The unique name of the telemetry packet
- `id` - A unique numeric value for the telemetry packet (integer)
- `group` - The group of the telemetry packet
- `telemetry-packet-member-sequence` - Can include individual telemetry indentifiers and/or imports of other telemetry packet fpp definitions

## Events [FPP User Guide](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Telemetry)

Events are specified via the FPP modeling language and can include the following options:

- `identifier` - The unique name of the event
- `severity` - The severity of the event (fatal, warning high, warning low, activity high, activity low, command, diagnostic)
- `format` - Format string for the event
- `throttle` - Defines the max number of events that can be reported in a given time period (also defines the time period)
- `throttle-time` - Defines the time period for the throttle (integer)
- `param-list` - A list of arguments for the command
    - `identifier` - The unique name of the parameter/argument
    - `type` - Limited to "displayable types" (INT, UINT, BOOL, ENUM, STRING, ARRAY, STRUCT)

## Parameters [FPP User Guide](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Parameters)

Parameters are specific variables stored in non-volatile memory. Parameters are specified via the FPP modeling language and can include the following options:

- `identifier` - The unique name of the parameter
- `type` - Limited to "displayable types" (INT, UINT, BOOL, ENUM, STRING, ARRAY, STRUCT)
- `default` - The default value of the parameter (optional)
- `id` - A unique numeric value for the parameter (integer)
- `set opcode` - The opcode for setting the parameter in volatile memory (integer)
- `save opcode` - The opcode for saving the parameter to non-volatile memory (integer)

## Data Products [FPP User Guide](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Components_Data-Products)

Data products are structured file data for downlink. They defined via the FPP modeling language as containers and data product records.

Containers define the existence of the data product and can include the following options:

- `identifier` - The unique name of the container
- `id` - A unique numeric value for the container (integer)
- `default priority` - The default priority of the container

Data Product Records define the content of the data products and can include the following options:

- `identifier` - The unique name of the data product record
- `id` - A unique numeric value for the data product record (integer)


