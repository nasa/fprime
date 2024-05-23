# How-To: Develop an F' Subtopology

Subtopologies are topologies for smaller chunks of behavior in F Prime. It allows for grouping bits of topology architecture that fit together, to then be imported into a base deployment's topology. The use case for this is seen when working with shareable components, specifically in the form of [libraries](./develop-fprime-libraries.md).

*Contents*
1. [Subtopology Structure](#subtopology-structure)
2. [Individual File Contents](#individual-file-contents) (this is a long section with multiple subsections!)
3. [Integration into a "Main" Deployment](#integration-into-a-main-deployment)
4. [Conclusion](#conclusion)

## Subtopology Structure

A subtopology is a folder that contains a `topology.cpp` file, which includes the behavior for setting up your custom topology. It additionally contains a `topology.fpp` file, which is a *unified* topology file. As opposed to having separate files for component instances and the topology itself, the single file encapsulates both. A `topologydefs.hpp` defines a struct that is used when instantiating your subtopology. A `CMakeLists.txt` file links your `topology.cpp` file to the build for your project.

Thus, the required structure for your subtopology should be:

```bash
MySubtopology/
├─ CMakeLists.txt
├─ MySubtopology.cmake
├─ MySubtopology.fpp
├─ MySubtopologyTopology.cpp
├─ MySubtopologyTopology.hpp
└─ MySubtopologyTopologyDefs.hpp
```

These files will be discussed in more detail in later sections of this guide. Additionally, note that there are optional files that can be included in your subtopology to extend its capability.

- It is highly recommended to include a `docs` folder to document your subtopology. Simple markdown files work well in this case.
- `.fppi` files can be included as config files to your subtopology. They can include useful constants or structures that allow users to modify your subtoplogy.
- Unit tests can also be added to your subtopology, using a similar structure to unit tests for components.

## Individual File Contents

This section will provide an overview to the contents of the individual files that make up a subtopology. We will use the names of the required files structure as shown in the [above](#subtopology-structure) section. However, note that the file names shown are not required to be duplicated within your own custom subtopologies.

### Example Scenario

For this section, it would be helpful to come up with an example scenario where the subtopology is used, such that it can be better understood. Let's imagine that we would like to create a component called `RNG` that will write a random integer to some telemetry channel when it is hooked up to a 1Hz clock (rate group). The `RNG` component will have an input port called `run`, that will be the entry point from the clock (rate group). The `RNG` component will be an active component, and is under the `MyLibrary` namespace.

### MySubtopology.fpp

As mentioned, this is a unified `.fpp` file. It not only includes the instances instance declarations for components that you'd like your subtopology to use, but also the definitions for the instances.

Based on our example, we may have the following instance declarations:

```cpp
instance rng // RNG component instance
instance rateGroup // rate group instance
instance rateGroupDriver // driving the rate group
``` 

and thus the following instance definitions:

```cpp
instance rng: MyLibrary.RNG base id 0xFF2FF \
    queue size Defaults.QUEUE_SIZE \
    stack size Defaults.STACK_SIZE \
    priority 100

instance rateGroup: Svc.ActiveRateGroup base id 0xFF4FF \
    queue size Defaults.QUEUE_SIZE \
    stack size DEFAULTS.STACK_SIZE \
    priority 150

instance rateGroupDriver: Svc.RateGroupDriver base id 0xFF8FF // to drive the 1Hz rate group
```

and also the following wiring:

```cpp
connections MyWiring {
    rateGroupDriver.CycleOut[0] -> rateGroup.CycleIn // to send the 1Hz configurable signal to our rate group
    rateGroup.RateGroupMemberOut[0] -> rng.run
}
```

Thus, your unified fpp file, and so `MySubtopology.fpp` could look like:

```cpp
module MySubtopology {
    instance rng: MyLibrary.RNG base id 0xFF2FF \
        queue size Defaults.QUEUE_SIZE \
        stack size Defaults.STACK_SIZE \
        priority 100

    instance rateGroup: Svc.ActiveRateGroup base id 0xFF4FF \
        queue size Defaults.QUEUE_SIZE \
        stack size DEFAULTS.STACK_SIZE \
        priority 150

    instance rateGroupDriver: Svc.RateGroupDriver base id 0xFF8FF // to drive the 1Hz rate group

    topology MySubtopology {
        instance rng // RNG component instance
        instance rateGroup // rate group instance
        instance rateGroupDriver // driving the rate group

        connections MyWiring {
            rateGroupDriver.CycleOut[0] -> rateGroup.CycleIn // to send the 1Hz configurable signal to our rate group
            rateGroup.RateGroupMemberOut[0] -> rng.run
        }
    } // end topology
} // end MySubtopology
```

## MySubtopology.cpp and MySubtopology.hpp (pt1)

This file is the bread and butter of developing your topology, as it runs any functions necessary to configure the topology, and also includes start and teardown functions for when the topology is started and after the topology is stopped respectively.

As such, this file requires three implemented functions. The stubs of these functions should go in the `MySubtopology.hpp` file:

```cpp
void configureTopology(const TopologyState& state) {
    // ... your code here ...
}

void startTopology(const TopologyState& state) {
    // ... your code here ...
}

void teardownTopology(const TopologyState& state) {
    // ... your code here ...
}
```

You may notice that all three functions take in the argument `state`, which is a struct that is passed in when the subtopology is called. `TopologyState` is that struct, which can include any variables you would like a developer to be able to modify. This provides dynamic-ness to your subtopology.

We're now going to interject information about `MySubtopologyDefs.hpp`, because it is more relevant to the current topic.

## MySubtopologyDefs.hpp

In our example, we may want to allow the user to customize the clock that runs the RNG component, from 1Hz to 2Hz or otherwise. Thus, our struct may look something like:

```cpp
struct TopologyState {
    // ...
    U32 clockRate;
    // ...
}
```

This struct definition is included within `MySubtopologyDefs.hpp`, and is included into `MySubtopology.cpp` at build time. In addition to this, we may notice that we need to include global definitions in our `Def.hpp` file; however depending on your subtopology, you may find this part optional. Since we use F Prime's rate group driver, we should include "WARN" and "ERROR" flags that are tied to the status of the driver. We'll also add these to `MySubtopologyDefs.hpp`.

```cpp
namespace GlobalDefs {
    namespace PingEntries {
        MySubtopology_rateGroup {
            enum { WARN = 3, ERROR = 5 };
        }
    } // end PingEntries
} // GlobalDefs
```

This brings us to a unique naming scheme for variable names for subtopologies. On the development-end of the subtopology, we see names like `configureTopology`. However, on the build end when these subtopologies are folded into a bigger project, these functions are added to namespaces via their names. Inherently this makes sense, so that we don't confuse a function, or especially a component instance, with each other. So:

```cpp
MySubtopology -> configureTopology() ==> MySubtopology::configureTopology()
MySubtopology -> startTopology() ==> MySubtopology::startTopology()
MySubtopology -> rateGroup ==> MySubtopology_rateGroup // example for rateGroup instance
// ... etc
```

We will encounter this structure later on in this guide. At the moment, know that `MySubtopology_rateGroup` is not a foreign concept!

## MySubtopology.cpp and MySubtopology.hpp (pt2)

Back to the `.cpp` and `.hpp` files. Let's go through each of the functions that are contained here to see what can go within each.

### `configureTopology()`

This function contains any setup that needs to be done for your topology. This will depend on the components you instantiate. It can range from setting up your rate group (which we will do) to defining your framing/deframing protocol for a custom (or F Prime's) communication engine. In our situation, we need to set up the rate group driver.

```cpp
// Rate groups may supply a context token to each of the attached children whose purpose is set by the project. The
// reference topology sets each token to zero as these contexts are unused in this project.
NATIVE_INT_TYPE rateGroupContext[Svc::ActiveRateGroup::CONNECTION_COUNT_MAX] = {};

void configureTopology(const TopologyState& state){
    U32 clockDivisor = 10; // default 1Hz
    if(state.clockRate){
        // ... do some verification of the input
        clockDivisor = 10/state.clockRate; // assuming the input frequency is 10Hz
    }

    // The reference topology divides the incoming clock signal (10Hz) into sub-signals: 1Hz
    Svc::RateGroupDriver::DividerSet rateGroupDivisors = {{{clockDivisor, 0}}};
    MySubtopology_rateGroupDriver.configure(rateGroupDivisors);
    MySubtopology_rateGroup.configure(rateGroupContext, Fw_NUM_ARRAY_ELEMENTS(rateGroupContext));
}
```

### `startTopology()`

This function contains all function calls that will run on start up of the topology. In our situation, none of our instantiated components have any start up calls. However, you can imagine that something like F Prime's communication driver may want to start up the communication engine on start of the topology. For a placeholder, let's do a simple print that says we've started.

```cpp
void startTopology(const TopologyState& state){
    Fw::Logger::logMsg("[RNG Topology] Topology has been started.")
}
```

### `teardownTopology()`

This function contains all the functions that are meant for clean up of the instantiated components. Again, in our example, we don't really have anything to clean up. But, a communication driver instance may want to clean up by terminating any active connections. For a placeholder, let's do a simple print that says we're all terminated.

```cpp
void teardownTopology(const TopologyState& state){
    Fw::Logger::logMsg("[RNG Topology] Topology has been cleaned up.")
}
```

## CMake and buildstep files

The last step is to include our CMake-specific files. This includes `CMakeLists.txt` and `MySubtopology.cmake`. There is a very repeatable structure, and inherently just notifies the compiler that our topology exists when we build a deployment. 

In `CMakeLists.txt`: 

```cmake
set(SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/MySubtopology.fpp"
)

register_fprime_modules()
```

and in `MySubtopology.cmake`:

```cmake
list(APPEND MOD_DEPS
    Fw/Logger
)

list(APPEND SOURCE_FILES
    "${CMAKE_CURRENT_LIST_DIR}/BaseTopology.cpp"
)
```

# Integration into a "Main" Deployment

At this point, we're ready to integrate our subtopology into the topology of our main deplopyment (you can think of this being our deliverable, with `MySubtopology` being a portion of it). We will assume, given our example, that you have the RNG component developed alongside the topology. Additionally, we assume that you have created an F Prime project and an associated deployment for it. Let's call this deployment "MainDeployment", and the project "MainProject".

First step is to ensure that our subtopology is linked to our project; within `project.cmake` add:

```
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MySubtopology/")
```

Then, let's `cd` into `MainDeployment/Top/`. We have some files here we need to configure to use our subtopology. Before anything let's go to `CMakeLists.txt` to include your own subtopology's CMake file:

```cmake
...
include("${CMAKE_CURRENT_LIST_DIR}/../../MySubtopology/MySubtopology.cmake")
register_fprime_module()
...
```

Then, we want to tell our MainDeployment's topology to import and use our unified topology file from our subtopology. We head over to the `topology.fpp` file, and include:

```cpp
...
topology MainDeployment {
    import MySubtopology.MySubtopology
    ...
}
...
```

At this point, we want to now call our subtopology's configure/start/teardown functions within the corresponding functions of our `MainDeployment` topology. So, in `MainDeployment.cpp`:

```cpp
// at the top, include our topology.hpp
#include <MySubtopology/MySubtopology.hpp>

...

void configureTopology() {
    ...
    MySubtopology::TopologyState = myState;
    myState.clockRate = 1; // we set our clock rate to whatever standard we want
    MySubtopology::configureTopology(myState);
}

...

void setupTopology(const TopologyState& state){
    ...
    MySubtopology::startTopology({});
}

...

void teardownTopology(const TopologyState& state){
    ...
    MySubtopology::teardownTopology({})
}
```

Lastly, since our RNG component has some telemetry, we need to include (or ignore) these channels within the `Packets.xml` file in this folder. As with any other component that is added to a deployment, you use the same syntax with the name of the instance followed by the name of the telemetry channel. For example:

```xml
<channel name="rng.RNGValue"/> <!-- based on our instance name -->
```

Now go ahead and run and build your deployment, and you should see that you have a built deployment that uses a subtopology.

# Conclusion

This how-to guide has walked through the development of a subtopology. Deployments can include multiple different subtopologies, and thus this feature truly paves the way for making F Prime more accessible to quick prototyping. 

If you'd like to see an example of how subtopologies are used within an actual project, please reference this repository: [mosa11aei/fprime-rngLibrary](https://github.com/mosa11aei/fprime-rngLibrary).