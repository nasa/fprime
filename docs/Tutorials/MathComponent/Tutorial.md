# Math Component Tutorial

The following example shows the steps to implement a simple pair of components connected by a pair of ports. The first,
`MathSender`, will invoke the second, `MathReceiver`, via  a `MathOp` port to perform a math operation and return the
result via a `MathResult` port.

![`Component` Diagram](img/Comp.jpg "Component Pair")

All the code in this tutorial can be found in this directory. This code will work if it is copied or reimplemented
within the `Ref` directory of F´. This tutorial will walk the user through modifying the Reference app, Ref, to
implement the math components.

## Prerequisites

This tutorial requires the user to have some basic skills and have installed F´. The prerequisite skills to understand
this tutorial are as follows:

1) Working knowledge of Unix; how to navigate in a shell and execute programs
2) An understanding of C++, including class declarations and inheritance
3) An understanding of how XML is structured

Installation can be done by following the installation guide found at: [INSTALL.md](../../INSTALL.md). This guide
will walk the user through the installation process and verifying the installation.  In addition, users may wish to
follow the [Getting Started Tutorial](../GettingStarted/Tutorial.md) in order to get a feel for the F´ environment and
tools.

# 1 Component Descriptions

This section will cover the components that will be built in this tutorial. Each component may define the commands,
events, telemetry channels, and parameters. This section will list commands, events, telemetry channels, and parameters
for each component that will be created.

## 1.1 MathSender
`MathSender` must do the following:

### 1.1.1 Commands

`MathSender` should implement a `MS_DO_MATH` command. This command will have three arguments:
1) A first value in the operation
2) A second value in the operation
3) An enumerated argument specifying the operation to perform

### 1.1.2 Events
`MathSender` should emit an event telling that a command was received to perform the operation. It should also emit an event when the result is received back from `MathReceiver`.

### 1.1.3 Telemetry Channels
MathSender should have four channels:
1) The first value
2) The second value
3) The operation
4) The result

### 1.1.4 Parameters
MathSender will have no parameters.

`MathSender` should be an active (i.e. threaded) component, so it will process the commands immediately. The command will be *asynchronous*, which means the handler will be executed on the thread of the active component. It will delegate the operation to `MathReceiver`.

## 1.2 MathReceiver

`MathReceiver` will be a queued component that performs the requested operation and returns the result. `MathReceiver` will be connected to the 1Hz rate group that is part of the reference example. The simple operation in this component could have just as easily been done in a passive or active component; it is done here as a queued component to illustrate how to implement one.

### 1.2.1 Commands

`MathReceiver` should implement a MR_SET_FACTOR1 command. This command will set a factor used for any subsequent operations. The result of the commanded operation will be multiplied by this factor. It should default to 0 if the command is never invoked.

`MathReceiver` should also implement a MR_CLEAR_EVENT_THROTTLE command to clear the throttled MR_SET_FACTOR1 event (see below).

### 1.2.2 Events

`MathReceiver` should have the following events:

1) MR_SET_FACTOR1 command event. When the command is received, `MathReceiver` should emit an event with the updated factor. The event should be throttled (i.e. stop emitting) after three invocations. Normally, throttling is used to prevent event floods if there a endlessly repeating condition.
2) MR_UPDATED_FACTOR2 event. When the factor2 parameter (see below) is updated, `MathReceiver` should emit an event with the updated value.
3) MR_OPERATION_PERFORMED event. When the component receives a request to perform the operation, it should emit an event with the arguments and operation.
4) MR_THROTTLE_CLEARED in response to the MR_CLEAR_EVENT_THROTTLE command above.

### 1.2.3 Channels

`MathReceiver` should have the following channels:

1) A channel that has a serializable structure argument that contains the two terms in the operation as well as the operation and the result. This will be used to illustrate an XML defined serializable as a single telemetry channel.
2) A channel that counts the number of MR_SET_FACTOR1 commands received, so that a count can be known past the throttled event.
3) A channel for each of the factors used in the operation.

### 1.2.4 Parameters

`MathReceiver` will have one parameter, a second factor used in the operation.

## 1.3 Operation

`MathReceiver` will perform the following operation when requested by `MathSender`:

result = (value1 operation value2)*factor1/factor2

# 2 Implementation

This section will cover the implementation of the components for this tutorial. The implementation of these components
will have the following steps:

1) Define the `MathOpPort` and 'MathResultPort' ports that are used between the components.
2) Define the `MathSender` component in XML and compile it.
3) Implement the `MathSender` derived implementation class.
4) Unit test the `MathSender` implementation component.
5) Define the `MathReceiver` component in XML.
6) Implement the `MathReceiver` implementation class.
7) Unit test the `MathReceiver` implementation class.
8) Connect the classes to the `Ref` topology.
9) Run the ground system and exercise the commands and view the telemetry and events in the GUI.

## 2.1 Port definition

There are two ports to define in order to perform the operation between the components. The XML for the ports will be first shown in their entirety, and then the individual parts will be described.

### 2.1.1 MathOpPort

`MathOpPort` is responsible for passing the invocation of the operation from `MathSender` to `MathReceiver`. The new XML file should be placed in a new directory `Ref/MathPorts` with the name `MathOpPortAi.xml`. The XML for the port is as follows:

```xml
<interface name="MathOp" namespace="Ref">
    <comment>
    Port to perform an operation on two numbers
    </comment>
    <args>
        <arg name="val1" type="F32">
		</arg>
        <arg name="val2" type="F32">
		</arg>
        <arg name="operation" type="ENUM">
            <enum name="MathOperation">
                <item name="MATH_ADD"/>
                <item name="MATH_SUB"/>
                <item name="MATH_MULTIPLY"/>
                <item name="MATH_DIVIDE"/>
            </enum>
            <comment>operation argument</comment>
        </arg>
    </args>
</interface>
```

#### 2.1.1.1 Port Name Specification

```xml
<interface name="MathOp" namespace="Ref">
    <comment>
    Port to perform an operation on two numbers
    </comment>
...
</interface>
```

The `interface` tag specifies that a port is being defined. The attributes are as follows:

|Attribute|Description|
|---|---|
|name|The name of the component type. Becomes the C++ class name|
|namespace|The namespace of the component. The C++ namespace the where the component class will appear|

#### 2.1.1.2 Port Argument Specification

The port arguments are passed from component to component when they are connected. The port argument XML is as follows:

```xml
    <args>
        <arg name="val1" type="F32">
		</arg>
        <arg name="val2" type="F32">
		</arg>
        <arg name="operation" type="ENUM">
            <enum name="MathOperation">
                <item name="MATH_ADD"/>
                <item name="MATH_SUB"/>
                <item name="MATH_MULTIPLY"/>
                <item name="MATH_DIVIDE"/>
            </enum>
            <comment>operation argument</comment>
        </arg>
    </args>
```

The `<args>` tag begins the section of the XML defining the arguments, while the `<arg>` tag defines a particular argument. The port argument attributes are define as follows:

|Attribute|Description|
|---|---|
|name|The name of the argument. Becomes the argument name in the C++ call|
|type|The type of the arguments. Can be one of the built-in types, a user define type, or an enumeration|

The enumerations are a special type of argument. When `type="ENUM"` is an attribute of the arguments, a further listing of the elements of the enumeration are needed. For each element of the array, a name is specified. These end up being C++ enumerated types.

```xml
           <enum name="MathOperation">
                <item name="MATH_ADD"/>
                <item name="MATH_SUB"/>
                <item name="MATH_MULTIPLY"/>
                <item name="MATH_DIVIDE"/>
            </enum>
 ```
#### 2.1.1.3 Adding the port to the build

The build system needs to be made aware of the port XML. To do this, the user needs to create a `CMakeLists.txt` file in
the directory of the port. Create a file named `CMakeLists.txt` in the `MathPorts` directory. This file tells the build
system that a new file needs to be added to the build. Here are the contents:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathOpPortAi.xml"
)
register_fprime_module()
```

Here the source files for this module are listed. In the case of ports, only the Ai.xml file is needed. The next step is
to add the `MathPorts` to the `Ref` project.

The port can be added into the `Ref` project by editing the `Ref/CMakeLists.txt`. This will add the port directory into
the directories available to the `Ref` build.  Find the following lines in `Ref/CMakeLists.txt` and append a record with
the current directory.

```cmake
...
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SendBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SignalGen/")
```

The file after modification should look like the following:

```cmake
...
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SendBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SignalGen/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MathPorts/")
```

Now that the ports directory is part of the build system, the port can be built. If you have not already generated a
build directory for `Ref` as described in the "INSTALL.md" and the "Getting Started" tutorial, then run the following
commands to generate a build.

```shell
# Change to Ref directory
cd fprime/Ref
fprime-util generate
```

Now, the port code can be generated and compiled:

```shell
# Change to the MathPorts directory from Ref
cd MathPorts
fprime-util build
```

The code generation from the XML produces two files, both of which are part of the generated build directory:

```
 MathOpPortAc.cpp
 MathOpPortAc.hpp
