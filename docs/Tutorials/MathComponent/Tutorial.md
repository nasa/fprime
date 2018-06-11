# Introduction

The following example shows the steps to implement a simple pair of components connected by a pair of ports. The first, `MathSender`, will invoke the second, `MathReceiver`, to perform a math operation and return the result. Here is a description of the components:

## MathSender
`MathSender` must do the following:

### Commands

`MathSender` should implement a `MS_DO_MATH` command. This command will have three arguments:
1) A first value in the operation
2) A second value in the operation
3) An enumerated argument specifying the operation to perform

### Events
`MathSender` should emit an event telling that a command was received to perform the operation. It should also emit an event when the result is received back from `MathReceiver`.

### Telemetry Channels
MathSender should have four channels:
1) The first value
2) The second value
3) The operation
4) The result

### Parameters
MathSender will have no parameters.

`MathSender` should be an active (i.e. threaded) component, so it will proces the commands immediately. The command will be *asynchronous*, which means the handler will be executed on the thread of the active component. It will delegate the operation to `MathReceiver`.

## MathReceiver 

`MathReceiver` will be a queued component that performs the requested operation and returns the result. `MathReceiver` will be connected to the 1Hz rate group that is part of the reference example. The simple operation in this component could have just as easily been done in a passive or active component; it is done here as a queued component to illustrate how to implement one.

### Commands

`MathReceiver` should implement a MR_SET_FACTOR1 command. This command will set a factor used for any subsequent operations. The result of the commanded operation will be multipled by this factor. It should default to 1 if the command is never invoked.

`MathReceiver` should also implement a MR_CLEAR_EVENT_THROTTLE command to clear the throttled MR_SET_FACTOR1 event (see below).

### Events

`MathReceiver` should have the following events:

1) MR_SET_FACTOR1 command event. When the command is received, `MathReceiver` should emit an event with the updated factor. The event should be throttled (i.e. stop emitting) after three invocations. Normally, throttling is used to prevent event floods if there a endlessly repeating condition.
2) MR_UPDATED_FACTOR2 event. When the factor2 parameter (see below) is updated, `MathReceiver` should emit an event with the updated value.
3) MR_OPERATION_PERFORMED event. When the component receives a request to perform the operation, it should emit an EVR with the arguments and operation.
4) MR_THROTTLE_CLEARED in response to the MR_CLEAR_EVENT_THROTTLE command above.

### Channels

`MathReceiver` should have the following channels:

1) A channel that has a serializable structure argument that contains the two terms in the operation as well as the operation and the result. This will be used to illustrate an XML defined serializable as a single telemetry channel.
2) A channel that counts the number of MR_SET_FACTOR1 commands received, so that a count can be known past the throttled event.
3) A channel for each of the factors used in the operation.

### Parameters

`MathReceiver` will have one parameter, a second factor used in the operation.

### Operation

`MathReceiver` will perform the following operation when requested by `MathSender`:

result = (value1 operation value2)*factor1/factor2

# Implementation

The implementation of the component will will have the following steps:

1) Define the `MathPort` port that is used between the components.
2) Define the `MathSender` component in XML and compile it.
3) Implement the `MathSender` derived implementation class. 
4) Unit test the `MathSender` implementation component.
5) Define the `MathReceiver` component in XML.
6) Implement the `MathReceiver` implemenation class.
7) Unit test the `MathReceiver` implemenation class.
8) Connect the classes to the `Ref` topology.
9) Run the ground system and exercise the commands and view the telemetry and events in the GUI.

## Port definition

There are two ports to define in order to perform the operation between the components. The XML for the ports will be first shown in their entirety, and then the individual parts will be described.

### MathInPort

`MathOpPort` is responsible for passing the invocation of the operation from `MathSender` to `MathReceiver`. The new XML file should be placed in a new directory `Ref/MathPorts` with the name `MathOpPortAi.mxl`. The XML for the port is as follows:

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
                <item name="MATH_MULTIPY"/>
                <item name="MATH_DIVIDE"/>
            </enum>
            <comment>operation argument</comment>
        </arg>
    </args>
</interface>
```

#### Port Name Specification

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
|namespace|The namespace of the component. The C++ namespace the component class will appear in.  

#### Port Argument Specification

The port arguments are what are passed from component to component when they are connected. The port argument XML is as follows:

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
                <item name="MATH_MULTIPY"/>
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
                <item name="MATH_MULTIPY"/>
                <item name="MATH_DIVIDE"/>
            </enum>
 ```
#### Adding the port to the build

The build system needs to be made aware of the port XML. To do this, edit the file `/mk/configs/modules/modules.mk`.  

Find the `REF_MODULES` variable and add the new port directory:

