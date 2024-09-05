# F´ LED Blinker Tutorial for Arduinos
Reference: [Arduino Cross-Compilation (LedBlinker) Tutorial Github Repository](https://github.com/fprime-community/fprime-arduino-led-blinker)

This project is an implementation of the [F´ LED Blinker ARM Linux Tutorial](https://github.com/fprime-community/fprime-workshop-led-blinker.git) which will allow you to test on Arduino-based microcontrollers using the [`fprime-arduino` toolchain](https://github.com/fprime-community/fprime-arduino.git) instead of `fprime-arm-linux`. 

This version uses a scaled down deployment of F´ to account for the limited resources available on baremetal hardware.

## Prerequisites

In order to run through this tutorial, users should first do the following:

1. Follow the [Hello World Tutorial](../hello-world.md)
2. Ensure F´ tools have been [bootstrapped](../hello-world.md#bootstrapping-f)
3. Acquire and set up the appropriate [hardware](../led-blinker/hardware.md) for this tutorial
4. Follow the [arduino-cli installation guide](arduino-cli-install.md)

## Tutorial Steps

This tutorial is composed of the following steps:

1. [Project Setup](#1-led-blinker-project-setup)
2. [Building for Arduino](#2-building-for-arduino-microcontrollers)
3. [Component Design and Initial Implementation](#3-led-blinker-component-design-and-initial-implementation)
4. [Initial Component Integration](#4-led-blinker-initial-component-integration)
5. [Continuing Component Implementation](#5-led-blinker-continuing-component-implementation)
6. [Full System Integration](#6-led-blinker-full-system-integration)
7. [Running on Hardware](#7-led-blinker-running-on-hardware)

## 1. LED Blinker: Project Setup

First, make sure to read the tutorial's [introduction](index.md) and follow the [prerequisites section](#prerequisites). Then set up a new F´ project using the `fprime-bootstrap project` command. Please select a project name of `led-blinker`.

```shell
fprime-bootstrap project
Project name (MyProject): led-blinker
```

!!! note
    Complete instructions on setting-up a new project are available in the [hello world tutorial](../hello-world.md#1-creating-an-f-project).

!!! note
    Remember to source the virtual environment after creating the project.

## 2. Building for Arduino Microcontrollers

This section will guide you on converting your project to build for Arduino microcontrollers using the `fprime-arduino` package.

### Adding fprime-arduino

First, add the `fprime-arduino` package as a submodule into your project root.

```shell
# In led-blinker
git submodule add https://github.com/fprime-community/fprime-arduino.git
```

Add `fprime-arduino` as a library and change the default build toolchain to `teensy41` in `led-blinker/settings.ini` after the framework path.
```
library_locations: ./fprime-arduino
default_toolchain: teensy41
```
!!! note
    If you would like to use a different board as your default toolchain, you may change `teensy41` to your desired board.

### Modifying F´ Configuration Files (Optional)

If you are building for a system with low memory (< 100KB RAM), this step is recommended.

Copy the default F´ configuration files into your project root.
```shell
cp -r fprime/config .
```

Reference the [`config`](../config/) files in this repository and modify your `config` files accordingly. Or, you may clone this repository and copy the entire `config` directory into your project.

Then redirect the configuration path in your `led-blinker/settings.ini` by adding the following line:
```
config_directory: ./config
```

### Add Arduino Deployment

In order to produce an executable to run the software, users need to create a deployment. A deployment is one software executable that contains the main entry point, and an F´ system topology. We will be using a custom deployment that autogenerates an Arduino deployment.

First, add the line into `led-blinker/settings.ini`:
```
deployment_cookiecutter: https://github.com/fprime-community/fprime-arduino-deployment-cookiecutter.git
```

Create a new deployment in the `led-blinker` directory with:

```shell
#In led-blinker
fprime-util new --deployment
```

This will ask for some input, respond with the following answers:
```shell
[INFO] Cookiecutter source: https://github.com/fprime-community/fprime-arduino-deployment-cookiecutter.git
  [1/1] deployment_name (fprime-arduino-deployment): LedBlinker
[INFO] Found CMake file at 'led-blinker/project.cmake'
Add LedBlinker to led-blinker/project.cmake at end of file? (yes/no) [yes]: yes
```
!!! note
    Use the default response for any other questions asked.

In order to check that the deployment was created successfully, the user can generate a build cache and build the deployment. This will generate and build the code for the current host system, not the remote embedded hardware allowing a local test during development. 

### Test Deployment

Test the new teensy41 build by running:

```shell
fprime-util generate
fprime-util build
```

If you wish to test other Arduino builds, append the build name after `fprime-util generate` and `fprime-util build`. Tested boards are listed below (build name in parenthesis):
- Teensy 3.2 (teensy32)
- Teensy 4.0 (teensy40)
- Teensy 4.1 (teensy41)
- Adafruit Feather M0 (featherM0)
- Adafruit Feather RP2040 (featherrp2040rfm)
- SparkFun Thing Plus RP2040 (thingplusrp2040)
- ESP32 Dev Module (esp32)
- Raspberry Pi Pico W (rpipicow)

### Conclusion

Congratulations! You are now able to build an F´ deployment for Arduino microcontrollers! The next step is to create your LED Component.

!!! note
    You can no longer build for the native system as this is now an Arduino specific deployment.

## 3. LED Blinker: Component Design and Initial Implementation

The purpose of this exercise is to walk you through the creation and initial implementation of an F´ component to control the blinking of an LED. This section will discuss the design of the full component, the implementation of a command to start/stop the LED blinking, and the sending of events.  Users will then proceed to the initial ground testing before finishing the implementation in a later section.

### Component Design

In order for our component to blink an LED, it needs to accept a command to turn on the LED and drive a GPIO pin via a port call to the GPIO driver. It will also need a [rate group](../../user-manual/design/rate-group.md) input port to control the timing of the blink. Additionally, we will define events and telemetry channels to report component state, and a parameter to control the period of the blink.

This component design is captured in the block diagram below with input ports on the left and output ports on the right. Ports for standard F´ functions (e.g. commands, events, telemetry, and parameters) are circled in green.

![Led Component Block Diagram](../../../img/component-design.png)

In this exercise, the `BLINKING_ON_OFF` command shall toggle the blinking state of the LED. The period of the blinking is controlled by the `BLINK_INTERVAL` parameter. Blinking is implemented on the `run` rate group input port. The component also defines several telemetry channels and events describing the various actions taken by the component.

#### Design Summary

**Component Ports:**
1. `run`: invoked at a set rate from the rate group, used to control the LED blinking
2. `gpioSet`: invoked by the `Led` component to control the GPIO driver

> Standard component ports (circled in green) are not listed here.

**Commands:**
1. `BLINKING_ON_OFF`: turn the LED blinking on/off

**Events:**
1. `InvalidBlinkArgument`: emitted when an invalid argument was supplied to the `BLINKING_ON_OFF` command
2. `SetBlinkingState`: emitted when the component sets the blink state
3. `BlinkIntervalSet`: emitted when the component blink interval parameter is set
4. `LedState`: emitted when the LED is driven to a new state

**Telemetry Channels:**
1. `BlinkingState`: state of the LED blinking
2. `LedTransitions`: count of the LED transitions

**Parameters:**
1. `BLINK_INTERVAL`: LED blink period in number of rate group calls

### Create the component

It is time to create the basic component. In a terminal, navigate to the project's root directory and run the following:

```bash
# In led-blinker
mkdir -p Components
cd Components

fprime-util generate
fprime-util new --component
```
You will be prompted for information regarding your component. Fill out the prompts as shown below:

```bash
[INFO] Cookiecutter source: using builtin
  [1/8] Component name (MyComponent): Led
  [2/8] Component short description (Component for F Prime FSW framework.): Component to blink an LED driven by a rate group
  [3/8] Component namespace (Components): Components
  [4/8] Select component kind
    1 - active
    2 - passive
    3 - queued
    Choose from [1/2/3] (1): 2
  [5/8] Enable Commands?
    1 - yes
    2 - no
    Choose from [1/2] (1): 1
  [6/8] Enable Telemetry?
    1 - yes
    2 - no
    Choose from [1/2] (1): 1
  [7/8] Enable Events?
    1 - yes
    2 - no
    Choose from [1/2] (1): 1
  [8/8] Enable Parameters?
    1 - yes
    2 - no
    Choose from [1/2] (1): 1
[INFO] Found CMake file at 'led-blinker/project.cmake'
Add Components/Led to led-blinker/project.cmake at end of file? (yes/no) [yes]: yes
Generate implementation files? (yes/no) [yes]: yes
```
Your new component is located in the directory `led-blinker/Components/Led`.

### Component State

Many of the behaviors of the component discussed in the [Component Design](#component-design) section require the tracking of some state. Before diving into the implementation of the behavior let us set up and initialize that state.

Open `Led.hpp` in `led-blinker/Components/Led`, and add the following private member variables to the end of the file.

```cpp
  PRIVATE:
  
    Fw::On state; //! Keeps track if LED is on or off
    U64 transitions; //! The number of on/off transitions that have occurred from FSW boot up
    U32 count; //! Keeps track of how many ticks the LED has been on for
    bool blinking; //! Flag: if true then LED blinking will occur else no blinking will happen
```

Open `Led.cpp` in `led-blinker/Components/Led`, and initialize your member variables in the constructor:

```cpp
Led ::Led(const char* const compName) : LedComponentBase(compName),
    state(Fw::On::OFF),
    transitions(0),
    count(0),
    blinking(true)
{}
```

Now that the member variables are set up, we can continue into the component implementation.

!!! note
    The above code will fail to find the `Fw::On` enum type until we use it in the FPP model in the next section. To fix immediately, add `#include <Fw/Types/OnEnumAc.hpp>` to the top of `Led.hpp`.

#### Commands

Commands are used to command the component from the ground system or a command sequencer. We will add a command named `BLINKING_ON_OFF` to turn on or off the blinking LED. This command will take in an argument named `on_off` of type `Fw.On`.


Inside your `led-blinker/Components/Led` directory, open the file `Led.fpp` and search for the following:

```
        # @ Example async command
        # async command COMMAND_NAME(param_name: U32)
```

Replace that block with the following:

```
        @ Command to turn on or off the blinking LED
        sync command BLINKING_ON_OFF(
            on_off: Fw.On @< Indicates whether the blinking should be on or off
        )
```

Exit the text editor, and run the following in the `led-blinker/Components/Led` directory:

```bash
# In led-blinker/Components/Led
fprime-util impl
```

This command will auto generate two files: Led.hpp-template and Led.cpp-template. These files contain the stub implementation for the component. These should now include stubs for this newly added command.

Inside your `led-blinker/Components/Led` directory, open `Led.hpp-template` and copy the following block of code. Paste it as a private function above the private member variabls in `Led.hpp`.

```cpp
      //! Implementation for BLINKING_ON_OFF command handler
      //! Command to turn on or off the blinking LED
      void BLINKING_ON_OFF_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          Fw::On on_off /*!< 
          Indicates whether the blinking should be on or off
          */
      );
```

Inside your `led-blinker/Components/Led` directory, open `Led.cpp-template` and copy the following block of code and paste it into `Led.cpp` below the destructor.

```cpp
  void Led ::
    BLINKING_ON_OFF_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        Fw::On on_off
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::CmdResponse::OK);
  }
```

!!! note
    This pattern of copying implementations from *-template files into our cpp and hpp files will be repeated throughout the rest of this tutorial.


Now we will implement the behavior of the `BLINKING_ON_OFF` command. An initial implementation is shown below and may be copied into `Led.cpp` in-place of the stub we just copied in.

```cpp
  void Led ::
    BLINKING_ON_OFF_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        Fw::On on_off
    )
  {
    // Create a variable to represent the command response
    auto cmdResp = Fw::CmdResponse::OK;

    // Verify if on_off is a valid argument.
    // Note: isValid is an autogenerate helper function for enums defined in fpp.
    if(!on_off.isValid())
    {
        // TODO: Add an event that indicates we received an invalid argument.
        // NOTE: Add this event after going through the "Events" exercise.

        // Update command response with a validation error
        cmdResp = Fw::CmdResponse::VALIDATION_ERROR;
    }
    else
    {
      this->count = 0; // Reset count on any successful command
      this->blinking = Fw::On::ON == on_off; // Update blinking state

      // TODO: Add an event that reports the state we set to blinking.
      // NOTE: This event will be added during the "Events" exercise.

      // TODO: Report the blinking state via a telemetry channel.
      // NOTE: This telemetry channel will be added during the "Telemetry" exercise.
    }

    // Provide command response
    this->cmdResponse_out(opCode,cmdSeq,cmdResp);
  }
```

Save the file then run the following command in the terminal to verify your component is building correctly.

```bash
# In led-blinker/Components/Led
fprime-util build
```

!!! warning
    Fix any errors that occur before proceeding with the rest of the tutorial.

### Events

Events represent a log of system activities. Events are typically emitted any time the system takes an action. Events are also emitted to report off-nominal conditions. Our component has four events, two that this section will show and two are left to the student.

Back inside your `led-blinker/Components/Led` directory, open the `Led.fpp` file. After the command you added in the previous section, add two events:

```
        @ Indicates we received an invalid argument.
        event InvalidBlinkArgument(badArgument: Fw.On) \
            severity warning low \
            format "Invalid Blinking Argument: {}"

        @ Reports the state we set to blinking.
        event SetBlinkingState(state: Fw.On) \
            severity activity high \
            format "Set blinking state to {}."
```

Save the file and in the terminal, run the following to verify your component is building correctly.

```bash
# In led-blinker/Components/Led
fprime-util build
```
!!! warning
    Resolve any errors before continuing.

Now open `Led.cpp` in your `led-blinker/Components/Led` directory and navigate to the `BLINKING_ON_OFF` command. Report via our new event when there is an error in the input argument. 

To do so, replace: 
```cpp
        // TODO: Add an event that indicates we received an invalid argument.
        // NOTE: Add this event after going through the "Events" exercise.
```

with:
```cpp
        this->log_WARNING_LO_InvalidBlinkArgument(on_off);
```

Similarly, use an event to report the blinking state has been set.

Replace the following:
```cpp
      // TODO: Add an event that reports the state we set to blinking.
      // NOTE: This event will be added during the "Events" exercise.
```

with:
```cpp
      this->log_ACTIVITY_HI_SetBlinkingState(on_off);
```

Save the file and in the terminal, run the following to verify your component is building correctly.

```bash
fprime-util build
```

Finally, add the `BlinkIntervalSet` event with an argument of `U32` type to indicate when the interval parameter is set and the `LedState` event with an argument of `Fw.On` type to indicate that the LED has been driven to a different state. You will emit these event in later sections. Build the component again to prove the FPP file is correct.

!!! warning
    Resolve any `fprime-util build` errors before continuing 

### Conclusion

Congratulations!  You have now implemented some basic functionality in a new F´ component. Before finishing the implementation, let's take a break and try running the above command through the ground system. This will require integrating the component into the system topology.

!!! note
    When running in the ground system try running `led.BLINKING_ON_OFF` with a value of `ON` and ensure that the event `SetBlinkingState` is emitted indicating the blinking switched to on!


## 4. LED Blinker: Initial Component Integration

In this section, perform the initial integration the LED component into their deployment. This deployment will automatically include the basic command and data handling setup needed to interact with the component. Wiring the `Led` component to the GPIO driver component will be covered in a later section after the component implementation has finished.

!!! note
    Users must have created the [initial Led component implementation](#3-led-blinker-component-design-and-initial-implementation) in order to run through this section. Users may continue to define commands, events, telemetry, and ports after this initial integration.

### Adding `Led` Component To The Deployment

The component can now be added to the deployment's topology effectively adding this component to the running system. This is done by modifying `instances.fpp` and `topology.fpp` in the `Top` directory.

Add the following to `led-blinker/LedBlinker/Top/instances.fpp`.  Typically, this is added to the "Passive component instances" section of that document.

```
  instance led: Components.Led base id 0x10000
```

This defines an instance of the `Led` component called `led`.

Next, the topology needs to use the above definition. This is done by adding the following to the list of instances defined in `led-blinker/LedBlinker/Top/topology.fpp`.

```
  topology LedBlinker {
    ...
    instance ...
    instance led
    ...
  }
```

!!! note
    No port connections need to be added because thus far the component only defines standard ports and those are connected automatically.
    
    This includes the large project (e.g. `Components`) in this deployment's build.

### Conclusion

Congratulations! You have now integrated your component and tested that integration.

This tutorial will return to the component implementation before finishing the integration of the component and testing on hardware.

## 5. LED Blinker: Continuing Component Implementation

In this section, we will complete the component implementation by transmitting a telemetry channel, and implementing the behavior of the `run` port, which is called by the rate-group.

!!! note
    Refer back to the [component design](#component-design) for explanations of what each of these items is intended to do.

### Telemetry Channels

Telemetry channels represent the state of the system. Typically, telemetry channels are defined for any states that give crucial insight into the component's behavior. This tutorial defines two channels: one will be shown, and the other is left up to the student.

Inside your `led-blinker/Components/Led` directory, open the `Led.fpp` file. After the events you added in the previous implementation section, add a telemetry channel of type `Fw.On` to report the blinking state.

```
        @ Telemetry channel to report blinking state.
        telemetry BlinkingState: Fw.On
```

Save the file. In the terminal, run the following to verify your component is building correctly.

```bash
# In led-blinker/Components/Led
fprime-util build
```

!!! warning
    Fix any errors that occur before proceeding with the rest of the tutorial.

Inside your `led-blinker/Components/Led` directory, open `Led.cpp`, and navigate to the `BLINKING_ON_OFF` command. Report the blinking state via the telemetry channel we just added. To do so, replace the following:

```cpp
        // TODO: Report the blinking state via a telemetry channel.
        // NOTE: This telemetry channel will be added during the "Telemetry" exercise.
```

with the command to send the telemetry channel:

```cpp
        this->tlmWrite_BlinkingState(on_off);
```

Save the file. In the terminal, run the following to verify your component is building correctly.

```bash
# In led-blinker/Components/Led
fprime-util build
```
!!! warning
    Fix any errors that occur before proceeding with the rest of the tutorial.


Now complete the component's telemetry model by adding the `LedTransitions` channel of type `U32` to `Led.fpp`. You will emit this telemetry channel in a further section.

!!! note
    Build with that new channel and fix any errors before continuing.

#### Adding `Led` Channels To the Packet Specification

Some users choose to send telemetry packets instead of raw channels to the ground system. Although this tutorial will not use telemetry packets, it is best practice to keep the packet definitions up-to-date to make switching to telemetry packets seamless should the user choose to do so.

Add the following to `led-blinker/LedBlinker/Top/LedBlinkerPackets.xml`:

```xml
    <packet name="LedChannels" id="8" level="1">
        <channel name="led.LedTransitions"/>
        <channel name="led.BlinkingState"/>
    </packet>
```
!!! note
    Add this after the opening `<packet>` tag and before the closing `</packet>` tag as a separate block.

Now that this has been added, build the topology:

```shell
# In led-blinker/LedBlinker
fprime-util build
```
!!! warning
    Fix any errors before continuing

### Parameters

Parameters are ground-controllable settings for the system. Parameters are used to set settings of the system that the ground may need to change at some point during the lifetime of the system. This tutorial sets one parameter, the blink interval.

For each parameter you define in your fpp, the F´ autocoder will autogenerate a SET and SAVE command. The SET command allows ground to update the parameter. The SAVE command allows ground to save the current value of the parameter for use even after FSW reboots.

In your `led-blinker/Components/Led` directory, open the `Led.fpp` file. After the telemetry channels you added previously, add a parameter for the blinking interval. Give the parameter the name `BLINK_INTERVAL` and type `U32`.

```
        @ Blinking interval in rate group ticks
        param BLINK_INTERVAL: U32
```

Save the file. In the terminal, run the following to verify your component is building correctly.

```bash
# In led-blinker/Components/Led
fprime-util build
```

In your `led-blinker/Components/Led` directory, open the file `Led.hpp` and add the following public function signature after the destructor:

```cpp
    //! Emit parameter updated EVR
    //!
    void parameterUpdated(FwPrmIdType id /*!< The parameter ID*/
    );
```

!!! note
    This function is called when a parameter is updated via the generated SET command. Although the value is updated automatically, this function gives developers a chance to respond to changing parameters. This tutorial uses it to emit an updated Event.

Save file and in your `led-blinker/Components/Led` directory, open `Led.cpp` and add the implementation for `parameterUpdated`:

```cpp
void Led ::parameterUpdated(FwPrmIdType id) {
    // Read back the parameter value
    Fw::ParamValid isValid;
    U32 interval = this->paramGet_BLINK_INTERVAL(isValid);
    // NOTE: isValid is always VALID in parameterUpdated as it was just properly set
    FW_ASSERT(isValid == Fw::ParamValid::VALID, isValid);

    // Check the parameter ID is expected
    if (PARAMID_BLINK_INTERVAL == id) {
        // Emit the blink interval set event
        // TODO: Add an event with, severity activity high, named BlinkIntervalSet that takes in an argument of type U32 to report the blink interval.
    }
}
```

When you are done, save the file. In the terminal, run the following to verify your component is building correctly.

```bash
# In led-blinker/Components/Led
fprime-util build
```
!!! warning
    Resolve any errors before continuing

### Additional Ports

Any communication between components should be accomplished through F´ ports. Thus far we have been using a set of standard ports for handling Commands, Telemetry, Events, and Parameters. This section will add two specific ports to our component: input `run` to be called from the rate group, and output `gpioSet` to drive the GPIO driver.

In your `led-blinker/Components/Led` directory, open the `Led.fpp` file. After the parameters you added previously, add the following two ports:

```
        @ Port receiving calls from the rate group
        sync input port run: Svc.Sched

        @ Port sending calls to the GPIO driver
        output port gpioSet: Drv.GpioWrite
```

!!! note
    Input ports can be given any name that you choose. In this example, we choose `run` and `gpioSet` since these names capture the behavioral intent. The types of `Svc.Sched` and `Drv.GpioWrite` are significant as these types must match the remote component.

In your `led-blinker/Components/Led` directory, run the following to autogenerate stub functions for the `run` input port we just added.

```bash
# In led-blinker/Components/Led
fprime-util impl
```

In your `led-blinker/Components/Led` directory, open `Led.hpp-template` file and copy this block over to `Led.hpp` as we did before

```cpp
    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for run
      //!
      void run_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< 
      The call order
      */
      );
```

In your `led-blinker/Components/Led` directory, open `Led.cpp-template` file and copy this block over to `Led.cpp`.
```cpp
  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void Led ::
    run_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }
```

The `run` port will be invoked repeatedly on each cycle of the rate group. Each invocation will call into the `run_handler` function such that the component may perform behavior on each cycle.

Here we want to turn the LED on or OFF based on a cycle count to implement the "blinking" behavior we desire.


In your `led-blinker/Components/Led` directory, open `Led.cpp`, copy in the following block of code, and try filling-in the TODOs based on what you learned and defined in previous sections.

!!! warning
    Don't forget to read the code and comments to understand more about how to use F´.

```cpp
    void Led ::
        run_handler(
            const NATIVE_INT_TYPE portNum,
            NATIVE_UINT_TYPE context)
    {
        // Read back the parameter value
        Fw::ParamValid isValid;
        U32 interval = 0; // TODO: Get BLINK_INTERVAL parameter value

        // Force interval to be 0 when invalid or not set
        interval = ((Fw::ParamValid::INVALID == isValid) || (Fw::ParamValid::UNINIT == isValid)) ? 0 : interval;

        // Only perform actions when set to blinking
        bool is_blinking = this->blinking;
        if (is_blinking)
        {
            Fw::On new_state = this->state;
            // Check for transitions
            if ((0 == this->count) && (this->state == Fw::On::OFF))
            {
                new_state = Fw::On::ON;
            }
            else if (((interval / 2) == this->count) && (this->state == Fw::On::ON))
            {
                new_state = Fw::On::OFF;
            }

            // A transition has occurred
            if (this->state != new_state)
            {
                this->transitions = this->transitions + 1;
                // TODO: Add a telemetry to report the number of LED transitions (this->transitions)

                // Port may not be connected, so check before sending output
                if (this->isConnected_gpioSet_OutputPort(0))
                {
                    this->gpioSet_out(0, (Fw::On::ON == new_state) ? Fw::Logic::HIGH : Fw::Logic::LOW);
                }

                // TODO: Add an event to report the LED state (new_state).
                this->state = new_state;
            }

            this->count = ((this->count + 1) >= interval) ? 0 : (this->count + 1);
        }
    }
```

Save the file and in the terminal, run the following to verify your component is building correctly.

```bash
# In led-blinker/Components/Led
fprime-util build
```
!!! warning
    Resolve any errors and finish any TODOs before continuing.

### Conclusion

Congratulations! You just completed the implementation of your component. It is time to finish implementation and run on hardware!

## 6. LED Blinker: Full System Integration

Now it is time to add a GPIO driver to our system and attach it to the `led` component instance. We'll also connect the `led` component instance to a 1 Hz rate group. Finally, we'll configure the driver to manage the GPIO 13 pin on our hardware. Once this section is finished, the system is ready for running on hardware.

### Adding the GPIO Driver to the Topology

`fprime-arduino` provides a GPIO driver for Arduino microcontrollers called `Arduino.GpioDriver`. This should be added to both the instance definition list and the topology instance list just like we did for the `led` component. Since the GPIO driver is a passive component, its definition is a bit more simple.

Add to "Passive Component" section of `led-blinker/LedBLinker/Top/instance.fpp`:
```
  instance gpioDriver: Arduino.GpioDriver base id 0x4C00
```

Add to the instance list of `led-blinker/LedBLinker/Top/topology.fpp`:
```
    instance gpioDriver
```

!!! note
    In `led-blinker` build the deployment and resolve any errors before continuing.

### Wiring The `led` Component Instance to the `gpioComponent` Component Instance and Rate Group

The `Led` component defines the `gpioSet` output port and the `Arduino.GpioDriver` defines the `gpioWrite` input port. These two ports need to be connected from output to input. The `ActiveRateGroup` component defines an array of ports called `RateGroupMemberOut` and one of these needs to be connected to `run` port defined on the `Led` component.

We can create a named connections block in the topology and connect the two port pairs. Remember to use the component instances and not the component definitions for each connection.

To do this, add the following lines to `led-blinker/LedBLinker/Top/topology.fpp`:
```
    # Named connection group
    connections LedConnections {
      # Rate Group 1 (1Hz cycle) ouput is connected to led's run input
      rateGroup1.RateGroupMemberOut[3] -> led.run
      # led's gpioSet output is connected to gpioDriver's gpioWrite input
      led.gpioSet -> gpioDriver.gpioWrite
    }
```

!!! note
    `rateGroup1` is preconfigured to call all `RateGroupMemberOut` at a rate of 1 Hz. We use index `RateGroupMemberOut[3]` because `RateGroupMemberOut[0]` through `RateGroupMemberOut[2]` were used previously in the `RateGroups` connection block.

### Configuring The GPIO Driver

So far the GPIO driver has been instantiated and wired, but has not been told what GPIO pin to control. For this tutorial, the built-in LED will be used. To configure this, the `open` function needs to be called in the topology's C++ implementation and passed the pin's number and direction.

This is done by adding the following line to the `configureTopology` function defined in `led-blinker/LedBLinker/Top/LedBLinkerTopology.cpp`.

```
void configureTopology() {
    ...
    gpioDriver.open(Arduino::DEF_LED_BUILTIN, Arduino::GpioDriver::GpioDirection::OUT);
    ...
}
```

This code tells the GPIO driver to open pin LED_BUILTIN (usually pin 13) as an output pin. If your device does not have a built in LED, select a GPIO pin of your choice.

!!! note
    In `led-blinker` build the deployment and resolve any errors before continuing.

### Conclusion

Congratulations!  You've wired your component to the rate group driver and GPIO driver components. It is time to try it on hardware.

## 7. LED Blinker: Running on Hardware

Now it is time to run on hardware. Connect the microcontroller to the host machine via USB.

First, upload the binary/hex file to the board after building. Follow [this guide](uploading-to-board.md) if the files are not automatically uploading.

Next run the F´ GDS without launching the native compilation (`-n`) and with the dictionary from the build above (`--dictionary ./build-artifacts/<build name>/LedBlinkerTopologyAppDictionary.xml`). Connect it to the USB device by adding the `--comm-adapter`, `--uart-device`, and `--uart-baud` flags

```bash
# In the project root
fprime-gds -n --dictionary ./build-artifacts/teensy41/LedBlinker/dict/LedBlinkerTopologyAppDictionary.xml --comm-adapter uart --uart-device /dev/ttyACM0 --uart-baud 115200
```

For MacOS users, you may have to install `pyserial`: `pip install pyserial`

!!! note
    Change `teensy41` to your build if it is not for the Teensy 4.1 (i.e. `teensy32`, `featherM0`, `esp32`, etc.)

!!! note
    `/dev/ttyACM0` may vary for your system/device. It may also be `/dev/ttyUSB0`. For MacOS, it will be along the lines of `/dev/tty.usbmodem12345`. Change accordingly.
    
    To view the list of your connected devices, run: `ls /dev/tty*`.

!!! note
    The green circle should now appear on the top right of F´ GDS.

### Testing the Topology

Test the component integration with the following steps:

1. Verify connection: confirm that there is a green circle and not a red X in the upper right corner.
2. Send a Command: select the 'Commanding' tab, search for led.BLINKING_ON_OFF and send it with the argument set to ON.
3. Verify Event: select the 'Events' tab and verify that the SetBlinkingState event reports the blinking state was set to ON.
4. Repeat steps 2 and 3 to turn the LED OFF.
5. Verify Telemetry: select the 'Channels' tab and verify that the LedBlinker telemetries appear.


### Conclusion

Congratulations you've now run on hardware.

[Return to Tutorials](index.md){ .md-button .md-button--primary }