```
These contain the C++ classes that implement the port functionality. The build system will automatically compile them when it is aware of the port XML file.

### 2.1.2 MathResultPort

`MathResultPort` is responsible for passing the result of the operation from `MathReceiver` to `MathSender`. The new XML file should be placed in the `Ref/MathPorts` directory with the name `MathResultPortAi.xml`. The XML for the port is as follows:

```xml
<interface name="MathResult" namespace="Ref">
    <comment>
    Port to return the result of a math operation
    </comment>
    <args>
        <arg name="result" type="F32">
            <comment>the result of the operation</comment>
        </arg>
    </args>
</interface>

```

This file can be added to the `CMakeLists.txt` in the `Ref/MathPorts` directory:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathOpPortAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathResultPortAi.xml"

)
register_fprime_module()
```

Running `fprime-util build` as before will compile the new port XML file and generate the C++ Autogenerated
files. The code generated to implement ports is complete. Developers do not need to add any implementation code of their
own to port definitions.

## 2.2 Serializable Definition

A structure needs to be defined that represents the channel value needed by `MathReceiver`. All port calls, telemetry channels, events and parameters need to be comprised of `Serializable` values, or values that can be turned into a byte stream. This is needed to pass port arguments through message queues and to pass commands and telemetry to and from the ground system. Built-in basic types like integers, floating point numbers and boolean values are supported by the framework, but there are times when a developer wishes to use a custom-defined type, perhaps to keep members of a object consistent with each other. These structures can be defined in XML and the code generator will generate the C++ classes with all the necessary serialization functions. Developers can hand-code their own, but they are not usable for telemetry since the ground system needs an XML definition to decode them.

### 2.2.1 MathOp

The `MathOp` serializable structure is needed by `MathReceiver` for a telemetry channel that gives the values of the operation. A new directory named `Ref/MathTypes` should be created for the structure, and the file should be named `MathOpSerializableAi.xml`. The XML is as follows:

```xml
<serializable namespace="Ref" name="MathOp">
    <comment>
    This value holds the values of a math operation
    </comment>
    <members>
        <member name="val1" type="F32"/>
        <member name="val2" type="F32"/>
        <member name="op" type="ENUM">
            <enum name="Operation">
                <item name="ADD"/>
                <item name="SUB"/>
                <item name="MULT"/>
                <item name="DIVIDE"/>
            </enum>
        </member>
        <member name="result" type="F32"/>
    </members>
</serializable>
```

Add a `CMakeLists.txt` file for the serializable:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathOpSerializableAi.xml"
)
register_fprime_module()
```

#### 2.2.1.1 Serializable Name Specification

The opening tag of the XML specifies the type name and namespace of the structure:

```xml
<serializable namespace="Ref" name="MathOp">
...
</serializable>
```

#### 2.2.1.2 Serializable Members

The `members` tag starts the section of the XML that specifies the members of the structure:

```xml
    <members>
        <member name="val1" type="F32"/>
        <member name="val2" type="F32"/>
        <member name="op" type="ENUM">
            <enum name="Operation">
                <item name="ADD"/>
                <item name="SUB"/>
                <item name="MULT"/>
                <item name="DIVIDE"/>
            </enum>
        </member>
        <member name="result" type="F32"/>
    </members>
```

As with the arguments to port definitions, built-in types can be specified as well as enumerations.

As before with the port definitions, the `Ref/MathTypes` directory needs to be added to `Ref/CMakeLists.txt`.

```cmake
...
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SendBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SignalGen/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MathPorts/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MathTypes/")
```

This XML defined structure compiles to a C++ class that has accessors for the members of the structure.

## 2.3 Component Definition

### 2.3.1 MathSender Component

The `MathSender` component XML definition is as follows. The XML should be placed in a file `Ref/MathSender/MathSenderComponentAi.xml`

```xml
<component name="MathSender" kind="active" namespace="Ref">
    <import_port_type>Ref/MathPorts/MathOpPortAi.xml</import_port_type>
    <import_port_type>Ref/MathPorts/MathResultPortAi.xml</import_port_type>
    <comment>Component sending a math operation</comment>
    <ports>
        <port name="mathOut" data_type="Ref::MathOp" kind="output">
            <comment>
            Port for sending the math operation
            </comment>
        </port>
        <port name="mathIn" data_type="Ref::MathResult" kind="async_input">
            <comment>
            Port for returning the math result
            </comment>
        </port>
    </ports>
    <commands>
        <command kind="async" opcode="0" mnemonic="MS_DO_MATH">
            <comment>
            Do a math operation
            </comment>
            <args>
                <arg name="val1" type="F32">
                    <comment>The first value</comment>
                </arg>
                <arg name="val2" type="F32">
                    <comment>The second value</comment>
                </arg>
                <arg name="operation" type="ENUM">
                    <enum name="MathOp">
                        <item name="ADD"/>
                        <item name="SUBTRACT"/>
                        <item name="MULTIPLY"/>
                        <item name="DIVIDE"/>
                    </enum>
                    <comment>The operation to perform</comment>
                </arg>
             </args>
        </command>
    </commands>
    <telemetry>
        <channel id="0" name="MS_VAL1" data_type="F32">
            <comment>
            The first value
            </comment>
        </channel>
        <channel id="1" name="MS_VAL2" data_type="F32">
            <comment>
            The second value
            </comment>
        </channel>
        <channel id="2" name="MS_OP" data_type="ENUM">
            <enum name="MathOpTlm">
                <item name="ADD_TLM"/>
                <item name="SUB_TLM"/>
                <item name="MULT_TLM"/>
                <item name="DIV_TLM"/>
            </enum>
            <comment>
            The operation
            </comment>
        </channel>
        <channel id="3" name="MS_RES" data_type="F32">
            <comment>
            The result
            </comment>
        </channel>
    </telemetry>
    <events>
        <event id="0" name="MS_COMMAND_RECV" severity="ACTIVITY_LO" format_string="Math Cmd Recvd: %f %f %d"  >
            <comment>
            Math command received
            </comment>
            <args>
                <arg name="val1" type="F32">
                    <comment>The val1 argument</comment>
                </arg>
                <arg name="val2" type="F32">
                    <comment>The val2 argument</comment>
                </arg>
                <arg name="op" type="ENUM">
                    <comment>The requested operation</comment>
                <enum name="MathOpEv">
                    <item name="ADD_EV"/>
                    <item name="SUB_EV"/>
                    <item name="MULT_EV"/>
                    <item name="DIV_EV"/>
                </enum>
                </arg>
            </args>
        </event>
        <event id="1" name="MS_RESULT" severity="ACTIVITY_HI" format_string = "Math result is %f" >
            <comment>
            Received math result
            </comment>
            <args>
                <arg name="result" type="F32">
                    <comment>The math result</comment>
                </arg>
            </args>
        </event>
    </events>
</component>
```

#### 2.3.1.1 Component Name Specification

The component name is specified in the opening tag of the XML:

```xml
<component name="MathSender" kind="active" namespace="Ref">
...
</component>
```

The attributes of the tag are as follows:

|Attribute|Description|
|---|---|
|name|The component name|
|kind|What the threading/queuing model of the component is. Can be `passive`, `queued`, or `active`|
|namespace|The C++ namespace the component will be defined in|

#### 2.3.1.2 Port Imports

The ports needed for the component are imported using `import_port_type` tags:

```xml
    <import_port_type>Ref/MathPorts/MathOpPortAi.xml</import_port_type>
    <import_port_type>Ref/MathPorts/MathResultPortAi.xml</import_port_type>
```

The path in the port import statement is relative to the root of the repository. There are a number of ports automatically included by the code generator when commands, telemetry, events or parameters are defined. They are:

|Facility|Ports|
|---|---|
|Commands|`Fw/Command/CmdPortAi.xml`,`Fw/Command/CmdResponsePortAi.xml`,`Fw/Command/CmdRegPortAi.xml`|
|Events|`Fw/Log/LogPortAi.xml`,`Fw/Log/LogTextPortAi.xml`|
|Telemetry|`Fw/Tlm/TlmPortAi.xml`|
|Parameters|`Fw/PrmGetPortAi.xml`,`Fw/PrmSetPortAi.xml`|

#### 2.3.1.3 Port Declarations

Ports and their attributes are declared once the port definitions are included.

```xml
    <ports>
        <port name="mathOut" data_type="Ref::MathOp" kind="output">
            <comment>
            Port for sending the math operation
            </comment>
        </port>
        <port name="mathIn" data_type="Ref::MathResult" kind="async_input">
            <comment>
            Port for returning the math result
            </comment>
        </port>
    </ports>
