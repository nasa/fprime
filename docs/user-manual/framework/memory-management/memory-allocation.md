# Memory Allocation with Fw::MemAllocator

This document treats the design pattern of how to allocate blocks of memory in F´ using dynamic (heap) memory allocation. This is restricted to system initialization only, as dynamic memory allocation during runtime is forbidden by flight software coding standards for safety and reliability reasons.

To learn more about working with buffers beyond system initialization in F´, please refer to the [Buffer Pool Guide](./buffer-pool.md) document.

## Fw::MemAllocator Overview

The [`Fw::MemAllocator` interface](../../../../Fw/Types/MemAllocator.hpp) provides an abstraction for memory allocation to be used during system initialization. Components that require memory to be allocated at startup can use this interface to request memory blocks with sizes specified at runtime.

### When to use Fw::MemAllocator

Components may need memory internally to function. There are several reasons why dynamic allocation through `Fw::MemAllocator` is used instead of global, static, stack or component member variables:

1. **Size constraints**: Certain buffers are too large for stack allocation. Stack sizes are typically limited and large allocations can cause stack overflow.

2. **Runtime sizing**: The size of the memory required may not be known at compile time. This includes cases where multiple instances of a component may require different sizes of memory.

A good example is the [`Svc::BufferManager`](../../../Svc/BufferManager/docs/sdd.md) component, which manages pools of buffers of configurable sizes. These pools are often large and the number can vary widely based on project requirements. There can also be the need to have multiple instances of the `Svc::BufferManager`, each instance with different size configurations. Using `Fw::MemAllocator` allows each instance of `Svc::BufferManager` to independently allocate the necessary memory during system initialization.

> [!TIP]
> Many other components in F´ also use `Fw::MemAllocator`, you can search the codebase for `MemAllocator` to find more examples.

The core framework ships with an implementation of `Fw::MemAllocator`, called [`Fw::MallocAllocator`](../../../../Fw/Types/MallocAllocator.cpp), which delegates to the C/C++ `malloc()` and `free()` functions. Projects are free to implement their own versions of `Fw::MemAllocator` if desired.

>[!WARNING]
> Flight Software coding standards forbid dynamic memory allocation outside of system initialization. This is for safety and reliability reasons. Therefore, the use of `Fw::MemAllocator` is intended for use during initialization only, typically through a component `configure()`/`setup()` method called during `configureTopology()`. For runtime memory management during operation, please consult the [Buffer Pools with Svc.BufferManager](./buffer-pool.md) document.

## Pattern to use the Fw::MemAllocator

The following steps outline the design pattern for using `Fw::MemAllocator` in a component to allocate memory during system initialization:

1. **Define a Member Variable for the Allocator**: In your component class, define a member variable to hold a pointer to the `Fw::MemAllocator`. It will be used to deallocate memory during shutdown.

    ```cpp
    class MyComponent : public Fw::MyComponentComponentBase {

        // [... other component code ...]

    private:
        Fw::MemAllocator* m_memAllocator; // Stored for deallocation during shutdown

        U8* m_memPtr; // Pointer to allocated memory
        FwSizeType m_memSize; // Size of allocated memory
    };
    ```

2. **Specify (or Modify) the Setup Method**: Update or create a setup method for your component, usually called `configure()`/`setup()`. This method should accept a reference to an `Fw::MemAllocator`. Use this allocator to allocate the required memory blocks in the setup method.

    ```cpp
    void MyComponent::setup(Fw::MemAllocator& memAllocator, FwSizeType memorySize, FwEnumStoreType memId /* other params */) {
        this->m_memAllocator = &memAllocator;  // Store the allocator for later use
        this->m_memSize = memorySize;          // Store the requested memory size
        // Allocate memory using the provided arguments
        this->m_memPtr = this->m_memAllocator->allocate(memId, this->m_memSize);
        if (this->m_memPtr == nullptr) {
            // Handle allocation failure
        }
    }
    // [... component code can use the allocated memory after setup() has been called in Topology.cpp ...]
    ```

    The `memId` parameter is the memory segment identifier and should be unique per component. The usage of this identifier is specific to each implementation of `Fw::MemAllocator`. The `Fw::MallocAllocator` implementation does not use this value.

    > [!IMPORTANT]
    > The `Fw::MemAllocator` instance's lifespan must persist at least through the `cleanup()` method call (or until the component's destructor if `cleanup()` is not called), as it will be used for deallocation.

