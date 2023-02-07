# Recommended F′ Development Process

The purpose of this guide is to lay out the standard F´ development process. This is the process used
by most developers who use F´ and, as such, many of the F´ tools are written to support the stages
of the process. This guide will walk through each step in this process.

The process:
1. [High Level Design](#high-level-design): Draft high level requirements and architecture.
2. [Setup Deployment](#setup-deployment): Setup project F´ deployment
3. [Develop Components](#develop-ports-and-components): Create individual project components
   - 3.1 [Design and Requirements](#design-and-requirements): Draft component requirements and interfaces
   - 3.2 [Creating a Port](#creating-a-port): Implement component interfaces
   - 3.3 [Creating a Component Definition](#creating-a-component-definition): Create initial mockup
     of component
   - 3.4 [Component Implementation](#component-Implementation): Implement component behavior
   - 3.5 [Component Unit Testing](#component-unit-testing): Test and verify component behavior
4. [Assemble Topology](#assemble-topology): Compose a F´ deployment from individual components
5. [Integration Testing](#integration-testing): Verify high-level project behaviors

## High-Level Design

The first step of the development process is to establish a high-level design for the project.
This involves specifying system-level requirements and a block diagram that represents the key
system functionality. Once complete the project should break this functionality into discrete units
of functionality that represent the system. In addition, the interface between these units should be
defined. The units of functionality are Components and the interfaces are further broken down into
discrete calls or actions through that interface. These are F´ ports. The full design of the system
of components and ports is the Topology. See: [Ports Components and
Topologies](../user/port-comp-top.md)

Next, the project should review the components provided by the F´ framework to see what
functionality can be inherited for free. This usually consists of the basic command and data
handling components, the Os layer, drivers, and other Svc components. Where possible, these
components should be used as-is to support a project to minimize extra work, but these may be
cloned and owned if they fall short of project requirements.

The project now has a list of what components they must provide, and what they will inherit.

## Setup Deployment

The next step for most projects is to prepare for development. This means getting enough of a
deployment setup such that the developers can be assigned components and ports to implement, and
test within a working deployment.

There are two options for creating a deployment:

You can create an in-tree deployment, where a deployment is created within the F´ git repository.
This is convenient and requires minimal setup, but placing your code within the F´ source tree
can make it harder to update F´ in the future if the project wants to take advantage of future
bug fixes and features. To create an in-tree deployment, the Ref application can be copied and used
as a starting point.

Another option is creating a standalone deployment, where the deployment is set up by itself and
points to an F´ installation. This requires more upfront work to setup, but provides the benefit
of separating mission code from F´ framework code.

The layout for a git repository for a standalone repository might look something like:

```
mission
├── mission_deployment_1
├── fprime (git submodule to fprime repository)
├── ...
└── library (git submodule to an external library)
```

After creating the project layout, external deployments can also be created by copying the Ref
application.

After copying the Ref app, both in-tree and standalone deployments need to create and modify a
`settings.ini` file in the root of the deployment to set the deployment toolchain, library
locations, and for standalone deployments, the location of the F´ framework.

For details on configuring the `settings.ini` file, see the [user guide page](../user/settings.md).


## Develop Ports and Components

Next, each developer is typically assigned a set of components to develop. This development starts
with the ports that are defined to be used by the component, and then the component itself. This
development is described in the following sections.

### Design and Requirements

Using the high-level requirements, the developer should define requirements for an individual
component. These requirements should define the behaviors of the component, as well as the
interfaces with other components.

### Creating a Port

Once the interfaces between components have been defined, ports should be created to implement those
interfaces. This can be done by hand or by using `fprime-util new --port`.

It's recommended that ports are kept in their own directories, separate from components.

To create a new port:

1. If necessary, create a new port directory
2. Create a new port `*Ai.xml` file, possibly by copying from an existing port.
3. Add the new port xml file to `SOURCE_FILES` in the `CMakeLists.txt` file in the directory
4. If necessary, add the port directory to the deployment's cmake file with `add_fprime_subdirectory`.

Alternatively, you may use `fprime-util new --port` from the fprime-tools package. This will 
walk the user through a few prompts about the port they want to create. Then the following
will be done automatically:

1. If the specified directory for the port does not exist, it will be created
2. The `*Ai.xml` file will be generated, with information and arguments filled in
3. The port will be added to the source files of `CMakeLists.txt`. If there is no `CMakeLists.txt`
   file, one will be automatically generated and filled out
4. If necessary, the port directory will be added to the deployment's cmake file with 
   `add_fprime_subdirectory`

### Creating a Component Definition

The first step in creating a component is to create the component xml definition, which defines
which interfaces it implements, what commands it supports, which telemetry it provides, and what
events it produces. This can be done by hand or by using `fprime-util new --component`.

To create a new component definition by hand:

1. Create a new component directory
2. Create a new component `*Ai.xml` file, possibly by copying from an existing component.
3. Optional, create a commands xml file for GDS commands and include it in the component xml file.
4. Optional, create an events xml file and include it in the component xml file.
5. Optional, create a telemetry xml for telemetry channels and include it in the component xml file.
6. Create a component `CMakeLists.txt` test file and add the component xml file to `SOURCE_FILES`
   variable in the file.
7. Add component directory to the deployment's cmake file with `add_fprime_subdirectory`.

Alternatively, you may use `fprime-util new --component` from the fprime-tools package. This will 
walk the user through a few prompts about the component they are creating. Then the following
will be done automatically:

1. A new component directory will be created
2. The `*Ai.xml` file will be generated and filled out with all of the information provided by the user
3. Commands, telemetry, events, and parameters will be added to the xml file based on what the user chooses through the prompts
4. Ports necessary for commands, telemetry, events, and parameters will be automatically added to the `*Ai.xml` file depending which elements the user chooses to include
5. A component `CMakeLists.txt` file will be generated and the component xml will be added
   to the source files.
6. The component directory will be added to the deployments cmake file with
   `add_fprime-subdirectory`
7. The user is given the option to generate implementation `*.cpp` and `*.hpp` files
8. The user is given the option to generate a unit test directory with necessary
   unit test files within it.
9. An SDD file is generated with documentation about ports, commands, events,  
   telemetry, parameters, and time of creation already filled out

The `fprime-util new --component` uses the built-in cookiecutter template by default, 
but users can substitute their own component template by using the component_cookiecutter 
field of the settings.ini file. To learn more, see [Creating and Using a Cookiecutter Template](../dev/cookiecutter.md).
### Component Implementation

Next, the developer typically runs `fprime-util impl` to produce `-template` files of the hand-coded
.cpp and .hpp files. However, if your component was created with `fprime-util new --component` and 
you select yes when asked about generating implementation files, this is automatically done. 
These can be used as a basis for implementation with all the stubs in place for the developer to 
implement the design. Developers then fill in these files and stubs with an implementation that 
supports the functionality of the design.

The component can then be built as development proceeds to look for errors.  Ports are entirely
autogenerated and do not need an implementation.

### Component Unit Testing

Along with implementation, unit tests can be templated and implemented to test against the
requirements of the component. These should be developed and run often to ensure the implemented
component works.

Note: If `fprime-util new --cookiecutter` was used to create the component, and the user chose
the generate unit tests, steps 1-3 should be skipped because these are automatically done.

To add unit tests to a component:

1. Create a test directory within the component (usually called `test`).
2. Run `fprime-util impl --ut` to generate unit testing code skeleton
3. Add unit test sources to `UT_SOURCE_FILES` and register unit tests with `register_fprime_ut()` in
   the component `CMakeLists.txt`.
4. Run `fprime-util check` in the component directory to run unit tests.

## Assemble Topology

As components are completed, it is helpful to add them to the topology. As more components are
completed, the topology is slowly built up over time. This can enable integration tests early on in
the project. The full deployment should be built at this stage to ensure that there are no errors.

To add a component to the topology:

1. In the topology `*Ai.xml` file
    - Import the component `*Ai.xml` xml file.
    - Instantiate the component as many times as necessary.
    - Connect component output ports with the corresponding input port and vice versa.
2. In the topology `Components.hpp` file, declare the component with the same name as the topology
   xml file.
3. In the topology `Topology.cpp` file:
    - Instantiate the component.
    - Call the component's `init` function.
    - If additional setup is required, call a user-defined setup function.
    - If using commands, register the component's commands.
    - If using health checking, add the component to ping entries.
    - If using an active component, start the component with `start` function and call `exit` when
      exiting.

## Integration Testing

As the topology comes together, it is helpful to write system-level tests for subsystems of the
overall deployment. This makes sure that as a system, top-level requirements are met.

The fprime-gds has a python API that can be used to write integration test cases that support sending commands, checking for events, and getting telemetry channel readings. To get started with writing integration tests, check out the [GDS integration test guide](../dev/testAPI/user_guide).

## Conclusion

This guide was an overview of the workflow most F´ developers follow, from the start to completion
of a project. These workflows can be applied to any F´ project and should hopefully provide the
smoothest development experience.