```

The port attributes are:

|Attribute|Description|
|---|---|
|name|The port name|
|data_type|The type of the port as defined in the included port definitions, in the form `namespace::name`|
|kind|The kind of port. Can be `sync_input`,`async_input`,`guarded_input`, or `output`|

For `MathSender`, the request for the operation will be sent on the `mathOut` output port, and the result will be returned on the `mathIn` asynchronous port. Because the component is active and the result input port is asynchronous, the port handler will execute on the thread of `MathSender`.

#### 2.3.1.4 Command Declarations

The commands defined for the component are:

```xml
    <commands>
        <command kind="async" opcode="0" mnemonic="MS_DO_MATH">
            <comment>
            Do a math operation
            </comment>
            <args>
                <arg name="val1" type="F32">
                    <comment>The first value</comment>
                </arg>
                <arg name="val2" type="F32">
                    <comment>The second value</comment>
                </arg>
                <arg name="operation" type="ENUM">
                    <enum name="MathOp">
                        <item name="ADD"/>
                        <item name="SUBTRACT"/>
                        <item name="MULTIPLY"/>
                        <item name="DIVIDE"/>
                    </enum>
                    <comment>The operation to perform</comment>
                </arg>
             </args>
        </command>
    </commands>
```

The `<command>` tag starts the section containing commands for `MathSender`. For each command, the following attributes are defined:

|Attribute|Description|
|---|---|
|mnemonic|A text version of the command name, used in sequences and the ground tool|
|opcode|A numeric value for the command. The value is relative to a base value set when the component is added to a topology|
|kind|The kind of command. Can be `sync_input`,`async_input`,`guarded_input`, or `output`|

#### 2.3.1.5 Telemetry

The telemetry XML is as follows:

```xml
    <telemetry>
        <channel id="0" name="MS_VAL1" data_type="F32">
            <comment>
            The first value
            </comment>
        </channel>
        <channel id="1" name="MS_VAL2" data_type="F32">
            <comment>
            The second value
            </comment>
        </channel>
        <channel id="2" name="MS_OP" data_type="ENUM">
            <enum name="MathOpTlm">
                <item name="ADD_TLM"/>
                <item name="SUB_TLM"/>
                <item name="MULT_TLM"/>
                <item name="DIV_TLM"/>
            </enum>
            <comment>
            The operation
            </comment>
        </channel>
        <channel id="3" name="MS_RES" data_type="F32">
            <comment>
            The result
            </comment>
        </channel>
    </telemetry>
```

The `<telemetry>` tag starts the section containing telemetry channels for `MathSender`. For each channel, the following attributes are defined:

|Attribute|Description|
|---|---|
|name|The channel name|
|id|A numeric value for the channel. The value is relative to a base value set when the component is added to a topology|
|data_type|The data type of the channel. Can be a built-in type, an enumeration or an externally defined serializable type|

#### 2.3.1.6 Events

The XML for the defined events is as follows:

```xml
    <events>
        <event id="0" name="MS_COMMAND_RECV" severity="ACTIVITY_LO" format_string = "Math Cmd Recvd: %f %f %d"  >
            <comment>
            Math command received
            </comment>
            <args>
                <arg name="val1" type="F32">
                    <comment>The val1 argument</comment>
                </arg>
                <arg name="val2" type="F32">
                    <comment>The val1 argument</comment>
                </arg>
                <arg name="op" type="ENUM">
                    <comment>The requested operation</comment>
                <enum name="MathOpEv">
                    <item name="ADD_EV"/>
                    <item name="SUB_EV"/>
                    <item name="MULT_EV"/>
                    <item name="DIV_EV"/>
                </enum>
                </arg>
            </args>
        </event>
        <event id="1" name="MS_RESULT" severity="ACTIVITY_HI" format_string = "Math result is %f" >
            <comment>
            Received math result
            </comment>
            <args>
                <arg name="result" type="F32">
                    <comment>The math result</comment>
                </arg>
            </args>
        </event>
    </events>
```

The `<events>` tag starts the section containing events for `MathSender`. For each event, the following attributes are defined:

|Attribute|Description|
|---|---|
|name|The event name|
|severity|The severity of the event. Can be DIAGNOSTIC, ACTIVITY_LO, ACTIVITY_HI, WARNING_LO, WARNING_HI or FATAL.
|id|A numeric value for the event. The value is relative to a base value set when the component is added to a topology|
|format_string|A C-style format string for displaying the event and the argument values.|

The directory containing the component XML can be added to the list of modules in `Ref/CMakeLists.txt`:

```cmake
...
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/PingReceiver/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/RecvBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SendBuffApp/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/SignalGen/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MathPorts/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MathTypes/")
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MathSender/")
```

Create a `CMakeLists.txt` file in `Ref/MathSender` and add `MathSenderComponentAi.xml`.

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
)

register_fprime_module()
```

Once it is added, add the directory to the build and build the component by typing `fprime-util build` from the `Ref` directory.

### 2.3.2 MathReceiver Component

#### 2.3.2.1 Component Specification

The `MathReceiver` component XML is as follows:

```xml
<component name="MathReceiver" kind="queued" namespace="Ref">
    <import_port_type>Ref/MathPorts/MathOpPortAi.xml</import_port_type>
    <import_port_type>Ref/MathPorts/MathResultPortAi.xml</import_port_type>
    <import_port_type>Svc/Sched/SchedPortAi.xml</import_port_type>
    <import_serializable_type>Ref/MathTypes/MathOpSerializableAi.xml</import_serializable_type>
    <comment>Component sending a math operation</comment>
    <ports>
        <port name="mathIn" data_type="Ref::MathOp" kind="async_input">
            <comment>
            Port for receiving the math operation
            </comment>
        </port>
        <port name="mathOut" data_type="Ref::MathResult" kind="output">
            <comment>
            Port for returning the math result
            </comment>
        </port>
        <port name="SchedIn" data_type="Sched" kind="sync_input">
            <comment>
            The rate group scheduler input
            </comment>
        </port>
    </ports>
    <commands>
        <command kind="async" opcode="0" mnemonic="MR_SET_FACTOR1">
            <comment>
            Set operation multiplication factor1
            </comment>
            <args>
                <arg name="val" type="F32">
                    <comment>The first factor</comment>
                </arg>
             </args>
        </command>
        <command kind="async" opcode="1" mnemonic="MR_CLEAR_EVENT_THROTTLE">
            <comment>Clear the event throttle
            </comment>
        </command>
    </commands>
    <telemetry>
        <channel id="0" name="MR_OPERATION" data_type="Ref::MathOp">
            <comment>
            The operation
            </comment>
        </channel>
        <channel id="1" name="MR_FACTOR1S" data_type="U32">
            <comment>
            The number of MR_SET_FACTOR1 commands
            </comment>
        </channel>
        <channel id="2" name="MR_FACTOR1" data_type="F32">
            <comment>
            Factor 1 value
            </comment>
        </channel>
        <channel id="3" name="MR_FACTOR2" data_type="F32">
            <comment>
            Factor 2 value
            </comment>
        </channel>
    </telemetry>
    <events>
        <event id="0" name="MR_SET_FACTOR1" severity="ACTIVITY_HI" format_string = "Factor 1: %f"  throttle = "3"   >
            <comment>
            Operation factor 1
            </comment>
            <args>
                <arg name="val" type="F32">
                    <comment>The factor value</comment>
                </arg>
            </args>
        </event>
        <event id="1" name="MR_UPDATED_FACTOR2" severity="ACTIVITY_HI" format_string = "Factor 2 updated to: %f" >
            <comment>
            Updated factor 2
            </comment>
            <args>
                <arg name="val" type="F32">
                    <comment>The factor value</comment>
                </arg>
            </args>
        </event>
        <event id="2" name="MR_OPERATION_PERFORMED" severity="ACTIVITY_HI" format_string = "Operation performed: %s" >
            <comment>
            Math operation performed
            </comment>
            <args>
                <arg name="val" type="Ref::MathOp">
                    <comment>The operation</comment>
                </arg>
            </args>
        </event>
        <event id="3" name="MR_THROTTLE_CLEARED" severity="ACTIVITY_HI" format_string = "Event throttle cleared" >
            <comment>
            Event throttle cleared
            </comment>
        </event>
    </events>
    <parameters>
        <parameter id="0" name="factor2" data_type="F32" default="1.0" set_opcode="10" save_opcode="11">
            <comment>
            A test parameter
            </comment>
        </parameter>
    </parameters>

</component>
```


The `CMakeLists.txt` file for this component is as follows:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathReceiverComponentAi.xml"
)