```make
REF_MODULES := \
    Ref/Top \
    Ref/RecvBuffApp \
    Ref/SendBuffApp \
    Ref/SignalGen \
    Ref/PingReceiver \
    Ref/MathPorts
```

Create a file named `mod.mk` in the `MathPorts` directory. This file tells the build system that a new file needs to be added to the build. Here are the contents:

```make
SRC = 	MathOpPortAi.xml 
```

A second make file is needed to build the code in the local `MathPorts` directory. It should be named `Makefile` and have the following contents:

```make
# derive module name from directory

MODULE_DIR = Ref/MathPorts
MODULE = $(subst /,,$(MODULE_DIR))

BUILD_ROOT ?= $(subst /$(MODULE_DIR),,$(CURDIR))
export BUILD_ROOT

include $(BUILD_ROOT)/mk/makefiles/module_targets.mk

```

The `MODULE_DIR` path should always be the current directory of the `Makefile`.

Now a make target can be called to include the module in the build. This only needs to be done once:

```shell
make gen_make
Generating templates
...
Regenerating global Makefile
Generating Makefiles in .../fprime/mk/makefiles
Makefile generation complete.
Build Time: 0:00.51
```

Now, the port code can be generated and compiled:

```shell
make
Building module RefMathPorts code generation (level 4)
...
make[1]: Leaving directory '.../fprime/Ref/MathPorts'
Build Time: 0:03.49
```

The code generation from the XML produces two files:

```
 MathOpPortAc.cpp
 MathOpPortAc.hpp 
```
These contain the C++ classes that implement the port functionality. The build system will automatically compile them when it is aware of the port XML file.

### MathResultPort

`MathOpPort` is responsible for passing the invocation of the operation from `MathSender` to `MathReceiver`. The new XML file should be placed in the `Ref/MathPorts` directory with the name `MathResultPortAi.xml`. The XML for the port is as follows:

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

This file can be added to the `mod.mk` in the `Ref/MathPorts` directory:

```make
SRC = 	MathOpPortAi.xml \
		MathResultPortAi.xml
```

Running `make gen_make` and 'make' as before will make the build system aware of the new port XML file and compile it.

The code generated to implement ports is complete. Developers do not need to add any implmentation code of their own.

## Serializable Definition

A structure needs to be defined that represents the channel value needed by `MathReceiver`. All port calls, telemetry channels, events and parameters need to be comprised of `Serializable` values, or values that can be turned into a byte stream. This is needed to pass port arguments through message queues and to pass commands and telemetry to and from the ground system. Built-in basic types like integers, floating point numbers and boolean values are supported by the framework, but there are times when a developer wishes to use a custom-defined type, perhaps to keep members of a object consistent with each other. These structures can be defined in XML and the code generator will generate the C++ classes with all the neccessary serialization functions. Developers can hand-code their own, but they are not usable for telemetry since the ground sysmtem needs an XML definition to decode them.

### MathOp

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

#### Serializable Name Specification

The opening tag of the XML specifies the type name and namespace of the structure:

```xml
<serializable namespace="Ref" name="MathOp">
...
</serializable>
```

#### Serializable Members

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

As before with the port definitions, the `Ref/MathTypes` directory needs to be added to `/mk/configs/modules/modules.mk`.  

```make
REF_MODULES := \
    Ref/Top \
    Ref/RecvBuffApp \
    Ref/SendBuffApp \
    Ref/SignalGen \
    Ref/PingReceiver \
    Ref/MathPorts \
    Ref/MathTypes
```

## Component Definition

### MathSender Component

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
        <event id="0" name="MS_COMMAND_RECV" severity="ACTIVITY_LO" format_string = "Math Cmd Recvd: %f %d %f"  >
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
        <event id="1" name="MD_RESULT" severity="ACTIVITY_HI" format_string = "Math result is %f" >
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

#### Component Name Specification

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

#### Port importing

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

#### Port Declarations

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
|data_type|The type of the port as defined in the included port definitions, in the form `namepace::name`|
|kind|The kind of port. Can be `sync_input`,`async_input`,`output`|

For `MathSender`, the request for the operation will be sent on the `mathOut` output port, and the result will be returned on the `mathIn` asynchronous port. Because the component is active and the result input port is asynchronous, the port handler will execute on the thread of `MathSender`.

#### Command Declarations

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
|kind|The kind of command. Can be `sync_input`,`async_input`,`output`|

#### Telemetry

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

#### Events

The XML for the defined events is as follows:

```xml
    <events>
        <event id="0" name="MS_COMMAND_RECV" severity="ACTIVITY_LO" format_string = "Math Cmd Recvd: %f %d %f"  >
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
        <event id="1" name="MD_RESULT" severity="ACTIVITY_HI" format_string = "Math result is %f" >
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
|id|A numeric value for the event. The value is relative to a base value set when the component is added to a topology|
|format_string|A C-style format string for displaying the event and the argument values.|

The directory containing the component XML can be added to the list of modules in `/mk/configs/modules/modules.mk`:

```make
REF_MODULES := \
	Ref/Top \
	Ref/RecvBuffApp \
	Ref/SendBuffApp \
	Ref/SignalGen \
	Ref/PingReceiver \
	Ref/MathPorts \
	Ref/MathTypes \
	Ref/MathSender
```

Create a `mod.mk` file in `Ref/MathSender` and add `MathSender`. 

Once it is added, add the directory to the build and build the component by typing `make rebuild`.

## Component Implementation

The component implementation consists of writing a class that is derived from the code-generated base class and filling in member functions that implement the port calls. 

### Stub generation

There is a make target that will generate stubs that the developer can fill in. The command to generate the stubs is: `make impl`. This will generate two files:

```
MathSenderComponentImpl.hpp-template
MathSenderComponentImpl.cpp-template
```

Rename the files by removing the `-template` from the end of the file names.

```
MathSenderComponentImpl.hpp
MathSenderComponentImpl.cpp
```

Add the new files to the `mod.mk` file:

```make
SRC = MathSenderComponentAi.xml MathSenderComponentImpl.cpp

HDR = MathSenderComponentImpl.hpp
```

Make the build system aware of the new files and build:

```
make rebuild
```

The stub files should sucessfully compile.

### Handler implementation

The next step is to fill in the handler with implementation. 

First, find the empty command handler:

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
          opPort = MATH_MULTIPY;
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

The handler will send the appropriate events and telemetry values, then invoke the output math operation port to request the operation. Note that each channel and event argument that has an enumeration has a unique type declaration. Finally, note that the output command response port must be called with a command status in order to let the framework components know that the command is complete. If the completion status isn't sent, it will stall any sequences the command was part of. There are command error status along with successfull completions. Most commands return this status at the end of the handler, but component implementations can store the `opCode` and `cmdSeq` values to return later, but those specific values must be returned in order to match the status with the command originally sent.

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

```
  void MathSenderComponentImpl ::
    mathIn_handler(
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
      this->tlmWrite_MS_RES(result);
      this->log_ACTIVITY_HI_MD_RESULT(result);
  }

```

This handler reports the result via a telemetry channel and an event.

### MathReceiver Component

#### Component Specification

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

Many of the elements are the same as described in `MathSender`, so this section will highlight the differences.

#### Queued component

The `MathReceiver` component is queued, which means it can receive port invocations as messages, but needs an external thread to dequeue them.

#### Importing the serializable type

The telemetry channels and events use a serializable type, `Ref::MathOp` to illustrate the use of those types. The following line specifies the import for this type:

```
<import_serializable_type>Ref/MathTypes/MathOpSerializableAi.xml</import_serializable_type>
```

This type is then available for events and channels, but are not available for parameters and command arguments.

#### Scheduler port

The queued component has a scheduler port that is `sync_input`. That means the port invocation is not put on a message queue, but calls the handler on the thread of the caller of the port:

```
        <port name="SchedIn" data_type="Sched" kind="sync_input">
            <comment>
            The rate group scheduler input
            </comment>
        </port>

```

This synchronous call allows the caller to pull any pending messages of the message queue on the thread of the component invoking the `SchedIn` port.

#### Throttled Event

The `MR_SET_FACTOR1` event has a new argument `throttle = "3"` that specifies how many events will be emitted before the event is throttled so no more appear.

```
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

#### Parameter

The `MathReceiver` component has a declaration for a parameter:

```
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
|set_opcode|The opcode of the command to set the parameter. Must not overlap with any of the commands|
|save_opcode|The opcode of the command to save the parameter. Must not overlap with any of the commands|

#### Component Implementation

As before, a stub can be generated:

```
make impl
cp MathReceiverComponentImpl.cpp-template MathReceiverComponentImpl.cpp
cp MathReceiverComponentImpl.hpp-template MathReceiverComponentImpl.hpp
```

Add the stub files to `mod.mk`:

```make
SRC = MathReceiverComponentAi.xml MathReceiverComponentImpl.cpp

HDR = MathReceiverComponentImpl.hpp

```

Add the files and compile them: `make rebuild`

##### Port handler

Look for the empty port hander in the sub class:

```
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

