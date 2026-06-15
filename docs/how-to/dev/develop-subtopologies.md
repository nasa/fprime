# Develop a Subtopology

Subtopologies are topologies for smaller chunks of behavior in F Prime. It allows for grouping bits of topology architecture that fit together, to then be imported into a base deployment's topology. The use case for this is seen when working with shareable components, specifically in the form of [libraries](./develop-fprime-libraries.md).

*Contents*
1. [Subtopology Structure](#subtopology-structure)
2. [Individual File Contents](#individual-file-contents)
    1. [Example Scenario](#example-scenario)
3. [Integration into a "Main" Deployment](#integration-into-a-main-deployment)
4. [Subtopology Autocoder](#the-subtopology-autocoder)
5. [Conclusion](#conclusion)

## Subtopology Structure

A subtopology is a folder that contains a `topology.cpp` file, which includes a `topology.fpp` file, which is a *unified* topology file including your wiring, component instances, and init specifications. As opposed to having separate files for component instances and the topology itself, the single file encapsulates both. A `topologydefs.hpp` defines a struct that is used when instantiating your subtopology. A `CMakeLists.txt` file links your `topology.cpp` file to the build for your project.

Thus, the required structure for your subtopology should be:

```bash
MySubtopology/
├─ CMakeLists.txt
├─ MySubtopology.fpp
└─ MySubtopologyTopologyDefs.hpp
```

Subtopologies often ship with configuration modules as well expanding the format to look like this:

```bash
MySubtopology/
├─ MySubtopologyConfig/
|  ├─ CMakeLists.txt
|  └─ MySubtopologyConfig.fpp
├─ CMakeLists.txt
├─ MySubtopology.fpp
└─ MySubtopologyTopologyDefs.hpp
```

All files will be discussed in more detail in later sections of this guide. Additionally, note that there are optional files that can be included in your subtopology to extend its capability.

- It is highly recommended to include a `docs` folder to document your subtopology. Simple markdown files (`sdd` for subtopology design document) work well in this case.
- Other `.fpp` files can also be included in your subtopology.
- Unit tests can also be added to your subtopology, using a similar structure to unit tests for components.

> Note that with the latest release of `fprime-tools`, you can run `fprime-util new --subtopology` to generate the subtopology structure.

## Individual File Contents

This section will provide an overview to the contents of the individual files that make up a subtopology. We will use the names of the required files structure as shown in the [above](#subtopology-structure) section. However, note that the file names shown are not required to be duplicated within your own custom subtopologies.

### Example Scenario

For this section, it would be helpful to come up with an example scenario where the subtopology is used, such that it can be better understood. Let's imagine that we would like to create a component called `RNG` that will write a random integer to some telemetry channel when it is hooked up to a 1Hz clock (rate group). The `RNG` component will have an input port called `run`, that will be the entry point from the clock (rate group). The `RNG` component will be an active component, and is under the `MyLibrary` namespace.

### MySubtopology.fpp

As mentioned, this is a unified `.fpp` file. It not only includes the instances instance declarations for components that you'd like your subtopology to use, but also the definitions for the instances.

Based on our example, we may have the following instance declarations:

```
instance rng // RNG component instance
instance rateGroup // rate group instance
``` 

and thus the following instance definitions:

```
instance rng: MyLibrary.RNG base id 0xFF2FF \
    queue size Defaults.QUEUE_SIZE \
    stack size Defaults.STACK_SIZE \
    priority 100

instance rateGroup: Svc.ActiveRateGroup base id 0xFF4FF \
    queue size Defaults.QUEUE_SIZE \
    stack size Defaults.STACK_SIZE \
    priority 150
```

and also the following wiring:

```
connections MyWiring {
    // we will hook up the cycle for our rate group later on
    rateGroup.RateGroupMemberOut[0] -> rng.run
}
```

Thus, your unified fpp file, and so `MySubtopology.fpp` could look like:

```
module MySubtopology {
    instance rng: MyLibrary.RNG base id 0xFF2FF \
        queue size Defaults.QUEUE_SIZE \
        stack size Defaults.STACK_SIZE \
        priority 100

    instance rateGroup: Svc.ActiveRateGroup base id 0xFF4FF \
        queue size Defaults.QUEUE_SIZE \
        stack size DEFAULTS.STACK_SIZE \
        priority 150

    topology MySubtopology {
        instance rng // RNG component instance
        instance rateGroup // rate group instance

        connections MyWiring {
            rateGroup.RateGroupMemberOut[0] -> rng.run
        }
    } // end topology
} // end MySubtopology
```

## Unified fpp with Phases

"Phases" are a way to be able to write C++ based configuration functions on a component instance-basis. They aim to replace a topology.cpp/hpp pair of files with the single, unified fpp file. For subtopologies, we enforce the usage of phases to maintain consistency and simplicity in the subtopology.

In a standard cpp/hpp configuration model, your topology would be initialized using the following three functions:

```cpp
namespace MySubtopology{
    void configureTopology(const TopologyState& state) {
        // ... your code here ...
    }

    void startTopology(const TopologyState& state) {
        // ... your code here ...
    }

    void teardownTopology(const TopologyState& state) {
        // ... your code here ...
    }
}
```

In the phase pattern, these three (as well as others, [described in detail](https://nasa.github.io/fpp/fpp-users-guide.html#Defining-Component-Instances_Init-Specifiers)) are tied as "init specifiers" to fpp component instances. So, instead of configuring all components in one place, each instance defines its own init specification. 

This looks like the following example, and can be done inline within your unified fpp file:

```
passive component C1

instance c1: C1 base id 0x4444 \
{
    phase Fpp.ToCpp.Phases.configComponents """
        // your cpp code here for configuring
    """
}
```

As aforementioned, we enforce the utilization of phases for subtopologies. Notably, not shown here is that phases can still take in the `TopologyState state` variable. `TopologyState` is that struct, which can include any variables you would like a developer to be able to modify. This provides dynamic-ness to your subtopology.

We're now going to interject information about `MySubtopologyTopologyDefs.hpp`, because it is more relevant to the current topic.

## MySubtopologyTopologyDefs.hpp

In our example, we may want to allow the user to customize the initial seed for our random number generator:

```cpp
struct TopologyState {
    // ...
    U32 initialSeed;
    // ...
}
```

This struct definition is included within `MySubtopologyDefs.hpp`. In addition to this, we may notice that we need to include global definitions in our `Defs.hpp` file; however depending on your subtopology, you may find this part optional. An example of how these definitions may be written is here:

```cpp
// Example; may not be required

namespace GlobalDefs {
    namespace PingEntries {
        MySubtopology_rateGroup {
            enum { WARN = 3, ERROR = 5 };
        }
    } // end PingEntries
} // GlobalDefs
```

Notice that we wrap `PingEntries` in a namespace called `GlobalDefs`. This is important, as it allows us to use the namespace `GlobalDefs` across multiple topologies, and then link them all together in the main deployment. This will become more clear in the [next section](#integration-into-a-main-deployment).

This brings us to a unique naming scheme for variable names for subtopologies. On the development-end of the subtopology, we see names like `configureTopology`. However, on the build end when these subtopologies are folded into a bigger project, these functions are added to namespaces via their names. Inherently this makes sense, so that we don't confuse a function, or especially a component instance, with each other. So:

| fpp                     | cpp syntax              |
| ----------------------- | ----------------------- |
| MySubtopology.rateGroup | MySubtopology_rateGroup |

## CMake and buildstep files

The last step is to include our CMake-specific files. This includes `CMakeLists.txt`. There is a very repeatable structure, and inherently just notifies the compiler that our topology exists when we build a deployment. 

In `CMakeLists.txt`: 

```cmake
register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/RNGTopology.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/MySubtopologyTopologyDefs.hpp"
    INTERFACE
)
```

## Integration into a "Main" Deployment

At this point, we're ready to integrate our subtopology into the topology of our main deployment (you can think of this being our deliverable, with `MySubtopology` being a portion of it). We will assume, given our example, that you have the RNG component developed alongside the topology. Additionally, we assume that you have created an F Prime project and an associated deployment for it. Let's call this deployment "MainDeployment", and the project "MainProject".

First step is to ensure that our subtopology is linked to our project; within `project.cmake` add:

```cmake
# before the line adding your main topology
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/MySubtopology/")
```


Head over to `MainDeploymentTopologyDefs.hpp`. We want to not only include our subtopology's definitions header, but also modify `PingEntires` to use `GlobalDefs::PingEntries`. At the end of `namespace MainDeployment`, include:

```cpp
namespace PingEntries = GlobalDefs::PingEntries;
```

Then modify the current `PingEntries` namespace call to be surrounded by GlobalDefs:

```cpp
namespace GlobalDefs {
    namespace PingEntries {
        namespace blockDrv {
            enum { WARN = 3, FATAL = 5 };
        }
...
```

Then, we want to tell our MainDeployment's topology to import and use our unified topology file from our subtopology. While we're here, we should also hook up the `CycleOut` port of our main deployment's rate group driver to the `CycleIn` port of our subtopology's rate group. Ensure that the rate group driver has an appropriate output port array size. We head over to the `topology.fpp` file, and include:

```cpp
...
topology MainDeployment {
    import MySubtopology.MySubtopology
    ...

    connections RateGroups{
        ...
        rateGroupDriver.CycleOut[3] -> MySubtopology.rateGroup.CycleIn # you'll notice that the syntax here is <Namespace>.<instance>
        ...
    }
}
...
```

At this point, we want to now call our subtopology's configure/start/teardown functions within the corresponding functions of our `MainDeployment` topology. So, in `MainDeploymentTopology.cpp`:

<!-- {% raw %} -->
```cpp
// at the top, include our topology.hpp
#include <MySubtopology/MySubtopologyTopology.hpp>

...
// MODIFY this line to include the 4th divider
Svc::RateGroupDriver::DividerSet rateGroupDivisorsSet{{{1, 0}, {2, 0}, {4, 0}, {1, 0}}};

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
<!-- {% endraw %} -->

Lastly, since our RNG component has some telemetry, we need to include (or ignore) these channels within the `Packets.fppi` file in this folder. As with any other component that is added to a deployment, you use the same syntax with the name of the instance followed by the name of the telemetry channel.

Now go ahead and run and build your deployment, and you should see that you have a built deployment that uses a subtopology.

## Adding Subtopology Configuration

Adding subtopology configuration is done by adding a new module to the subtopology directory. Typically the name appends `Config` (e.g. `MySubtopologyConfig`).  This directory contains, at minimum, a `CMakeLists.txt` and configurable files.

In our case, there are two pieces that should be configured:

1. The subtopology base ID
2. Component properties: queue depth, stack size, and priority.

This can be done in `MySubtopologyConfig.fpp` as shown below.

```
module MySubtopologyConfig {
    #Base ID for the CdhCore Subtopology, all components are offsets from this base ID
    constant BASE_ID = 0xA0000000
    
    module QueueSizes {
        constant rng         = 10
        constant rateGroup   = 10
    }
    

    module StackSizes {
        constant rng       = 64 * 1024
        constant rateGroup = 64 * 1024
    }

    module Priorities {
        constant rng       = 89
        constant rateGroup = 90
    }
}
```

`MySubtopology.fpp` must be updated to use this configuration:

```
    instance rng: MyLibrary.RNG base id MySubtopologyConfig.BASE_ID + 0x1000 \
        queue size MySubtopologyConfig.QueueSizes.rng \
        stack size MySubtopologyConfig.StackSizes.rng \
        priority MySubtopologyConfig.Priorities.rng

    instance rateGroup: Svc.ActiveRateGroup base id MySubtopologyConfig.BASE_ID + 0x2000 \
        queue size MySubtopologyConfig.QueueSizes.rateGroup \
        stack size MySubtopologyConfig.StackSizes.rateGroup \
        priority MySubtopologyConfig.Priorities.rateGroup
```

> [!IMPORTANT]
> Configuration values should be set per-instance of a component.

Next, add a `CMakeList.txt` to your subtopology configuration module.

```
register_fprime_config(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/MySubtopologyConfig.fpp"
    INTERFACE
)
```

Update the subtopology to depend on config:

```
register_fprime_module(
    EXCLUDE_FROM_ALL
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/RNGTopology.fpp"
    HEADERS
        "${CMAKE_CURRENT_LIST_DIR}/MySubtopologyTopologyDefs.hpp"
    DEPENDS
        MySubtopology_MySubtopologyConfig
    INTERFACE
)
```

Users may now configure your subtopology!


# Conclusion

This how-to guide has walked through the development of a subtopology. Deployments can include multiple different subtopologies, and thus this feature truly paves the way for making F Prime more accessible to quick prototyping. 