register_fprime_module()
```

Don't forget to `Ref/MathReceiver` to `Ref/CMakeLists.txt`.


Many of the elements are the same as described in `MathSender`, so this section will highlight the differences.

#### 2.3.2.1 Queued component

The `MathReceiver` component is queued, which means it can receive asynchronous port invocations as messages, but needs an external thread to dequeue them.

#### 2.3.2.2 Importing the serializable type

The telemetry channels and events use a serializable type, `Ref::MathOp` to illustrate the use of those types. The following line specifies the import for this type:

```xml
   <import_serializable_type>Ref/MathTypes/MathOpSerializableAi.xml</import_serializable_type>
```

This type is then available for events and channels, but are not available for parameters and command arguments.

#### 2.3.2.3 Scheduler port

The queued component has a scheduler port that is `sync_input`. That means the port invocation is not put on a message queue, but calls the handler on the thread of the caller of the port:

```xml
        <port name="SchedIn" data_type="Sched" kind="sync_input">
            <comment>
            The rate group scheduler input
            </comment>
        </port>

```

This synchronous call allows the caller to pull any pending messages of the message queue using the thread of the component invoking the `SchedIn` port.

#### 2.3.2.4 Throttled Event

The `MR_SET_FACTOR1` event has a new argument `throttle = "3"` that specifies how many events will be emitted before the event is throttled so no more appear.

```xml
        <event id="0" name="MR_SET_FACTOR1" severity="ACTIVITY_HI" format_string = "Factor 1: %f"  throttle = "3"   >
            <comment>
            Operation factor 1
            </comment>
            <args>
                <arg name="val" type="F32">
                    <comment>The factor value</comment>
                </arg>
            </args>
        </event>
```

#### 2.3.2.5 Parameters

The `MathReceiver` component has a declaration for a parameter:

```xml
    <parameters>
        <parameter id="0" name="factor2" data_type="F32" default="1.0" set_opcode="10" save_opcode="11">
            <comment>
            A test parameter
            </comment>
        </parameter>
    </parameters>

```

The `parameter` attributes are as follows:

|Attribute|Description|
|---|---|
|id|The unique parameter ID. Relative to base ID set for the component in the topology|
|name|The parameter name|
|data_type|The data type of the parameter. Must be a built-in type|
|default|Default value assigned to the parameter if there is an error retrieving it.|
|set_opcode|The opcode of the command to set the parameter. Must not overlap with any of the command opcodes|
|save_opcode|The opcode of the command to save the parameter. Must not overlap with any of the command opcodes|



## 2.4 Component Implementation

The component implementation consists of writing a class that is derived from the code-generated base class and filling in member functions that implement the port calls.

### 2.4.1 MathSender Implementation

#### 2.4.1.1 Stub Generation

There is a F´ utility command that will generate stubs that the developer can fill in. The command to generate the stubs is: `fprime-util impl`.
This should be run in the directory for the MathSender component, and will generate two files:

```
MathSenderComponentImpl.hpp-template
MathSenderComponentImpl.cpp-template
```

Rename the files by removing the `-template` from the end of the file names.

```
MathSenderComponentImpl.hpp
MathSenderComponentImpl.cpp
```

Add the new files to the MathSender's `CMakeLists.txt` file:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentImpl.cpp"
)
register_fprime_module()
```

Now attempt to build the component with:

```
fprime-util build
```

The stub files should successfully compile.

#### 2.4.1.2 Handler implementation

The next step is to fill in the handler with implementation code.

First, find the empty command handler in the `MathSenderComponentImpl.cpp` file:

```c++
  void MathSenderComponentImpl ::
    MS_DO_MATH_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        F32 val1,
        F32 val2,
        MathOp operation
    )
  {
    // TODO
  }
```
Then, fill in the function with the code to perform the functions described at the beginning of the tutorial:

```c++
  void MathSenderComponentImpl ::
    MS_DO_MATH_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        F32 val1,
        F32 val2,
        MathOp operation
    )
  {
    MathOpTlm opTlm;
    MathOperation opPort;
    MathOpEv opEv;
    switch (operation) {
      case ADD:
          opTlm = ADD_TLM;
          opPort = MATH_ADD;
          opEv = ADD_EV;
          break;
      case SUBTRACT:
          opTlm = SUB_TLM;
          opPort = MATH_SUB;
          opEv = SUB_EV;
          break;
      case MULTIPLY:
          opTlm = MULT_TLM;
          opPort = MATH_MULTIPLY;
          opEv = MULT_EV;
          break;
      case DIVIDE:
          opTlm = DIV_TLM;
          opPort = MATH_DIVIDE;
          opEv = DIV_EV;
          break;
      default:
          FW_ASSERT(0,operation);
          break;
    }

    this->tlmWrite_MS_OP(opTlm);
    this->tlmWrite_MS_VAL1(val1);
    this->tlmWrite_MS_VAL2(val2);
    this->log_ACTIVITY_LO_MS_COMMAND_RECV(val1,val2,opEv);
    this->mathOut_out(0,val1,val2,opPort);
    // reply with completion status
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

```

The handler will send the appropriate events and telemetry values, then invoke the output math operation port to request the operation.
Note that each channel and event argument that has an enumeration has a unique type declaration.
Finally, note that the output command response port must be called with a command status in order to let the framework components know that the command is complete.
If the completion status isn't sent, it will stall any sequences the command was part of.
There are command error status along with successful completions.
Most commands return this status at the end of the handler, but component implementations can store the `opCode` and `cmdSeq` values to return later, but those specific values must be returned in order to match the status with the command originally sent.

Find the empty result handler:

```c++
  void MathSenderComponentImpl ::
    mathIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
    // TODO
  }
```

Fill in the result handler with code that reports telemetry and an event:

```c++
  void MathSenderComponentImpl ::
    mathIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
      this->tlmWrite_MS_RES(result);
      this->log_ACTIVITY_HI_MS_RESULT(result);
  }

```

This handler reports the result via a telemetry channel and an event.

Once complete, add the directory to the build and build the component by typing `fprime-util build` from the `Ref` directory.

#### 2.4.1.3 Unit Tests

Unit Tests are used to exercise the component's functions by invoking input ports and commands and checking the values of output ports, telemetry and events.

##### 2.4.1.3.1 Test Code Generation

The code generator will generate test components that can be connected to the component to enable a set of unit tests to check functionality and to get coverage of all the code. To generate a set of files for testing, from the module directory type:

```shell
fprime-util impl --ut
```

The files that are generated are:

```
Tester.hpp
Tester.cpp
TesterBase.hpp
TesterBase.cpp
GTestBase.hpp
GTestBase.cpp
```

The functions of the files are:

|File|Function|
|---|---|
|TesterBase.*|Base class for test class. Defines necessary handlers as well as helper functions
|GTestBase.*|Helper class derived from TesterBase that has macros that use Google Test to test interfaces|
|Tester.*|Derived tester class that inherits from GTestBase. Includes instance of the component and helpers to connect ports|

Unit tests are built in subdirectories of the module, so the unit test file must be copied there. The build system supports a standard subdirectory of `test/ut` below the module being tested. While in the MathSender directory, create the `test/ut` directory:

```
mkdir -p test/ut
```

Move the above set of files into that subdirectory.

The new unit test files have to be registered with the build system, so modifications to the `CMakeLists.txt` files are
necessary. To do this, add a "UT_SOURCE_FILES" variable to `CMakeLists.txt` followed by a call `register_fprime_ut()`.
The UT_SOURCE_FILES variable contains a list of the C++ files associated with the UT (see list above).

The final `CMakeLists.txt` file should look like the following:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathSenderComponentImpl.cpp"
)
register_fprime_module()

set(UT_SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/main.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/Tester.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/TesterBase.cpp"
  "${CMAKE_CURRENT_LIST_DIR}/test/ut/GTestBase.cpp"
)
register_fprime_ut()
```
The `UT_SOURCE_FILES` variable includes any source code needed to run the test. It usually only includes the generated
test code and a `main.cpp`, but it can include any code the user needs to test.

A `UT_MODS` variable may be set should the UT depend on modules not automatically included by the component.

##### 2.4.1.3.2 Test Code Implementation

The `main.cpp` file must be added. For this test, it appears like this:

```c++
#include "Tester.hpp"

TEST(Nominal, AddOperationTest) {
    Ref::Tester tester;
    tester.testAddCommand();
}

TEST(Nominal, SubOperationTest) {
    Ref::Tester tester;
    tester.testSubCommand();
}

TEST(Nominal, MultOperationTest) {
    Ref::Tester tester;
    tester.testMultCommand();
}

