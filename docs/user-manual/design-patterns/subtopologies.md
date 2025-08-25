# Subtopologies

A subtopology in FPP is a grouping of component instances and connections between them. It can be imported into a larger topology, allowing for modular, reusable design, and easier management of complex systems. Other modeling languages may have similar constructs to encapsulate subsystems within a reusable piece, such as a composite block in UML/SysML.

## Overview of a simple example 

Let's look at the [ManagerWorker example](https://github.com/nasa/fprime-examples/tree/devel/FlightExamples/ManagerWorker):

```python
module ManagerWorker {

    # Defining the instances to be used in subtopology
    instance manager: ManagerWorker.Manager base id ManagerWorkerSubtopologyConfig.ManagerWorkerSubtopology_BASE_ID + 0x0000 \
        queue size ManagerWorkerSubtopologyConfig.Defaults.QUEUE_SIZE \
        stack size ManagerWorkerSubtopologyConfig.Defaults.STACK_SIZE \
        priority ManagerWorkerSubtopologyConfig.Priorities.manager

    instance worker: ManagerWorker.Worker base id ManagerWorkerSubtopologyConfig.ManagerWorkerSubtopology_BASE_ID + 0x1000 \
        queue size ManagerWorkerSubtopologyConfig.Defaults.QUEUE_SIZE \
        stack size ManagerWorkerSubtopologyConfig.Defaults.STACK_SIZE \
        priority ManagerWorkerSubtopologyConfig.Priorities.worker

    @ Subtopology for connecting manager/worker
    topology Subtopology {
        # Instantiation in subtopology
        instance manager
        instance worker

        connections ManagerWorker {
            manager.startWorker -> worker.startWork
            manager.cancelWorker -> worker.cancelWork
            worker.workDone -> manager.doneRecv
        }
    } # end Subtopology
} # end ManagerWorker
```

This simple example defines a subtopology for a pair of component instances, a manager and a worker component, as well as the connections between them. 
This allows users to use this pair of components that were designed to work together, without having to manually re-specify their connections every time.

In a larger topology, we can import this subtopology which will automatically include all of its instances and connections.

```python
topology ExamplesDeployment {
    import ManagerWorker.Subtopology
    [...]
}
```

Inside the `ExamplesDeployment` topology, we refer to the imported components by using their qualified names:

```python
topology ExamplesDeployment {
    import ManagerWorker.Subtopology

    instance otherComponent

    connections Other {
        otherComponent.cancelAll -> ManagerWorker.worker.cancelWork
    }
}
```

## Subtopology configuration

### Configuration Overview

Subtopologies _can_ be designed with configurability in mind. This means that parameters can be defined and used at the subtopology definition level and then overridden when the subtopology is instantiated by a user in their own project. This is enabled by the use of F´ config modules (see [`register_fprime_config` API](https://fprime.jpl.nasa.gov/latest/docs/reference/api/cmake/API/#function-register_fprime_config) in the build system), which allow subtopology developers to define default configuration files, and projects to optionally override those files at build time.

Let's look at the [Svc.CdhCore](../../../Svc/Subtopologies/CdhCore) subtopology, and specifically the `CdhCoreConfig` directory.

> [!NOTE]
> CdhCore stands for core Command and Data Handling.

This subtopology provides a set of components and connections for handling core command and data processing tasks of an F´ application. This includes components such as a [CommandDispatcher](../../../Svc/CmdDispatcher/docs/sdd.md) and [EventManager](../../../Svc/EventManager/docs/sdd.md). Taking a closer look at the topology definition, we can notice a few interesting aspects:

### Configurable aspects

- **Configurable parameters**: The `CdhCoreConfig/CdhCoreConfig.fpp` module contains configuration values for the subtopology, such as queue sizes and `BASE_ID`. These parameters are used by the subtopology and can be easily overridden when the subtopology is instantiated in a larger topology, which will impact how the components are instantiated.
```python
instance events: Svc.EventManager base id CdhCoreConfig.BASE_ID + 0x001000 # notice the use of BASE_ID
```
- **Component instances**: Components themselves can be part of the configuration module. The CdhCore subtopology uses a `tlmSend` instance, and that instance is defined in the `CdhCoreConfig/CdhCoreTlmConfig.fpp`. Again, users can override the actual instance definition by overriding the configuration file in their project (see next section).
- **Configurable C++**: C++ source and header files can also leverage the overriding mechanism. This can allow users to customize the construction and setup of components in their topology. This is for example done in the [`Svc.ComFprime`](../../../Svc/Subtopologies/ComFprime/) subtopology to choose a type of memory allocation to be used.

### How-to Configure a Subtopology

To override the configuration of a subtopology, you need to create a configuration module in your project overriding the configuration files of the topology. The steps are detailed below.

>[!TIP]
> A reference implementation of the below steps can be found here: [ExampleCdhCoreConfig](https://github.com/nasa/fprime-examples/tree/0e66d4eb3acb7eccf54257847a9e5e1f18df9370/FlightExamples/ExamplesDeployment/ExampleCdhCoreConfig). This configures CdhCore to use TlmPacketizer instead of TlmChan.

#### Step 1: Create a Configuration Directory
Create a directory in your deployment to hold your custom configuration module:
```bash
mkdir MyDeployment/MyCdhCoreConfig
```

#### Step 2: Create Configuration Override File
Copy the configuration file with the **exact same name** as the original config file you want to override:
```bash
# File: MyDeployment/MyCdhCoreConfig/CdhCoreConfig.fpp
module CdhCoreConfig {
    constant BASE_ID = 0x01000000

    module QueueSizes {
        constant cmdDisp     = 10
        constant events      = 10
        constant tlmSend     = 10
    }

    ...
}
```

#### Step 3: Register the Configuration Module
In your config module `CMakeLists.txt`, register a config module with `register_fprime_config()` as shown below. The config name, passed as first argument, must be unique and different from `config`.
```cmake
# File: MyDeployment/MyCdhCoreConfig/CMakeLists.txt
register_fprime_config(
    "MyCdhCoreConfig"
    CONFIGURATION_OVERRIDES 
        "${CMAKE_CURRENT_LIST_DIR}/CdhCoreConfig.fpp"
    EXCLUDE_FROM_ALL
    INTERFACE
)
```

#### Step 4: Add as Dependency
Include the config module as a dependency of your deployment's topology module registration:
```cmake
# File: MyDeployment/Top/CMakeLists.txt
register_fprime_module(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/instances.fpp"
        "${CMAKE_CURRENT_LIST_DIR}/topology.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/ExampleTopology.cpp"
    DEPENDS
        "MyCdhCoreConfig"
)
```

## F Prime core subtopologies

F Prime provides several configurable core subtopologies that can be reused across different applications. Some of those are by default used in the deployments generated by `fprime-util new --deployment`. These include:

- [Svc.CdhCore](../../../Svc/Subtopologies/CdhCore): Core command and data handling components
- [Svc.ComCcsds](../../../Svc/Subtopologies/ComCcsds): Communications stack using the CCSDS protocols
- [Svc.ComFprime](../../../Svc/Subtopologies/ComFprime): Communications stack using the lightweight F´ protocol
- [Svc.DataProducts](../../../Svc/Subtopologies/DataProducts): Data products handling components
- [Svc.FileHandling](../../../Svc/Subtopologies/FileHandling): File handling components
