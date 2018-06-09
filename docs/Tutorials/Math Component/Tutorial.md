# Introduction

The following example shows the steps to implement a simple pair of components connected by a pair of ports. The first, `MathSender`, will invoke the second, `MathReceiver`, to perform a math operation and return the result. Here is a description of the components:

## MathSender - `MathSender` must do the following:

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

`MathInPort` is responsible for passing the invocation of the operation from `MathSender` to `MathReceiver`. The new XML file should be placed in a new directory `Ref/MathPorts` with the name `MathInPortAi.mxl`. The XML for the port is as follows:

```xml
<interface name="MathIn" namespace="Ref">
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
<interface name="MathIn" namespace="Ref">
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
SRC = 	MathInPortAi.xml 
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
 MathInPortAc.cpp
 MathInPortAc.hpp 
```
These contain the C++ classes that implement the port functionality. The build system will automatically compile them when it is aware of the port XML file.

### MathOutPort

`MathInPort` is responsible for passing the invocation of the operation from `MathSender` to `MathReceiver`. The new XML file should be placed in the `Ref/MathPorts` directory with the name `MathInPortAi.xml`. The XML for the port is as follows:

```xml
<interface name="MathOut" namespace="Ref">
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
SRC = 	MathInPortAi.xml \
		MathOutPortAi.xml
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

The `MathSender` component XML definition is as follows:

```xml
<component name="MathSender" kind="active" namespace="Ref">
    <import_port_type>Ref/MathPorts/MathInPortAi.xml</import_port_type>
    <import_port_type>Ref/MathPorts/MathOutPortAi.xml</import_port_type>
    <comment>Component sending a math operation</comment>
    <ports>
        <port name="mathOut" data_type="Ref::MathIn" kind="output">
            <comment>
            Port for sending the math operation
            </comment>
        </port>
        <port name="mathIn" data_type="Ref::MathOut" kind="async_input">
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