TEST(Nominal, DivideOperationTest) {
    Ref::Tester tester;
    tester.testDivCommand();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

```

F' uses the Google Test framework to run unit tests. For more information about the Google Test Framework see here:

https://github.com/google/googletest


In the Google Test framework, the following lines of code are standard:

```c++
int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
```

For each test, there is a Google Test macro defined:

```c++
TEST(Name1, Name2) {
    // run some code
}
```

The code in each of the macros defined this way will automatically be run be the framework.

In this case, the tests are defined as follows:

```c++
TEST(Nominal, AddOperationTest) {
    Ref::Tester tester;
    tester.testAddCommand();
}

TEST(Nominal, SubOperationTest) {
    Ref::Tester tester;
    tester.testSubCommand();
}

TEST(Nominal, MultOperationTest) {
    Ref::Tester tester;
    tester.testMultCommand();
}

TEST(Nominal, DivideOperationTest) {
    Ref::Tester tester;
    tester.testDivCommand();
}
```
For each unit test, the Google Test test case for F' components looks like:

```c++
TEST(Nominal, DivideOperationTest) {
    NameSpace::Tester tester;
    tester.someUnitTestFunc();
}
```
The test component is instantiated here:

```c++
    NameSpace::Tester tester;
```

This allows the component to start from an newly initialized state for each unit test.

The unit test is executed by calling a member function of the `tester` class:

```c++
    tester.someUnitTestFunc();
```

The `Tester.hpp` stub can be updated to include the declarations of the unit test functions:

```c++
    ...
    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Test operation command
      //!
      void testAddCommand(void);
      void testSubCommand(void);
      void testMultCommand(void);
      void testDivCommand(void);

    private:
    ...
```

The next step is to add the specific test cases to the `Tester.cpp` implementation file. It is important to note that the unit tests are designed to be single-threaded. The active components do not have their threads started, so any messages to asynchronous ports are manually retrieved from the message queue and dispatched to handlers. This makes testing simpler since the execution of the thread in response to port calls or commands does not need to be managed. Examples of this will be seen in the test code.

The first test case will be to test the `MS_DO_MATH` command for the addition operation. In the example component implementation, `MS_DO_MATH` command calls the `mathOut` output port and emits some channelized telemetry and events. The test component provides methods for invoking the command and checking that the telemetry and events were emitted as expected. The steps to write the test case are as follows:

Add a member function to the implementation class in `Tester.cpp` to implement the test case:

```c++
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  void Tester ::
    testAddCommand(void)
  {
      // send MS_DO_MATH command
      this->sendCmd_MS_DO_MATH(0,10,1.0,2.0,MathSenderComponentBase::ADD);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify that that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOut(0,1.0,2.0,MATH_ADD);
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_MS_VAL1_SIZE(1);
      ASSERT_TLM_MS_VAL2_SIZE(1);
      ASSERT_TLM_MS_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_MS_VAL1(0,1.0);
      ASSERT_TLM_MS_VAL2(0,2.0);
      ASSERT_TLM_MS_OP(0,MathSenderComponentBase::ADD_TLM);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_MS_COMMAND_RECV(0,1.0,2.0,MathSenderComponentBase::ADD_EV);
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0,MathSenderComponentBase::OPCODE_MS_DO_MATH,10,Fw::COMMAND_OK);

      // reset all telemetry and port history
      this->clearHistory();
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathIn(0,10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_MS_RES_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_MS_RES(0,10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_RESULT_SIZE(1);
      // verify the expected value of the event arguments
      ASSERT_EVENTS_MS_RESULT(0,10.0);
  }

```

Some highlights are:

Send the `MS_DO_MATH` command:

```c++
      // send MS_DO_MATH command
      this->sendCmd_MS_DO_MATH(0,10,1.0,2.0,MathSenderComponentBase::ADD);
      // retrieve the message from the message queue and dispatch
      this->component.doDispatch();
```

Verify that the operation port was called as expected:

```c++
      // verify that that only one output port was called
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      // verify that the math operation port was only called once
      ASSERT_from_mathOut_SIZE(1);
      // verify the arguments of the operation port
      ASSERT_from_mathOut(0,1.0,2.0,MATH_ADD);
```

The first call verifies that one and only one port call was made. This can be used to confirm that there were no other ports called besides the expected one.

The second call verifies that the port call that was made was the expected one.

The third call looks at a stored history of calls to this port and verifies the expected call arguments were made. The history can store multiple calls, so the first argument indicates which index in the history to examine.

Verify that the telemetry channels were written:

```c++
      // verify telemetry - 3 channels were written
      ASSERT_TLM_SIZE(3);
      // verify that the desired telemetry values were only sent once
      ASSERT_TLM_MS_VAL1_SIZE(1);
      ASSERT_TLM_MS_VAL2_SIZE(1);
      ASSERT_TLM_MS_OP_SIZE(1);
      // verify that the correct telemetry values were sent
      ASSERT_TLM_MS_VAL1(0,1.0);
      ASSERT_TLM_MS_VAL2(0,2.0);
      ASSERT_TLM_MS_OP(0,MathSenderComponentBase::ADD_TLM);
```
The first statement verifies that three channels were written as expected. The following statements verify that the correct channels were written with the expected values.

Verify that the event for the command was sent:

```c++
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_COMMAND_RECV_SIZE(1);
      // verify the correct event arguments were sent
      ASSERT_EVENTS_MS_COMMAND_RECV(0,1.0,2.0,MathSenderComponentBase::ADD_EV);

```

Next, verify that the correct response to the command was sent:

```c++
      // verify command response was sent
      ASSERT_CMD_RESPONSE_SIZE(1);
      // verify the command response was correct as expected
      ASSERT_CMD_RESPONSE(0,MathSenderComponentBase::OPCODE_MS_DO_MATH,10,Fw::COMMAND_OK);
```

Next, prepare for calling `MathSender`'s result port by clearing the port and telemetry history:

```c++
      // reset all telemetry and port history
      this->clearHistory();
```

As ports and commands are invoked in the component, the test component stores the history of calls. This function clears the history, in order to provide a clean slate for the next test. There are calls to clear individual histories as well. See `TesterBase.hpp` for a list. The `this->clearHistory()` call will clear them all, so is generally preferable.

The next step is to invoke the port that the `MathReceiver` component will call in the example program. For the unit test, the `MathReceiver` is not present to send the result back, so the unit test will emulate that call.

First, the port invocation is made:

```c++
      // call result port. We don't care about the value being correct since MathSender doesn't
      this->invoke_to_mathIn(0,10.0);
      // retrieve the message from the message queue and dispatch the command to the handler
      this->component.doDispatch();
```

Next, the test checks for the expected telemetry and events:

```c++
      // verify only one telemetry value was written
      ASSERT_TLM_SIZE(1);
      // verify the desired telemetry channel was sent only once
      ASSERT_TLM_MS_RES_SIZE(1);
      // verify the values of the telemetry channel
      ASSERT_TLM_MS_RES(0,10.0);
      // verify only one event was sent
      ASSERT_EVENTS_SIZE(1);
      // verify the expected event was only sent once
      ASSERT_EVENTS_MS_RESULT_SIZE(1);
      // verify the expected value of the event
      ASSERT_EVENTS_MS_RESULT(0,10.0);
```

The other test cases are similarly implemented for the other operations. See the tutorial code for their implementation.

You must first generate the unit test before you can build it. Enter:

```
fprime-util generate --ut
```

Once you generate the unit test, you can build the unit test by entering:

```
fprime-util build --ut
```

You can run the unit test by typing the following in the `MathSender` (not `test/ut`) directory:

```shell
$ fprime-util check
[==========] Running 4 tests from 1 test case.
[----------] Global test environment set-up.
[----------] 4 tests from Nominal
[ RUN      ] Nominal.AddOperationTest
[       OK ] Nominal.AddOperationTest (2 ms)
[ RUN      ] Nominal.SubOperationTest
[       OK ] Nominal.SubOperationTest (0 ms)
[ RUN      ] Nominal.MultOperationTest
[       OK ] Nominal.MultOperationTest (0 ms)
[ RUN      ] Nominal.DivideOperationTest
[       OK ] Nominal.DivideOperationTest (0 ms)
[----------] 4 tests from Nominal (3 ms total)

[----------] Global test environment tear-down
[==========] 4 tests from 1 test case ran. (3 ms total)
[  PASSED  ] 4 tests.

```

### 2.4.2 MathReceiver Implementation

#### 2.4.2.1 Component Implementation

As before, a stub can be generated:

```
cd fprime/Ref/MathReceiver
fprime-util impl
mv MathReceiverComponentImpl.cpp-template MathReceiverComponentImpl.cpp
mv MathReceiverComponentImpl.hpp-template MathReceiverComponentImpl.hpp
```

Add the stub files to `CMakeLists.txt`:

```cmake
set(SOURCE_FILES
  "${CMAKE_CURRENT_LIST_DIR}/MathReceiverComponentAi.xml"
  "${CMAKE_CURRENT_LIST_DIR}/MathReceiverComponentImpl.cpp"
)
register_fprime_module()
```

Add the files and compile them: `fprime-util build`

##### 2.4.2.1.1 Port handler

Look for the empty port handler in the sub class:

```c++
  void MathReceiverComponentImpl ::
    mathIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        F32 val2,
        MathOperation operation
    )
  {
    // TODO
  }
```

Fill the handler in with the computation of the result. The handler will also update telemetry and events:

```c++
  void MathReceiverComponentImpl ::
    mathIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        F32 val2,
        MathOperation operation
    )
  {
      // declare result serializable
      Ref::MathOp op;
      F32 res = 0.0;
      switch (operation) {
          case MATH_ADD:
              op.setop(ADD);
              res = (val1 + val2)*this->m_factor1;
              break;
          case MATH_SUB:
              op.setop(SUB);
              res = (val1 - val2)*this->m_factor1;
              break;
          case MATH_MULTIPLY:
              op.setop(MULT);
              res = (val1 * val2)*this->m_factor1;
              break;
          case MATH_DIVIDE:
              op.setop(DIVIDE);
              res = (val1 / val2)*this->m_factor1;
              break;
          default:
              FW_ASSERT(0,operation);
              break;
      }
      Fw::ParamValid valid;
      res = res/paramGet_factor2(valid);

      op.setval1(val1);
      op.setval2(val2);
      op.setresult(res);
      this->log_ACTIVITY_HI_MR_OPERATION_PERFORMED(op);
      this->tlmWrite_MR_OPERATION(op);
      this->mathOut_out(0,res);
  }

