# Hello World
Reference: [Hello World Tutorial Github Repository](https://github.com/fprime-community/fprime-tutorial-hello-world)

## Introduction and F´ Terminology

The Hello World tutorial is designed to teach new users the basics of F´ usage, instruct existing users on new
command that help in F´ development, and act as the canonical "Hello World" example for F´.

This tutorial walks through the following steps:

1. [Creating an F´ Project](#1-creating-an-f-project)
2. [Creating an F´ Hello World Component](#2-creating-an-f-hello-world-component)
3. [Integration and Testing With F´ Deployments](#3-integration-and-testing-with-f-deployments)

Once finished, users should have a good understanding of the basic development mechanics for working with F´ and then
could dive deeper into concepts through the [Math Component Tutorial](math-component.md). 


## F´ Terminology

F´ uses specific terminology to refer to specific parts of the system. This section dives into the basic F´ terminology
used in this tutorial and an explanation of how the terminology is used.

#### Project

An F´ project is a collection of files and folders used to work with F´. At its core, a project is just a folder that
can be used to hold F´ code. Projects should specifically exclude the core F´ library to avoid the "clone and own"
problem. Rather projects should link-to a version of F´.

This tutorial will create a new `MyProject` project used to contain the other things created by the tutorial.

#### Component

An F´ component encapsulates a unit of system behavior. Components use ports to communicate with other components and
define commands, events, telemetry channels, and parameters.

This tutorial will create a new `HelloWorld` component that defines a `SAY_HELLO` command, `Hello` event, and
`GreetingCount` telemetry channel. The component only used built-in ports. It does not use custom ports nor parameters.

#### Port

A port is an interface used to communicate between components. This tutorial only uses built-in ports.

#### Command

Commands represent actions that a component can execute. Commands can be sent from the ground or via command sequences.
Command behavior is defined in a command handler defined in the component's implementation.

This tutorial defines one command `SAY_HELLO` with a single argument `greeting`. This command will be sent via the
ground system and will echo the greeting back via the `Hello` event.

#### Event

Events represent actions that a component has performed. Events are akin to software log messages. Events are received
and displayed by the ground system.

This tutorial defines one event `Hello` emitted in response to the `SAY_HELLO` command and containing the same greeting
that the command had.

#### Telemetry Channel

Telemetry channels provide the active state of the component. Each channel represents a single item of state that will
be sent to the ground system.

This tutorial defines a single channel `GreetingCount` that contains the count of the number of `SAY_HELLO` greeting
commands received.

#### Deployment

Deployments are a single build of F´ software. Each deployment results in one software executable that can be run along
with other associated files. Each deployment has a topology that defines the system.

This tutorial will create the `HelloWorldDeployment` deployment, run this deployment, and test it through the F´ GDS.

#### Topology

Topologies are networks of connected components that define the software. Multiple instances of a component may be added
to the topology.

This tutorial will use a standard command and data handling topology. A single `HelloWorld` component instance called
`helloWorld` will be added to the standard topology.

---


## 1. Creating an F´ Project 

This tutorial will walk new users through creating a new F´ project. First, ensure you meet the [F´ System Requirements](../../getting-started/installing-fprime.md#system-requirements).

### Tutorial Steps:

- [Bootstrapping F´](#bootstrapping-f)
- [Building the New F´ Project](#building-the-new-f-project)
- [Conclusion](#hello-world-step-1-conclusion)

### Bootstrapping F´

An F´ [project](../../getting-started/projects.md) ties to a specific version of tools to work with F´. In order to create
this project and install the correct version of tools, you should perform a bootstrap of F´.

#### 1. Install the F´ Bootstrap tool


The F´ Bootstrap tool is responsible for creating a new F´ project and installing the Python dependencies within the project's virtual environment. Install the fprime-bootstrap tool with:
```
pip install fprime-bootstrap
```

#### 2. Create a new project

The entrypoint to developing with F´ is creating a new project. This will clone the F´ repository and install the full tool suite of the specified version for working with the selected version of F´. To create a new project, run:
```
fprime-bootstrap project
```

This command will ask for a project name. We will use the default: `MyProject`
```
  [1/1] Project name (MyProject): MyProject
```

#### 3. Understanding the project structure

Bootstrapping your F´ project created a folder called `MyProject` (or any name you chose) containing the standard F´ project structure as well as the [virtual environment](https://packaging.python.org/en/latest/guides/installing-using-pip-and-virtual-environments/) containing the tools to work with F´.

We should navigate to the project's directory and look around:

```bash
cd MyProject
ls
```
This will show the following files:
1. `fprime/`: F´ repository, this is a git submodule that points to https://github.com/nasa/fprime. Contains core F´ components, the API for the build system, among others
2. `settings.ini`: allows users to set various settings to control the build
3. `CMakeList.txt` and `project.cmake`: CMake files defining the build system
4. `Components/`: directory to place user components in
4. `fprime-venv/`: this directory is the virtual environment containing the Python tools to work with F´

#### 4. Activate the virtual environment
Activate the virtual environment to use the F´ tool suite.

```bash
# in MyProject/
. fprime-venv/bin/activate
```
!!! tip
    Always remember to activate the virtual environment whenever you work with this F´  project.

### Building the New F´ Project

The next step is to set up and build the newly created project. This will serve as a build environment for any newly
created components, and will build the F´ framework supplied components.

```bash
cd MyProject
fprime-util generate
fprime-util build
```

!!! note
    `fprime-util generate` sets up the build environment for a project/deployment. It only needs to be done once.

!!! note
    `fprime-util build` can be sped up by building in parrallel on multiple cores, using the `-j <N>` option. For example, `fprime-util build -j16`

### Hello World Step 1 Conclusion

A new project has been created with the name `MyProject` and has been placed in a new folder called `MyProject` in
the current directory. It includes the initial build system setup, and F´ version. It is still empty in that the user
will still need to create components and deployments.

For the remainder of this Hello World tutorial we should use the tools installed for our project and issue commands
within this new project's folder:

```bash
# In: MyProject
. fprime-venv/bin/activate
```
!!! note
    Use this command if your virtual environment is not already running. 

---


## 2. Creating an F´ Hello World Component

This tutorial will walk new users through creating a basic F´ component. Users should have completed the new project
tutorial and have the tools sourced as shown in the [conclusion](#hello-world-step-1-conclusion) portion of that tutorial.

F´ components encapsulate the various parts of system behavior. These components can interact with the ground system
through [commands](#command), [events](#event), and
[telemetry channels](#telemetry-channel). Components communicate with other components through
[ports](#port), which are covered in-depth in [another tutorial](math-component.md#constructing-ports). 

### Prerequisites:

- [Hello World: Creating an F´ Project](#1-creating-an-f-project)

### Tutorial Steps:

- [Hello World Component](#hello-world-component-requirements)
- [Creating the Hello World Component](#creating-the-hello-world-component)
- [Editing the Component Model](#editing-the-component-model)
- [Implementing Component Behavior](#implementing-component-behavior)
- [Conclusion](#hello-world-step-2-conclusion)

### Hello World Component Requirements

The first step for creating a new component is understanding what it is that we wish to implement. This is called
defining requirements. In the spirit of "Hello World" this component will encapsulate greeting behavior. The component
will define three items to implement greeting behaviour:

1. A [command](#command) called `SAY_HELLO` that will command the component to send a greeting
2. An [event](#event) called `Hello` that is the greeting sent in response to the `SAY_HELLO` command
3. A [telemetry channel](#telemetry-channel) called `GreetingCount` that will count each `Hello` event sent

These are a simple set of requirements for this component.

### Creating the Hello World Component

The next step is to create the new component. The project contains a `Components/` directory to create components in. 

```bash
# In: MyProject
cd Components
```

Creating a new component is accomplished with the following command:

```bash
# In: MyProject/Components
fprime-util new --component
```
This command will ask for some input. You should respond with the following answers:

```
[INFO] Cookiecutter source: using builtin
Component name [MyComponent]: HelloWorld
Component short description [Component for F Prime FSW framework.]: Hello World Tutorial Component
Component namespace [Components]: Components
Select component kind:
1 - active
2 - passive
3 - queued
Choose from 1, 2, 3 [1]: 1
Enable Commands?:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
Enable telemetry?:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
Enable Events:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
Enable Parameters:
1 - yes
2 - no
Choose from 1, 2 [1]: 1
[INFO] Found CMake file at 'Components/CMakeLists.txt'
Add component Components/HelloWorld to Components/CMakeLists.txt at end of file (yes/no)? yes
Generate implementation files (yes/no)? yes
```

!!! note
    For any other questions, select the default response.

This will create a new component called "HelloWorld" in the "Components" namespace. This new component will be able to define commands, events, telemetry channels, and parameters.

We should navigate to the component's directory and look around:

```bash
# In: MyProject/Components
cd HelloWorld
ls
```
This will show the following files:

1. `HelloWorld.fpp`: design model for the component
2. `HelloWorld.hpp` and `HelloWorld.cpp`: C++ implementation files for the component, currently empty.
3. `CMakeList.txt`: build definitions for the component.
4. `docs` folder to place component documentation

To build this component run `fprime-util build` in the current folder.

!!! note
    Any component in F´ can be built by navigating to the component's folder and running `fprime-util build`.

### Editing the Component Model

A component model defines the interface of the component with the rest of the F´ system and with the ground system F´
communicates with. In this case we intend to define a command, an event, and a telemetry channel as specified above.

Open the model file `HelloWorld.fpp` and replace the line:

```
async command TODO opcode 0
```

with the following:

```
@ Command to issue greeting with maximum length of 20 characters
async command SAY_HELLO(
    greeting: string size 20 @< Greeting to repeat in the Hello event
)

@ Greeting event with maximum greeting length of 20 characters
event Hello(
    greeting: string size 20 @< Greeting supplied from the SAY_HELLO command
) severity activity high format "I say: {}"

@ A count of the number of greetings issued
telemetry GreetingCount: U32
```
!!! note
    You should ensure to replace any existing command, event, and channel definitions with those supplied above but leave the 'Standard AC Ports' section untouched.

With this step completed you can generate a basic implementation with the following command:

```bash
# In: MyProject/Components/HelloWorld
fprime-util impl
```

`fprime-util impl` creates `HelloWorld.hpp-template` and `HelloWorld.cpp-template` which contain empty functions based on what we have written in the FPP file. While normally one would merge new templates with the existing code, we will instead overwrite the existing implementations as we have not edited those files yet. To do this:

```bash
mv HelloWorld.template.hpp HelloWorld.hpp
mv HelloWorld.template.cpp HelloWorld.cpp
```
We are now ready for implementing component behavior.

### Implementing Component Behavior

F´ behavior is implemented in two types of methods: command handler functions to implement command behavior and handler
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
!!! note
    We must also add the m_greetingCount member variable to the class defined in `HelloWorld.hpp` and the constructor defined in `HelloWorld.cpp`. This looks like:
    
    **HelloWorld.hpp: Adding New Member Variable**
    ```c++
    PRIVATE:
        U32 m_greetingCount;
    ```
    
    Should be added inside the `class` definition in `HelloWorld.hpp`.

    **HelloWorld.cpp: Updating Constructor**
    ```c++
    HelloWorld:: HelloWorld(const char *const compName) : HelloWorldComponentBase(compName),
        m_greetingCount(0)
    {
    }
    ```
    
    Should be added to the `HelloWorld` constructor at the top of the file.

The component should build without errors by running `fprime-util build`.  Resolve any errors that occur before
proceeding to the next section. Remember to always save before you build, unsaved changes will not be included in the build.

### Hello World Step 2 Conclusion

This tutorial has walked through the creation of component that implements a "Hello World" style greeting behavior for
our F´ system. In the next tutorial, this component will be hooked-up to an F´ deployment and tested!

---


## 3. Integration and Testing With F´ Deployments

This section will walk new users through creating a new F´ [deployment](#deployment). This deployment will
build a [topology](#topology) containing the standard F´ stack of components and a single `HelloWorld`
component instance. The `HelloWorld` was created in the [last section](#2-creating-an-f-hello-world-component). The tutorial will close by
testing the deployment and `HelloWorld` component through the `fprime-gds`.

### Prerequisites:

- [Hello World: F´ Hello World Component](#2-creating-an-f-hello-world-component)

### Tutorial Steps:

- [Creating A New Deployment](#creating-a-new-deployment)
- [Adding The Hello World Component](#adding-the-hello-world-component)
- [Running With `fprime-gds`](#running-with-fprime-gds)
- [Conclusion](#hello-world-step-3-conclusion)

### Creating A New Deployment

F´ deployments represent one flight software executable. All the components we develop for F´ run within a deployment.
The deployment created here will contain the standard command and data handling stack. This stack enables
ground control and data collection of the deployment.

To create a deployment, run the following commands:
```bash
# In: MyProject
fprime-util new --deployment
```
This command will ask for some input. Respond with the following answers:

```
Deployment name [MyDeployment]: HelloWorldDeployment
```

!!! note
    For any other questions, select the default response.

At this point, the `HelloWorldDeployment` has been created, but our `HelloWorld` component has not been added to the deployment.

### Adding The Hello World Component

In this section the `HelloWorld` component will be added to the `HelloWorldDeployment` deployment. This can be done by adding the component to the topology defined in `HelloWorldDeployment/Top`. 

Topologies instantiate all the components in a running system and link them together. For some port types, like the commanding, event, and telemetry ports used by `HelloWorld`, the connections are made automatically. 
In addition, the topology specifies how to construct the component instance. This is also done automatically unless the component has specific configuration.

In order to add a component to the topology, it must be added to the topology model. An instance definition and an instance initializer must both be added.

To add an instance definition, add `instance helloWorld` to the instance definition list in the `topology HelloWorldDeployment` section of `HelloWorldDeployment/Top/topology.fpp`. This is shown below.

Edit `HelloWorldDeployment/Top/topology.fpp`:
```
...
    topology HelloWorldDeployment {
        # ----------------------------------------------------------------------
        # Instances used in the topology
        # ----------------------------------------------------------------------
        
        instance ...
        instance ...
        instance helloWorld
```
!!! warning
    Be careful to not remove any other instances from the list.

`helloWorld` is the name of the component instance. Like variable names, component instance names should be descriptive
and are typically named in camel or snake case.

Next, an instance initializer must be added to topology instances defined in `MyDeploymment/Top/instances.fpp` file.
Since the `HelloWorld` component is an `active` component it should be added to the active components section and should
define a priority and queue depth options.  This is shown below.

Add to `MyDeploymment/Top/instances.fpp`:
```
...
  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------
  instance ...
    ...
    ...
    ...
    
  instance ...
  
  instance helloWorld: Components.HelloWorld base id 0x0F00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 50
```
!!! note
    The user must ensure that the base id (0x0F00) does not conflict with any other base ids in the topology. 0x0F00 should be safe for deployments created with `fprime-util new --deployment`.

!!! note
    Make sure to use the same instance name (i.e. helloWorld) as defined in the instance definition just added to `topology.fpp`.

Finally, our new telemetry channel should be added to our telemetry packet specification. For this tutorial the
channel can be ignored as the deployment will not use the telemetry packetizer. Add the following to the `ignore`
section of `HelloWorldDeployment/Top/HelloWorldDeploymentPackets.xml`.

Update `HelloWorldDeployment/Top/HelloWorldDeploymentPackets.xml`:
```
    <ignore>
        ...
        <channel name="helloWorld.GreetingCount"/>
    </ignore>
```

Since this component has no custom ports nor does it require special configuration, our addition to the topology is
completed. The deployment can now be set up and built using the following commands:

```
# In: MyProject/HelloWorldDeployment
fprime-util build -j4
```
!!! warning
    Resolve any errors that occur before continuing to the running section.

### Running With `fprime-gds`

It is now time to test the `HelloWorld` component by running the deployment created in this section. This can be
accomplished by running the `fprime-gds` command in the deployment, verifying connection, sending the new SEND_HELLO
command and verifying that the `Hello` event and `GreetingCount` channel appears.

To start the deployment with default settings, run:
```bash
fprime-gds
```

The F´ GDS control page should open up in your web browser. If it does not open up, navigate to `http://127.0.0.1:5000`.

Once the F´ GDS page is visible, look for a green circle icon in the upper right corner. This shows that the flight
software deployment has connected to the GDS system. If a red X appears instead, navigate to the Logs tab and look for
errors in the various logs.

Now that communication is verified, navigate to the "Commanding" tab and select `helloWorld.SAY_HELLO` from the
dropdown list. Type a greeting into the argument input box and click the button "Send Command". If the argument has
validated successfully the command will send. Resolve all errors and ensure the command has sent.

!!! note
    Notice commands are instance specific. Had several HelloWorld component instances been used, there would be multiple `SAY_HELLO` listings, one for each component instance.

Now that the command has sent, navigate to the "Events" tab. Ensure that the event list contains the Hello event with
the text entered when sending the command.

Lastly, navigate to the "Channels" tab. Look for "helloWorld.GreetingCount" in the channel list. Ensure it has recorded
the number of times a `helloWorld.SAY_HELLO` was sent.

Congratulations, you have now set up a project, component, and deployment in F´.

### Hello World Step 3 Conclusion

This concludes both the adding deployment section of the Hello World tutorial and the tutorial itself. The user has
been able to perform the following actions:

1. Create a new blank F´ projects
2. Create a new F´ components
3. Create a new F´ deployments and add components it

To explore components more in-depth and see how components communicate with one another, see the
[Math Component Tutorial](math-component.md).

[Next Step: Math Component Tutorial](math-component.md){ .md-button .md-button--primary }