```
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
          case MATH_MULTIPY:
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

In this handler, the operation is done based on the port arguments from `MathSender`. The `op` structure is populated for the event and telemetry calls, and the `mathOut` port is called to send the result back to `MathSender`. The parameter value is retrieved during initialization and is returned via the `paramGet_factor2()` call. The commands to set and save the commands run entirely in the code generated base classes.

##### Commands

The command handler to update the value of `factor1` is as follows:

```
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

The telemetry and log values are sent, and the command is responded to. Note that after three calls to the handler, the `this->log_ACTIVITY_HI_MR_SET_FACTOR1(val)` call will not actually send any events until the throttle is cleared.

The hander to clear the throttle is as follows:

```
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
##### Scheduler Call

The port invoked by the scheduler retrieves the messages from the message queue and dispatches them:

```
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

##### Parameter Updates

The developer can optionally receive a notification that a parameter has been updates by overriding a virtual function in the code generated base class:

```     
  void MathReceiverComponentImpl ::
     parameterUpdated(
      FwPrmIdType id /*!< The parameter ID*/
  ) {
      Fw::ParamValid valid;
      F32 val = this->paramGet_factor2(valid);
      this->log_ACTIVITY_HI_MR_UPDATED_FACTOR2(val);
  }
```

## Topology

Now that the two components are defined and implemented, they need to be added to the `Ref` topology.

### Define instances

The first step is to include the implementation files in the topology source code.

#### Components.hpp

There is a C++ header file that declares all the component instances as externals for use by the initialization code and the generated code that interconnects the components. The two new components can be added to this file:

`Ref/Top/Components.hpp`

```c++
#include <Ref/MathSender/MathSenderComponentImpl.hpp>
#include <Ref/MathReceiver/MathReceiverComponentImpl.hpp>
...
extern Ref::MathSenderComponentImpl mathSender;
extern Ref::MathReceiverComponentImpl mathReceiver;
```

The `Top` module can be built by typing `make` at the command line.

#### Topology.cpp

The initialization file must have instances of the all the components. The components are initialized and interconnected in this file.

Put these declarations at the end of the declarations for the other `Ref` components:

```c++
Ref::MathSenderComponentImpl mathSender
#if FW_OBJECT_NAMES == 1
    ("mathSender")
#endif
;

Ref::MathReceiverComponentImpl mathReceiver
#if FW_OBJECT_NAMES == 1
    ("mathReceiver")
#endif
;
```

Where the other components are initialzed, add `MathSender` and `MathReceiver`:

```
    mathSender.init(10,0);
    mathReceiver.init(10,0);
    // Connect rate groups to rate group driver
    constructRefArchitecture();
```

The `constructRefArchitecture()` call interconnects the components and it has be called after the components have been initialized.

Next, the components commands are registered:

```
    health.regCommands();
    pingRcvr.regCommands();

    mathSender.regCommands();
    mathReceiver.regCommands();
```

The parameters are retrieved:

```
...
    sendBuffComp.loadParameters();
    mathReceiver.loadParameters();

```

The threads for the active components need to be started (and later stopped):

```
    mathSender.start(0,100,10*1024);
```
In the `exitTasks()` called when the process is shut down:

```
    mathSender.exit();
```

#### RefTopologyAppAi.xml

The `RefTopologyAppAi.xml` XML file defines the connections between components. Parts of it are code generated using the MagicDraw tool, but for this exercise new entries will be added by hand at the end.

##### Component Imports

The component XML definitions are imported into the topology file:

```
    <import_component_type>Ref/MathSender/MathSenderComponentAi.xml</import_component_type>
    <import_component_type>Ref/MathReceiver/MathReceiverComponentAi.xml</import_component_type>

```



##### Command connections

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

The command registration and command dispatch have to follow the rule that the port number used to register the commands in `cmdDisp` is the port number that must be used to dispatch commands to the component.

##### Event Connections

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

There are two kinds of connections for logging: One for a binary form that will be sent to the ground system, and a text version for displaying on the text console.

##### Telemetry Connections

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
There is one port to sent binary telemetry updates to the ground software.

##### Parameter Connections

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

There are two connections when a component has parameters: One to get the current value during initialization and one to send an updated value back to the parameter database.

##### Time Connections

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

The components need a time connection to time tag events and channelized telemetry.

###### The Math Connection

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

There is a connection to perform the math operation.

###### Scheduler Connection

```xml
   <!-- Scheduler Connection -->
    <connection name = "MathReceiverRG">
         <source component = "rateGroup1Comp" port = "RateGroupMemberOut" type = "Sched" num = "3"/>
         <target component = "mathReceiver" port = "SchedIn" type = "Sched" num = "0"/>
    </connection>
```

The scheduler connection is connected to the 1Hz rate group to allow the queued component to pull any messages of its message queue.