```

If needed, add `m_factor1` and `m_factor1s` as private variables in `MathReceiverComponentImpl.hpp`:


```c++
//! Implementation for MR_CLEAR_EVENT_THROTTLE command handler
//! Clear the event throttle
void MR_CLEAR_EVENT_THROTTLE_cmdHandler(
    const FwOpcodeType opCode, /*!< The opcode*/
    const U32 cmdSeq /*!< The command sequence number*/
);

// stored factor1
F32 m_factor1;
// number of times factor1 has been written
U32 m_factor1s;
```



In this handler, the operation is done based on the port arguments from `MathSender`.
The `op` structure is populated for the event and telemetry calls, and the `mathOut` port is called to send the result back to `MathSender`.
The parameter value is retrieved during initialization and is returned via the `paramGet_factor2()` call.
The commands to set and save the factor2 parameter run entirely in the code generated base classes.

##### 2.4.2.1.2 Commands

The command handler to update the value of `factor1` is as follows:

```c++
  void MathReceiverComponentImpl ::
    MR_SET_FACTOR1_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        F32 val
    )
  {
      this->m_factor1 = val;
      this->log_ACTIVITY_HI_MR_SET_FACTOR1(val);
      this->tlmWrite_MR_FACTOR1(val);
      this->tlmWrite_MR_FACTOR1S(++this->m_factor1s);
      // reply with completion status
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

```

The telemetry and log values are sent, and the command response is sent.
Note that after three calls to the handler, the `this->log_ACTIVITY_HI_MR_SET_FACTOR1(val)` call will not actually send any events until the throttle is cleared.
The throttled state is part of the generated code.

The handler to clear the throttle is as follows:

```c++
  void MathReceiverComponentImpl ::
    MR_CLEAR_EVENT_THROTTLE_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
      // clear throttle
      this->log_ACTIVITY_HI_MR_SET_FACTOR1_ThrottleClear();
      // send event that throttle is cleared
      this->log_ACTIVITY_HI_MR_THROTTLE_CLEARED();
      // reply with completion status
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }
```
##### 2.4.2.1.3 Scheduler Call

The port invoked by the scheduler retrieves the messages from the message queue and dispatches them.
The message dispatches invoke the command and input port handlers that were implemented earlier in the tutorial.

```c++
  void MathReceiverComponentImpl ::
    SchedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      QueuedComponentBase::MsgDispatchStatus stat = QueuedComponentBase::MSG_DISPATCH_OK;
      // empty message queue
      while (stat != MSG_DISPATCH_EMPTY) {
          stat = this->doDispatch();
      }
  }

```

##### 2.4.2.1.4 Parameter Updates

The developer can optionally receive a notification that a parameter has been updated by overriding a virtual function in the code generated base class:

```c++
  void MathReceiverComponentImpl ::
     parameterUpdated(
      FwPrmIdType id /*!< The parameter ID*/
  ) {
      if (id == PARAMID_FACTOR2) {
        Fw::ParamValid valid;
        F32 val = this->paramGet_factor2(valid);
        this->log_ACTIVITY_HI_MR_UPDATED_FACTOR2(val);
      }
  }
```

Add the function to the header file:

```c++
 // stored factor1
  F32 m_factor1;
  // number of times factor1 has been written
  U32 m_factor1s;

  void parameterUpdated(
      FwPrmIdType id /*!< The parameter ID*/
  );
