# How-To: Define State Machines in F Prime

This guide shows how to define and use state machines in F Prime using the F Prime Modeling Language (FPP). State machines help capture component behavior by modeling modes (states) and transitions explicitly, making complex logic easier to implement, test, and maintain. FPP provides autocoding capabilities to allow users to quickly implement state-defined behavior.

---

## Prerequisites

Before starting, you should have:

* Completed the [Hello World Tutorial](https://fprime.jpl.nasa.gov/latest/tutorials-hello-world/docs/hello-world/) (so you’ve built and run at least one component).
* A general understanding of [FPP component modeling](https://nasa.github.io/fpp/fpp-users-guide.html).
* Experience creating commands, events, and telemetry in FPP.
* A working build of F Prime on your system (`fprime-util` runs successfully).

---

## When to Use State Machines

State machines are useful when your component has distinct **modes** or **operational states** with different behavior in specific states and rules for the transitions between states. Examples include:

* A radio with `OFF`, `IDLE`, and `TRANSMITTING` states.
* A sensor with a series of start-up states.

By modeling these as a state machine, you make system behavior explicit, verifiable, and easier to test.

---

## Example State Machine

This guide will walk through the steps of implementing a state machine with the following properties:

0. The initial state is `RESET`. Followed by `WAIT_RESET`, `ENABLE`, `CONFIGURE` and `RUN`.
1. At each state a `tick` signal will trigger an action.
2. Actions may yield `success` or `error` signals.
3. `success` signals progress linearly through to the next state.
4. `error` signals return to state `RESET`
5. The state machine remains in a given state (and repeats the action) when the action yields no signals.

This state machine can be seen in the following diagram.

```mermaid
stateDiagram-v2
    [*] --> RESET

    %% State definitions with annotations
    state "RESET

    tick / doReset" as RESET

    state "WAIT_RESET

    tick / checkReset" as WAIT_RESET

    state "ENABLE

    tick / doEnable" as ENABLE

    state "CONFIGURE

    tick / doConfigure" as CONFIGURE

    state "RUN

    tick / doRead" as RUN

    RESET --> WAIT_RESET : success
    WAIT_RESET --> ENABLE : success
    ENABLE --> CONFIGURE : success
    CONFIGURE --> RUN : success

    ENABLE --> RESET : error
    WAIT_RESET --> RESET : error
    CONFIGURE --> RESET : error
    RUN --> RESET : error
```

This state machine is implemented in the [MpuImu](https://github.com/fprime-community/fprime-sensors/blob/devel/fprime-sensors/MpuImu/Components/ImuManager/ImuStateMachine.fpp) component. This component drives the `tick` signal from a rate group handler.

> [!NOTE]
> The [MpuImu](https://github.com/fprime-community/fprime-sensors/blob/devel/fprime-sensors/MpuImu/Components/ImuManager/ImuStateMachine.fpp) component implements a further `reconfigure` signal allowing the machine to return to reconfigure state.

## Designing a State Machine in FPP

We will model the Imu state machine in FPP.  In this guide, we will use a separate file `ImuStateMachine.fpp` defined in the same folder (i.e. module) as the `ImuManager`, but you may also inline the state machine directly in the component itself.  We define the basic module and state machine with:

```
module MpuImu {
    state machine ImuStateMachine
}
```

The module, filename, etc was chosen to remain consistent with our example code (and the `ImuManager` component that uses the state machine).


### Defining a State Machine and Initial State

The first step of defining our state machine, is to name the machine and the initial `RESET` state. This step will give us a basis for state machine implementation.

```
module MpuImu {
    @ Define ImuStateMachine State Machine
    state machine ImuStateMachine {
        @ Initial state: reset the device
        initial enter RESET

        @ Reset the Imu
        state RESET {
        }
    }
}
```

### Defining Signals and Adding More States

Next we should define our `success`, `error`, and `tick` signals and the remaining states: `WAIT_RESET`, `ENABLE`, `CONFIGURE`, and `RUN`.  Here we have added in all the structures (states, and signals) that define the general flow of the state machine. However, we have not added any transition logic yet.

The revised machine should look something like:

```
module MpuImu {
    @ Define ImuStateMachine State Machine
    state machine ImuStateMachine {
        @ Initial state: reset the device
        initial enter RESET

        @ Rate-group driven tick signal
        signal tick

        @ Current state passed successfully
        signal success

        @ Current state erred
        signal error

        @ Reset the Imu
        state RESET

        @ Wait for the Imu to reset
        state WAIT_RESET

        @ Enable Imu data flows
        state ENABLE

        @ Configure Imu
        state CONFIGURE

        @ Run the Imu
        state RUN
    }
}
```

### Defining Transitions

The next step is to add our transitions between states. Here we handle signals using the `on` syntax, and dictate the next state to enter. This provides the linear flow, and return to reset behaviors.

This update results in:

```
module MpuImu {
    @ Define ImuStateMachine State Machine
    state machine ImuStateMachine {
        @ Initial state: reset the device
        initial enter RESET

        @ Rate-group driven tick signal
        signal tick

        @ Current state passed successfully
        signal success

        @ Current state erred
        signal error

        @ Reset the Imu
        state RESET {
            on success enter WAIT_RESET
        }

        @ Wait for the Imu to reset
        state WAIT_RESET {
            on success enter ENABLE
            on error enter RESET
        }

        @ Enable Imu data flows
        state ENABLE {
            on success enter CONFIGURE
            on error enter RESET
        }

        @ Configure Imu
        state CONFIGURE {
            on success enter RUN
            on error enter RESET
        }

        @ Run the Imu
        state RUN {
            on error enter RESET
        }
    }
}
```

> [!NOTE]
> The `RESET` state defines no `error` transition because the machine should remain in `RESET` on error. Similarly, the `RUN` state defines no `success` transition as it should remain in `RUN` on success.

The state machine now has transitions between states, but lacks taking action at each state. This will be covered next.


### Defining Actions

Here is where we define actions for the state machine to take. Actions may occur as part of transitions, in response to signals, and on entry/exit from a state.  They call back into the components' C++ implementation. This allows user defined behavior (i.e. talking to the Imu over I2C).

In this state machine, we will use the `tick` signal to trigger action. `tick` will be invoked off our rate-group handler. By restricting actions to the `tick` signal, we ensure that only one state and the associated (singular) I2C communication is performed on each rate group invocation.  This was done to ensure that rate group calls are of a deterministic length and so that the I2C bus remains uncontested.

We use the `action` keyword to define actions: `doReset`, `checkReset`, `doEnable`, `doConfigure`, and `doRead`. We use `on <signal> do { <action> }` syntax to specify the actions to run on each `tick` signal.  This results in:

```
module MpuImu {
    @ Define ImuStateMachine State Machine
    state machine ImuStateMachine {
        @ Initial state: reset the device
        initial enter RESET

        @ Rate-group driven tick signal
        signal tick

        @ Current state passed successfully
        signal success

        @ Current state erred
        signal error


        @ Perform reset commands
        action doReset

        @ Check if reset completed
        action checkReset

        @ Perform enable commands
        action doEnable

        @ Perform configure commands
        action doConfigure

        @ Read the IMU
        action doRead

        @ Reset the Imu
        state RESET {
            on success enter WAIT_RESET
            on tick do { doReset }
        }

        @ Wait for the Imu to reset
        state WAIT_RESET {
            on success enter ENABLE
            on error enter RESET
            on tick do { checkReset }
        }

        @ Enable Imu data flows
        state ENABLE {
            on success enter CONFIGURE
            on error enter RESET
            on tick do { doEnable }
        }

        @ Configure Imu
        state CONFIGURE {
            on success enter RUN
            on error enter RESET
            on tick do { doConfigure }
        }

        @ Run the Imu
        state RUN {
            on error enter RESET
            on tick do { doRead }
        }
    }
}
```

That should complete the definition of our state machine. However, we still have yet to bind it to our `ImuManager` component.

## Instantiating the State Machine

Every state machine can be used multiple times. To define a single instance attached to the `ImuManager` component, we use the following in our component definition:

```
queued component ImuManager {
    @ Use the ImuStateMachine
    state machine instance imuStateMachine: ImuStateMachine
}
```


Here we chose a `queued` component which will require intentional dispatch of the component's queue. `active` components may also work and dispatch via a thread, however; we'd lose the ability to perform the component's work on the rate group invocation.

> [!WARNING]
> Only `queued` and `active` components may contain state machines. Users choosing a `queued` component must dispatch their state machines as they do with other component messaging (e.g. commands and port invocations). `active` components dispatch all messages via their internal thread.


## Integrating Into C++

Now we need to integrate the state machine into the C++ implementation of our component.  Like other constructs defined in FPP, we can get prototypes and templates by running:

```
fprime-util impl
```

This will generate the necessary action handlers for us to fill in.

### Implementing Actions in C++

Actions must be implemented for the state machine by the component. We are provided the following prototype  from `fprime-util impl` in the `.template.hpp` file. Prototypes belong in the component's HPP file.

**Function Prototype in HPP**
```c++
    //! Implementation for action doReset of state machine MpuImu_ImuStateMachine
    //!
    //! Perform reset commands
    void MpuImu_ImuStateMachine_action_doReset(SmId smId,                             //!< The state machine id
                                               MpuImu_ImuStateMachine::Signal signal  //!< The signal
                                               ) override;
```

Additionally, we can fill in the C++ implementation in the CPP file. Below we call a helper function "reset" and based on the return value choose to output an error.

**Function Implementation in CPP**
```c++
void ImuManager ::MpuImu_ImuStateMachine_action_doReset(SmId smId, MpuImu_ImuStateMachine::Signal signal) {
    Drv::I2cStatus status = this->reset();
    // Transition to RESET state on failure
    if (status != Drv::I2cStatus::I2C_OK) {
        this->log_WARNING_HI_I2cError(DEVICE_ADDRESS, status);
    } else {
        // TODO: success
    }
```

> [!WARNING]
> You must implement all action methods in the component. This guide shows just `doReset` for brevity and the full implementation is available in the [`ImuManager`](https://github.com/fprime-community/fprime-sensors/blob/devel/fprime-sensors/MpuImu/Components/ImuManager/ImuManager.cpp)

### Implementing Signaling in C++

The next step is to add signaling. In this case, we need to send `success` and `error` signals. We should also send `tick` in the rate group invocation (`run_handler`). Signaling is done by calling the `this-><state_machine_instance_name>_sendSignal_<signal_name>();` function. Below we've added an if-block to our `doReset` implementation checking for status and signaling appropriately.

```c++
void ImuManager ::MpuImu_ImuStateMachine_action_doReset(SmId smId, MpuImu_ImuStateMachine::Signal signal) {
    // This function is implemented only for the specific instance "imuStateMachine"
    FW_ASSERT(smId == SmId::imuStateMachine);
    Drv::I2cStatus status = this->reset();
    // Transition to RESET state on failure
    if (status != Drv::I2cStatus::I2C_OK) {
        this->log_WARNING_HI_I2cError(DEVICE_ADDRESS, status);
        this->imuStateMachine_sendSignal_error();
    } else {
        this->imuStateMachine_sendSignal_success();
    }
}
```

> [!TIP]
> The action function is generic to the state machine type (i.e. `ImuStateMachine`). Signals are sent via the state machine instance (i.e. `imuStateMachine`). Asserting the state machine id prevents against dispatching signals to the wrong instance. Users with multiple instances of a state machine in their single component may use a `switch`-`case` block to handle multiple different signal functions.
>
> ```c++
> switch (smId) {
>    case SmId::imuStateMachine1:
>        this->imStateMachine1_sendSignal_success();
>        break;
>    case SmId::imuStateMachine2:
>        this->imStateMachine2_sendSignal_success();
>        break;
> }
> ```


Sending the `tick` signal happens in the rate group invocation (`run_handler`) and uses the same structure.

```c++
void ImuManager ::run_handler(FwIndexType portNum, U32 context) {
    this->imuStateMachine_sendSignal_tick();
}
```

### Dispatching the State Machine

Finally, we need to dispatch the state machine messages because we chose a `queued` component. This is done in the `run_handler` using the `dispatchCurrentMessages()` helper.

```c++
void ImuManager ::run_handler(FwIndexType portNum, U32 context) {
    this->imuStateMachine_sendSignal_tick();
    this->dispatchCurrentMessages();
}
```

> [!WARNING]
> Only `queued` components should dispatch queued messages in this way. `active` components use their thread for dispatching. `this->dispatchCurrentMessages()` will dispatch all messages to the component (state machine signals, asynchronous commands, asynchronous port calls, etc).

That's all! The state machine should run at this point.

## Conclusion

State machines in FPP let you capture operational modes explicitly, enforce valid transitions, and ensure components behave predictably. They are especially useful for reducing the code written to handle state changes, reduce state variables, and model high-level behavior.  You can explore the full [`ImuStateMachine`](https://github.com/fprime-community/fprime-sensors/blob/devel/fprime-sensors/MpuImu/Components/ImuManager/ImuStateMachine.fpp) for an understanding of how to handle new transitions (like `reconfigure`) as well as see the fully integrated state machine.

---

## References

* [`ImuManager` Component](https://github.com/fprime-community/fprime-sensors/tree/devel/fprime-sensors/MpuImu/Components/ImuManager)
* [FPP User’s Guide on State Machines](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-State-Machines)
* [F Prime Hello World Tutorial](https://fprime.jpl.nasa.gov/latest/tutorials-hello-world/docs/hello-world/)
* [Example Components in fprime-examples](https://github.com/nasa/fprime-examples)


