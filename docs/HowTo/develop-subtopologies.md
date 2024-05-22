# How-To: Develop an F' Subtopology

Subtopologies are topologies for smaller chunks of behavior in F Prime. It allows for chunking bits of topology architecture that fit together, to then be imported into a base deployment's topology. The use case for this is seen when working with shareable components, specifically in the form of [libraries](./develop-fprime-libraries.md).

*Contents*
1. [Subtopology Structure](#subtopology-structure)
2. [Individual File Contents](#individual-file-contents)
3.
...

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