```

Once it is added, add the directory to the build and build the component by typing `fprime-util build` from the `Ref` directory.

#### 2.4.2.2 Unit Tests

See section `2.4.1.3.1` for directions on how to generate unit test stubs and copy them to the correct subdirectory.
The `MathReceiver` tests are similar to `MathSender`.

##### 2.4.2.2.1 Test Code Implementation

The full unit test code for the `MathReceiver` component can be found in the `docs/Tutorials/MathComponent/MathReceiver/test/ut` directory. Many of the patterns are the same. Following are some highlights:

##### 2.4.2.2.2 Parameter Initialization

`Tester.cpp`, line 60:

```c++
  void Tester ::
    testAddCommand(void)
  {
      // load parameters
      this->component.loadParameters();
      ...
```

The `loadParameters()` call will attempt to load any parameters that the component needs.
The `this->paramSet_*` functions in the `*TesterBase` base classes allow the developer to set parameter and status values prior to the `loadParameters()`
With no manually set parameter values preceding the call, in this test case the parameter value is set to the default value.
It is a way to test default settings for parameters.

`Tester.cpp`, line 206:

```c++
  void Tester ::
    testSubCommand(void)
  {
      // set the test value for the parameter before loading - it will be initialized to this value
      this->paramSet_factor2(5.0,Fw::PARAM_VALID);

      // load parameters
      this->component.loadParameters();

```

In this test case, the parameter value was set prior to the `loadParameters()` call. A `Fw::PARAM_VALID` status is also set, which allows the component consider the value valid and use it.

##### 2.4.2.2.3 Serializable Usage

`Tester.cpp`, line 78:

```c++
      ...
      // verify the result of the operation was returned
      F32 result = (2.0-3.0)*2.0/5.0;
      // the event and telemetry channel use the Ref::MathOp type for values
      Ref::MathOp checkOp(2.0,3.0,Ref::SUB,result);
      ...
```

The `Ref::Mathop` class is the C++ implementation of the serializable type defined in `2.2.1`. When checking event and telemetry histories against the expected values, simply instantiate the serializable class in the test code and use it for comparisons.

##### 2.4.2.2.4 Event Throttling

`Tester.cpp`, line 395:

```c++
  void Tester ::
    testThrottle(void)
  {
```

This unit test demonstrates how event throttling works. The event is repeatedly issued until it reaches the throttle count and then is suppressed from then on. The throttle is reset by the `MR_CLEAR_EVENT_THROTTLE` command:

`Tester.cpp`, line 446:

```c++
      // send the command to clear the throttle
      this->sendCmd_MR_CLEAR_EVENT_THROTTLE(0,10);
```

The header file should be updated to include the `testThrottle` method as a public member.

`Tester.hpp`, line 51:

```c++
void testThrottle(void);
```


# 3 Topology

Now that the two components are defined, implemented and unit tested they can to be added to the `Ref` topology.
The topology describes the interconnection of all the components so the system operates as intended.
They consist of the core Command and Data Handling (C&DH) components that are part of the reusable set of components that come with the F´ repository as well as custom components written for the `Ref` reference example including the ones in this tutorial.
The `Ref` topology has already been developed as an example.
The tutorial will add the `MathSender` and `MathReceiver` components to the existing demonstration.
It involves modification of a topology description XML file as well as accompanying C++ code to instantiate and initialize the components.

## 3.1 Define C++ Component Instances

The first step is to include the implementation files in the topology source code.

### 3.1.1 Components.hpp

There is a C++ header file that declares all the component instances as externals for use by the initialization code and the generated code that interconnects the components. The two new components can be added to this file. First, include the header files for the implementation classes:

`Ref/Top/Components.hpp`, line 30:

```c++
#include <Drv/BlockDriver/BlockDriverImpl.hpp>

#include <Ref/MathSender/MathSenderComponentImpl.hpp>
#include <Ref/MathReceiver/MathReceiverComponentImpl.hpp>
```

`extern` declarations need to be made in this header file for use by the topology connection file that is discussed later as well as initialization code.

`Ref/Top/Components.hpp`, line 61:

```c++
extern Ref::PingReceiverComponentImpl pingRcvr;

extern Ref::MathSenderComponentImpl mathSender;
extern Ref::MathReceiverComponentImpl mathReceiver;
```

### 3.1.2 Topology.cpp

This C++ file is where the instances of the all the components are declared and initialized. The generated topology connection function is called from this file.

#### 3.1.2.1 Component Instantiation

Put these declarations after the declarations for the other `Ref` components:

`Ref/Top/Topology.cpp`, line 187:

```c++
Ref::MathSenderComponentImpl mathSender(FW_OPTIONAL_NAME("mathSender"));
Ref::MathReceiverComponentImpl mathReceiver(FW_OPTIONAL_NAME("mathReceiver"));
```

Where the other components are initialized, add `MathSender` and `MathReceiver`:

`Ref/Top/Topology.cpp`, line 286:

```c++
	pingRcvr.init(10);

    mathSender.init(10,0);
    mathReceiver.init(10,0);
```

The first argument is the queue message depth.
This is the number of messages that can be pending while other messages are being dispatched.

After all the components are initialized, the generated function `constructRefArchitecture()` (see `RefTopologyAppAc.cpp`) can be called to connect the components together. How this function is generated will be seen later in the tutorial.

`Ref/Top/Topology.cpp`, line 291:

```c++
    // call generated function to connect components
    constructRefArchitecture();

```

Next, the components commands are registered.

`Ref/Top/Topology.cpp`, line 308:

```c++
    health.regCommands();
    pingRcvr.regCommands();

    mathSender.regCommands();
    mathReceiver.regCommands();
```

Component parameters are retrieved from disk by `prmDb` prior to the components requesting them:

`Ref/Top/Topology.cpp`, line 314:

```c++
    // read parameters
    prmDb.readParamFile();
```

Once the parameters are read by `prmDb`, the components can request them:

`Ref/Top/Topology.cpp`, line 300:

```c++
    sendBuffComp.loadParameters();

    mathReceiver.loadParameters();
```

The thread for the active `MathSender` component needs to be started:

`Ref/Top/Topology.cpp`, line 357:

```c++
    pingRcvr.start(0, 100, 10*1024);

    mathSender.start(0,100,10*1024);
```

The arguments to the `start()` function is as follows:

|Argument|Usage|
|---|---|
|1|Thread ID, unique value for each thread. Not used for Linux|
|2|Thread priority. Passed to underlying OS|
|3|Thread stack size. Passed to underlying OS|


The `MathReceiver` queued component will execute on the thread of the 1Hz rate group, which will be shown later.
It does not need to to have a thread started, since queued components do not have threads.

The `exitTasks()` function is called when the process is shut down.
It contains `exit()` calls to all the active components.
These functions internally send a message to the component's thread to shut down.

`Ref/Top/Topology.cpp`, line 396:

```c++
    cmdSeq.exit();

    mathSender.exit();
```
## 3.2 Define Component Connections

Components need to be connected to invoke each other via ports.
The connections are specified via a topology XML file.
The file for the Ref example is located in `Ref/Top/RefTopologyAppAi.xml`
The connections for the new components will be added to the existing connections.

### 3.2.1 Component Imports

The component XML definitions must be imported into the topology file:

`Ref/Top/RefTopologyAppAi.xml`, line 32:

```xml
	<import_component_type>Svc/PassiveConsoleTextLogger/PassiveTextLoggerComponentAi.xml</import_component_type>


    <import_component_type>Ref/MathSender/MathSenderComponentAi.xml</import_component_type>
    <import_component_type>Ref/MathReceiver/MathReceiverComponentAi.xml</import_component_type>
```

### 3.2.2 Component Instances

The Component instances must be declared.

`Ref/Top/RefTopologyAppAi.xml`, line 92:

```xml
   <instance namespace="Svc" name="textLogger" type="PassiveTextLogger" base_id="521"  base_id_window="20" />

   <instance namespace="Ref" name="mathSender" type="MathSender" base_id="1000"  base_id_window="20" />
   <instance namespace="Ref" name="mathReceiver" type="MathReceiver" base_id="1100"  base_id_window="20" />
```

The name in the `name=` attribute must match the one declared previously in `Ref/Top/Components.hpp`. For example:

```c++
extern Ref::MathSenderComponentImpl mathSender;
```

The type must match the type declared in the component XML:

`Ref/MathSender/MathSenderComponentAi.xml`:

```xml
<component name="MathSender" kind="active" namespace="Ref">
```

The `base_id` attribute specifies the beginning range of the assigned IDs for commands, telemetry, events, and parameters.
The values declared in the component XML are added to this base address.
This allows multiple instances of components to be declared with unique ID ranges.
The `base_id_window` attribute is used to set a limit on ID ranges for spacing the base IDs from different components sufficiently apart.
If the IDs exceed the limit, the code generator will issue a warning.

### 3.2.3 Command connections

The command connections should follow these rules:

1. The port number of the command registration port on the `cmdDisp` component connection from the commanded components must be unique for all components.
2. The port number of the command dispatch port connection from the `cmdDisp` component to the commanded component must match the registration port number.
3. The command status from the components can go to port 0 of the command status port of the `cmdDisp` component.

The following XML shows the command connection for the tutorial components.

The port number used for the registration and dispatch ports is selected as 20,
a unique number that hasn't been used yet in the `Ref` example.

`Ref/Top/RefTopologyAppAi.xml`, line 817:

```xml
   <!-- Command Registration Ports - Registration port number must match dispatch port for each component -->

   <connection name = "MathSenderReg">
        <source component = "mathSender" port = "CmdReg" type = "CmdReg" num = "0"/>
        <target component = "cmdDisp" port = "compCmdReg" type = "CmdReg" num = "20"/>
   </connection>
   <connection name = "MathReceiverReg">
        <source component = "mathReceiver" port = "CmdReg" type = "CmdReg" num = "0"/>
        <target component = "cmdDisp" port = "compCmdReg" type = "CmdReg" num = "21"/>
   </connection>

    <!-- Command Dispatch Ports - Dispatch port number must match registration port for each component -->

   <connection name = "MathSenderDisp">
        <source component = "cmdDisp" port = "compCmdSend" type = "Cmd" num = "20"/>
        <target component = "mathSender" port = "CmdDisp" type = "Cmd" num = "0"/>
   </connection>
   <connection name = "MathReceiverDisp">
        <source component = "cmdDisp" port = "compCmdSend" type = "Cmd" num = "21"/>
        <target component = "mathReceiver" port = "CmdDisp" type = "Cmd" num = "0"/>
   </connection>

    <!-- Command Reply Ports - Go to the same response port on the dispatcher -->

   <connection name = "MathSenderReply">
      <source component = "mathSender" port = "CmdStatus" type = "CmdResponse" num = "0"/>
      <target component = "cmdDisp" port = "compCmdStat" type = "CmdResponse" num = "0"/>
   </connection>
   <connection name = "MathReceiverReply">
      <source component = "mathReceiver" port = "CmdStatus" type = "CmdResponse" num = "0"/>
      <target component = "cmdDisp" port = "compCmdStat" type = "CmdResponse" num = "0"/>
   </connection>
```

### 3.2.4 Event Connections

The output connections for log ports are connected to the `eventLogger` component.

`Ref/Top/RefTopologyAppAi.xml`, line 845:

```xml
   <!-- Event Logger Binary Connections -->

   <connection name = "MathSenderLog">
       <source component = "mathSender" port = "Log" type = "Log" num = "0"/>
        <target component = "eventLogger" port = "LogRecv" type = "Log" num = "0"/>
   </connection>
   <connection name = "MathReceiverLog">
       <source component = "mathReceiver" port = "Log" type = "Log" num = "0"/>
        <target component = "eventLogger" port = "LogRecv" type = "Log" num = "0"/>
   </connection>

   <!-- Event Logger Text Connections -->

   <connection name = "MathSenderTextLog">
       <source component = "mathSender" port = "LogText" type = "LogText" num = "0"/>
        <target component = "textLogger" port = "TextLogger" type = "LogText" num = "0"/>
   </connection>
   <connection name = "MathReceiverTextLog">
       <source component = "mathReceiver" port = "LogText" type = "LogText" num = "0"/>
        <target component = "textLogger" port = "TextLogger" type = "LogText" num = "0"/>
   </connection>
```

There are two kinds of connections for logging: One for a binary form that will be sent to the ground system, and a text version for displaying on standard output of the target machine.

### 3.2.5 Telemetry Connections

The telemetry output ports are connected to the `chanTlm` component.

`Ref/Top/RefTopologyAppAi.xml`, line 872:

```xml
   <!-- Telemetry Connections -->

   <connection name = "MathSenderTextTlm">
       <source component = "mathSender" port = "Tlm" type = "Tlm" num = "0"/>
        <target component = "chanTlm" port = "TlmRecv" type = "Tlm" num = "0"/>
   </connection>
   <connection name = "MathReceiverTextTlm">
       <source component = "mathReceiver" port = "Tlm" type = "Tlm" num = "0"/>
        <target component = "chanTlm" port = "TlmRecv" type = "Tlm" num = "0"/>
   </connection>

```

### 3.2.6 Parameter Connections

There are two parameter connections, a `PrmGet` connection for reading parameters during software initialization and a `PrmSet` for updating parameters in the component that manages parameter values. F' has a basic parameter storage component `prmDb` that stores parameters in files. Upon bootup, they are read from a file specified in the constructor and stored in memory. Subsequent to this, components request their parameters via the `PrmGet` connection. If they are updated by command, they can be saved to storage by issuing a command to call the `PrmSet` with the new value and issuing the `PRM_SAVE_FILE` command.

`Ref/Top/RefTopologyAppAi.xml`, line 883:

```xml
   <!-- Parameter Connections -->

   <connection name = "MathReceiverPrmGet">
       <source component = "mathReceiver" port = "ParamGet" type = "PrmGet" num = "0"/>
        <target component = "prmDb" port = "getPrm" type = "PrmGet" num = "0"/>
   </connection>
   <connection name = "MathReceiverPrmSet">
       <source component = "mathReceiver" port = "ParamSet" type = "PrmSet" num = "0"/>
        <target component = "prmDb" port = "setPrm" type = "PrmSet" num = "0"/>
   </connection>
```

### 3.2.7 Time Connections

Components that have telemetry or events need to be able to time stamp the events. The time connections connect the components to a time source to provide the time stamps.

`Ref/Top/RefTopologyAppAi.xml`, line 894:

```xml
   <!-- Time Connections -->

   <connection name = "MathSenderTime">
       <source component = "mathSender" port = "Time" type = "Time" num = "0"/>
        <target component = "linuxTime" port = "timeGetPort" type = "Time" num = "0"/>
   </connection>
   <connection name = "MathReceiverTime">
       <source component = "mathReceiver" port = "Time" type = "Time" num = "0"/>
        <target component = "linuxTime" port = "timeGetPort" type = "Time" num = "0"/>
   </connection>
```

### 3.2.8 Scheduler Connection

The `MathReceiver` component does not have a thread of its own, but relies on the thread of another component to drive it via the `SchedIn` port. The `SchedIn` port is connected to the 1Hz rate group component that is part of the `Ref` example. This means that every second the component gets a call and can unload messages from its message queue and dispatch them to handlers.

`Ref/Top/RefTopologyAppAi.xml`, line 894:

```xml
   <!-- Scheduler Connection -->
    <connection name = "MathReceiverRG">
         <source component = "rateGroup1Comp" port = "RateGroupMemberOut" type = "Sched" num = "4"/>
         <target component = "mathReceiver" port = "SchedIn" type = "Sched" num = "0"/>
    </connection>
```

### 3.2.9 The Math Operation Connection

The final connection is the connection that performs the math operation. It goes from `MathSender` to `MathReceiver`.

`Ref/Top/RefTopologyAppAi.xml`, line 911:

```xml

   <!-- Math Connection -->
   <connection name = "MathOpConnection">
       <source component = "mathSender" port = "mathOut" type = "Ref::MathOp" num = "0"/>
        <target component = "mathReceiver" port = "mathIn" type = "Ref::MathOp" num = "0"/>
   </connection>
   <connection name = "MathResultConnection">
       <source component = "mathReceiver" port = "mathOut" type = "Ref::MathResult" num = "0"/>
        <target component = "mathSender" port = "mathIn" type = "Ref::MathResult" num = "0"/>
   </connection>

```

Once all the updates to the topology file have been made, the module can be built by typing `fprime-util build` at the command line in the `Ref/` directory.
If the updates were correct, the module should compile with no errors.
The overall `Ref` deployment can be built by changing to the `Ref` directory and typing `fprime-util build`.

If running on a different platform, you can specify the build target by typing `fprime-util generate <target>`.

## 4.1 Running the Ground System

Once the `Ref` example has built successfully, you can run the ground system and executable by entering `fprime-gds -r fprime/Ref/build-artifacts`. The ground system GUI should appear.

### 4.1.1 Executing Commands

Commands can be executed by selecting the `Commands` tab and clicking on the `Cmds` drop-down list.

For the tutorial example, select the `MathSender` command `MS_DO_MATH` and fill in the arguments.

Clicking on the `Send` button will send the command to the software. When the command is sent, it is placed in the command history. It can be selected and sent again if the user desires.

### 4.1.2 Checking Events

The `Events` tab shows events that are generated by the software. For the tutorial, the events tab shows the events that were sent by the `MS_DO_MATH` command:

It shows the F' `CmdDispatcher` event indicating a command was dispatched and completed. It also has the events defined by the tutorial example that are sent as a results of requesting a math operation. The result is zero, since the `factor1` value is zero, as shown in the unit testing in section `2.4.2.2`.

The events are also echoed to `stdout` of the application, which can be found in the `Logs` tab, selecting "Ref.log" in the
dropdown.

### 4.1.3 Checking Telemetry

The `Channel Telemetry` tab shows channelized telemetry sent by the software. The channels defined by the tutorial have the last values and time they were updated:

### 4.1.5 Updating `factor1`

In order to get a non-zero result, `factor1` needs to be updated. The tutorial defined a command to update it, `MR_SET_FACTOR1`. It can be selected from the command tab:

When the command is executed, the `Log Events` tab will show the event indicating the value was updated.

The `Channel Telemetry` tab shows the two channels related to the update. `MR_FACTOR1` shows the new value, while `MR_FACTOR1S` show how many times the value has been updated.

### 4.1.6 Running the Command Again

After `factor1` has been updated, the command can be repeated:

### 4.1.7 Updated Events and Telemetry

The new events will appear in the `Log Events` tab:

Notice that the updated events are added to the end of the log, since events are meant to be a record of events in the software.

The `Channel Telemetry` tab will also show the updated values:

Notice that the `MS_OP`, `MS_VAL1`, `MS_VAL2`, `MR_OPERATION`, and `MS_RESULT` are updated to the latest value with a more recent time stamp, since telemetry channels are meant to show the latest value. The new result is `10.0` now that `factor1` has been updated.

### 4.1.8 Parameter Updates

The tutorial defined a `factor2` parameter in the `MathReceiver` component. The code generator creates two commands for each parameter: `XXXX_PRM_SET` and `XXX_PRM_SAVE` where `XXX` is an upper case version of the parameter name. The `FACTOR2_PRM_SET` command will set the value in `MathReceiver`, while `FACTOR2_PRM_SAVE` will send the current value to `PrmDb` for storage. `PrmDb` is an F' infrastructure component that reads and writes parameters to storage. It is important to note that `PrmDb` does not immediately write the value to storage. There is an explicit `PRM_SAVE_FILE` command that will take all the parameter values currently in RAM and write them.

#### 4.1.8.1 Setting the Parameter Value

The `FACTOR1_PRM_SET` command can be sent to the software:

The notification function that was implemented as part of the tutorial will send an event indicating the value was updated:

The `MS_DO_MATH` command can now be executed with the new value:

The `MathReceiver` component sends the events with the new result:

The new result is `1.0` with the new value of `factor2`. The "Channel Telemetry" tab also shows the new values:

#### 4.1.8.2 Saving the Parameter Value

Once the parameter value has been tested to the user's satisfaction, it can be saved to `PrmDb` by sending the `FACTOR2_PRM_SAVE` command:

The `Log Events` tab has an event from `PrmDb` indicating that the `FACTOR2` parameter value was added:

#### 4.1.8.3 Writing the Parameter to Storage

The parameter can be written to storage by sending the `PRM_SAVE_FILE` command:

`PrmDb` sends an event indicating that the parameters in RAM were stored:

### 4.1.9 Ground System Logs

The ground system keeps logs of all received events and telemetry. They can be found in the directories `<deployment>/logs/`, where `<Run Directory>` is the location of the deployment. e.g. `Ref`.

# Conclusion

This tutorial is an attempt to communicate the concepts and implementation. If there are aspects that are confusing,
feel free to submit GitHub issues asking for clarification or to report errors:

https://github.com/nasa/fprime/issues
