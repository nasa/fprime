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
This allows users to use this pair of components that were designed to work together, without having to manually re-specifiy their connections every time.

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

### Overview

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

### How-to Configure

To override the configuration of a subtopology, you need to create a configuration module in your project overriding the configuration files of the topology. For example, to override files in the `CdhCoreConfig` module, you would create a file named `CdhCoreConfig.fpp` in a directory and register it as a config module. The steps are detailed below:

1. Create a directory in your project and name it appropriately (e.g. `MyCdhCoreConfig`). This would usually be inside your deployment directory.
2. Create a file named `CdhCoreConfig.fpp` in the new directory. The filename **must** match the name of the original config file you want to override.
3. In the `CdhCoreConfig.fpp` file, define the new configuration values you want to use. It is recommended to start from a copy of the original file and modify the values as you see fit.
4. Register the new config module in your project's CMakeLists.txt file using the `register_fprime_config` API.
5. Specify the newly registered module as a dependency of your deployment.

## F Prime core subtopologies

F Prime provides several configurable core subtopologies that can be reused across different applications. They are by default used in the deployments generated by `fprime-util new --deployment`. These include:

- [Svc.CdhCore](../../../Svc/Subtopologies/CdhCore): Core command and data handling components
- [Svc.ComCcsds](../../../Svc/Subtopologies/ComCcsds): Communications stack using the CCSDS protocols
- [Svc.ComFprime](../../../Svc/Subtopologies/ComFprime): Communications stack using the lightweight F´ protocol
- [Svc.DataProducts](../../../Svc/Subtopologies/DataProducts): Data products handling components
- [Svc.FileHandling](../../../Svc/Subtopologies/FileHandling): File handling components
