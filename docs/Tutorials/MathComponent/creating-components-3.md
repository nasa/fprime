# Creating Components Part 3: Starting the MathReceiver

## In this Section 

In this section you will begin creating `MathReceiver` following the same steps as the last section. Note, that the `MathReceiver` a little different than the `MathSender`.

**Component Description** The `MathReceiver` is a queued component which receives parameters, send parameters, logs events, and sends telemetry. With this is mind, use the following command to create the `MathReceiver` component. 

```shell
# In: Components
fprime-util new --component 
```
This command will prompt you for some inputs. You should specify the follow so that the components matches the short description above. Don't forget that this is a QUEUED component:

```
[INFO] Cookiecutter source: using builtin
component_name [MyComponent]: MathReceiver 
component_short_description [Example Component for F Prime FSW framework.]: Your description
Component_namespace[Component]: MathModule
Select component_kind:
1 - active
2 - passive
3 - queued
Choose from 1, 2, 3 [1]: 3 
Select enable_commands:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
Select enable_telemetry:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
Select enable_events:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
Select enable_parameters:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
[INFO] Found CMake file at 'MathProject/project.cmake'
Add component Components/MathSender to MathProject/project.cmake at end of file (yes/no)? yes
Generate implementation files (yes/no)? yes
```

Before doing anything to the files you have just generated, try building. 

```shell 
# In: MathReceiver
fprime-util build
```

Now that you have created the component, you can implement the component behavior in the fpp model. Use a text editor to replace the existing MathReceiver.fpp with the following: 

```fpp
module MathModule {

  @ Component for receiving and performing a math operation
  queued component MathReceiver {

    # ----------------------------------------------------------------------
    # General ports
    # ----------------------------------------------------------------------

    @ Port for receiving the math operation
    async input port mathOpIn: MathOp

    @ Port for returning the math result
    output port mathResultOut: MathResult

    @ The rate group scheduler input
    sync input port schedIn: Svc.Sched

    # ----------------------------------------------------------------------
    # Special ports
    # ----------------------------------------------------------------------

    @ Command receive
    command recv port cmdIn

    @ Command registration
    command reg port cmdRegOut

    @ Command response
    command resp port cmdResponseOut

    @ Event
    event port eventOut

    @ Parameter get
    param get port prmGetOut

    @ Parameter set
    param set port prmSetOut

    @ Telemetry
    telemetry port tlmOut

    @ Text event
    text event port textEventOut

    @ Time get
    time get port timeGetOut

    # ----------------------------------------------------------------------
    # Parameters
    # ----------------------------------------------------------------------

    @ The multiplier in the math operation
    param FACTOR: F32 default 1.0 id 0 \
      set opcode 10 \
      save opcode 11

    # ----------------------------------------------------------------------
    # Events
    # ----------------------------------------------------------------------

    @ Factor updated
    event FACTOR_UPDATED(
                          val: F32 @< The factor value
                        ) \
      severity activity high \
      id 0 \
      format "Factor updated to {f}" \
      throttle 3

    @ Math operation performed
    event OPERATION_PERFORMED(
                               val: MathOp @< The operation
                             ) \
      severity activity high \
      id 1 \
      format "{} operation performed"

    @ Event throttle cleared
    event THROTTLE_CLEARED \
      severity activity high \
      id 2 \
      format "Event throttle cleared"

    # ----------------------------------------------------------------------
    # Commands
    # ----------------------------------------------------------------------

    @ Clear the event throttle
    async command CLEAR_EVENT_THROTTLE \
      opcode 0

    # ----------------------------------------------------------------------
    # Telemetry
    # ----------------------------------------------------------------------

    @ The operation
    telemetry OPERATION: MathOp id 0

    @ Multiplication factor
    telemetry FACTOR: F32 id 1

  }

}
```

Implement the behavior you just wrote in the fpp model using:

```shell
# In: MathReceiver 
fprime-util impl
```

Replace the orginal cpp and hpp files with the ones you just created:

```shell 
# In: MathReceiver
mv MathReceiver.cpp-template MathReceiver.cpp
mv MathReceiver.hpp-template MathReceiver.hpp
```

Test the build:

```shell 
# In: MathReceiver
fprime-util build 
```

## Summary 

You just created a queued component stub, filled in the fpp file, and wrote specified component charactaristics in the fpp file. 