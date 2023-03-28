# Getting Started: Creating an F´ Hello World Component

This tutorial will walk new users through creating a basic F´ component. Users should have completed the new project
tutorial and have the tools sourced as shown in the[conclusion](./NewProject.md#conclusion) portion of that tutorial.

F´ components encapsulate the various parts of system behavior. These components can interact with the ground system
through [commands](Tutorial.md#command), [events](./Tutorial.md#event), and
[telemetry channels](./Tutorial.md#telemetry-channel). Components communicate with other components through
[ports](./Tutorial.md#port), which covered in-depth in [another tutorial](../MathComponent/Tutorial.md).

### Prerequisites:
- [Getting Started: Creating an F´ Project](./NewProject.md)

### Tutorial Steps:
- [Hello World Component](#hello-world-component-requirements)
- [Creating the Hello World Component](#creating-the-hello-world-component)
- [Editing the Component Model](#editing-the-component-model)
- [Implementing Component Behavior](#implementing-component-behavior)
- [Conclusion](#conclusion)

## Hello World Component Requirements

The first step for creating a new component is understanding what it is that we wish to implement. This is called
defining requirements. In the spirit of "Hello World" this component will encapsulate greeting behavior. The component
will define three items to implement greeting behaviour:

1. A [command](./Tutorial.md#command) called `SAY_HELLO` that will command the component to send a greeting
2. An [event](./Tutorial.md#event) called `Hello` that is the greeting sent in response to the `SAY_HELLO` command
3. A [telemetry channel](./Tutorial.md#telemetry-channel) called `GreetingCount` that will count each `Hello` event sent

These are a simple set of requirements for this component.

## Creating the Hello World Component

The next step is to create the new component. This is accomplished with the following command:

```bash
fprime-util new --component
```
This command will ask for some input. You should respond with the following answers:
1. Component Name: HelloWorld
2. Component Namespace: MyComponents
3. Add Commands: yes
4. Add Events: yes
5. Add Telemetry: yes
6. Add to the build system: yes

> For any other questions, select the default response.

This will create a new component called "HelloWorld" in the "MyProject" namespace. This new component will be able to
define commands, events, and telemetry channels.

We should navigate to the component's directory and look around:

```bash
cd MyComponents/HelloWorld
ls
```
This will show the following files:
1. `HelloWorld.fpp`: design model for the component
2. `HelloWorld.hpp` and `HelloWorld.cpp`: C++ implementation files for the component
3. `CMakeList.txt`: build definitions for the component.

To build this component run `fprime-util build` in the current folder.

> Any component in F´ can be built by navigating to the component's folder and running `fprime-util build`.

## Editing the Component Model

A component model defines the interface of the component with the rest of the F´ system and with the ground system F´
communicates with. In this case we intend to define a command, an event, and a telemetry channel as specified above.

Open the model file `HelloWorld.fpp` and add the following:

```
@ Command to issue greeting with maximum length of 20 characters
async command SAY_HELLO(
    greeting: string size 20 @ Greeting to repeate in the Hello event
)

@ Greeting event with maximum greeting length of 20 characters
event Hello(
    greeting: string size 20 @ Greeting supplied from the SAY_HELLO command
) severity activity high format "I say: {}"

@ A count of the number of greetings issued
telemetry GreetingCount: U32
```
> You should ensure to replace any existing command, event, and channel definitions with those supplied above but leave
> the other definitions untouched.

With this step completed you can generate a basic implementation with the following command:

```bash
fprime-util impl
```

This creates `HelloWorld.hpp-template` and `HelloWorld.cpp-template` files that contain our new fill-in template. While
normally one would merge new templates with the existing code, we will instead overwrite the existing implementations as
we have not edited those files yet.  To do this:

```bash
mv HelloWorld.hpp-template HelloWorld.hpp
mv HelloWorld.cpp-template HelloWorld.cpp
```
We are now ready for implementing component behavior.

## Implementing Component Behavior

F´ behavior is implemented in two types of methods command handler functions to implement command behavior and handler
functions to implement port behavior (as described in the next tutorial). For this tutorial we need to implement the
`SAY_HELLO` command, so we need to edit the `SAY_HELLO_cmdHandler` function in the `HelloWorld.cpp` file.  Ensure its
contents look like:

```c++
void HelloWorld:: SAY_HELLO_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& greeting) {
    // Copy the command string input into an event string for the Hello event
    Fw::LogStringArg eventGreeting(greeting.toChar());
    // Emit the Hello event with the copied string
    this->log_ACTIVITY_HI_Hello(eventGreeting);
    
    this->tlmWrite_GreetingCount(++this->m_greetingCount);
    
    // Tell the fprime command system that we have completed the processing of the supplied command with OK status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}
```
> We must also add the m_greetingCount member variable to the class defined in `HelloWorld.hpp` and the constructor
> defined in `HelloWorld.cpp`. This looks like:
> 
> **HelloWorld.hpp: Adding New Member Variable**
> ```c++
> private:
>     U32 m_greetingCount;
> ```
> **HelloWorld.cpp: Updating Constructor**
> ```c++
> HelloWorld:: HelloWorld() :
>   m_greetingCount(0),
>   HelloWorldComponentBase() {
> ```

The component should build without errors by running `fprime-util build`.  Resolve any errors that occur before
proceeding to the next section.

## Conclusion

This tutorial has walked through the creation of component that implements a "Hello World" style greeting behavior for
our F´ system. In the next tutorial, this component will be hooked-up to an F´ deployment and tested!

**Next:** [Getting Started: Integration and Testing With F´ Deployments](./Deployments.md)