3. **Deallocate Memory During Shutdown**: In the component's cleanup method, ensure that you deallocate any memory that was allocated using the `Fw::MemAllocator`. Use the stored allocator pointer to free the memory.

    ```cpp
    void MyComponent::cleanup() {
        if (this->m_memPtr != nullptr && this->m_memAllocator != nullptr){
            this->m_memAllocator->deallocate(this->getInstance(), this->m_memPtr);
        }
        this->m_memPtr = nullptr;
        this->m_memSize = 0;
    }
    ```

4. **Update Topology Configuration and Teardown**: In your deployment's `Topology.cpp` (or phase code if you use phases), ensure to call `setup()` and `cleanup()`, choosing an appropriate `Fw::MemAllocator` implementation to pass in. For demonstration purposes, let's imagine that we need two instances of `MyComponent`, each requiring different memory sizes:

    ```cpp
    #include "Fw/MemAllocator/MemAllocator.hpp"
    static Fw::MallocAllocator mallocAllocator;  // Here we choose to use the MallocAllocator implementation

    void configureTopology() {
        // [... other component initializations ...]

        // Setup two MyComponent instances with a required memory size of 1024 and 2048 bytes respectively
        // Pass unique IDs for memory tracking
        myComponentInstanceOne.setup(mallocAllocator, 1024, 100);
        myComponentInstanceTwo.setup(mallocAllocator, 2048, 101);
    }

    void teardownTopology(const TopologyState& state) {\
        // Cleanup MyComponent to deallocate memory
        myComponentInstanceOne.cleanup();
        myComponentInstanceTwo.cleanup();

        // [... other component cleanups ...]
    }
    ```

    This assumes that your `instances.fpp` would have defined both instances like the following:
    
    ```python
    module MyDeployment {
        instance myComponentInstanceOne: MyNamespace.MyComponent
        instance myComponentInstanceTwo: MyNamespace.MyComponent
    }
    ```

Once the memory is allocated in the `setup()` method, the component can use the allocated memory as needed until shutdown. The memory will be properly deallocated in the `cleanup()` method when the component is shut down. To see examples of this pattern in practice, refer to components like [`Svc::BufferManager`](../../../../Svc/BufferManager/BufferManagerComponentImpl.cpp), [`Svc::FrameAccumulator`](../../../../Svc/FrameAccumulator/FrameAccumulator.cpp), and others, in the F´ codebase.

## Fw::MemAllocatorRegistry

The [`Fw::MemAllocatorRegistry`](../../../../Fw/Types/MemAllocator.hpp) is a singleton registry that allows registering different memory allocators for different purposes within the system. This is useful when projects want to manage multiple types of memory allocators in a centralized way. Projects may define project-specific allocators that implement the `Fw::MemAllocator` interface and register them with the registry.
### Basic Usage

```cpp
// In your topology initialization:
#include <Fw/Types/MemAllocator.hpp>

// Get the singleton registry
Fw::MemAllocatorRegistry& registry = Fw::MemAllocatorRegistry::getInstance();

// Instantiate and register custom allocators for different purposes
Fw::MallocAllocator systemAllocator;
MyCustomAllocator specialPurposeAllocator;

registry.registerAllocator(Fw::MemoryAllocation::MemoryAllocatorType::SYSTEM, systemAllocator);
registry.registerAllocator(Fw::MemoryAllocation::MemoryAllocatorType::CUSTOM_ALLOCATOR_1, specialPurposeAllocator);

// [... other code ...]

// Later, retrieve and use allocators
Fw::MemAllocator& allocator = registry.getAllocator(Fw::MemoryAllocation::MemoryAllocatorType::SYSTEM);
```

> [!NOTE]
> The registry maintains pointers to allocators; it does not take ownership. Registered allocators must remain valid for the duration of the program.

The types of allocators available are defined in [`config/MemoryAllocation.fpp`](../../../../default/config/MemoryAllocation.fpp) and can be customized per deployment.
