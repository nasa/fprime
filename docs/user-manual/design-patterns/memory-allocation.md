# Memory Allocation

This document treats the design pattern of how to allocate blocks of memory for a component in F´.

The memory ownership is **not to be shared across components**. To learn more about dynamic memory management and sharing in F´, please refer to the [Dynamic Memory Management](../framework/dynamic-memory.md) document.

## Fw::MemAllocator Overview

The [`Fw::MemAllocator` interface](../../../Fw/Types/MemAllocator.hpp) provides an abstraction for memory allocation to be used during system initialization. Components that require memory to be allocated at startup can use this interface to request memory blocks with sizes specified at runtime.

### Why not use stack memory?

Components may need memory internally to function. In some cases, the size of the memory required may not be known at compile time, which is a requirement for stack memory. Using `Fw::MemAllocator` allows components to request memory blocks of sizes determined at runtime, accommodating varying memory needs without the constraints of fixed stack sizes.

A good example is the [`Svc::BufferManager`](../../../Svc/BufferManager/docs/sdd.md) component, which manages pools of buffers of varying sizes and therefore requires backing memory of varying size. The size and number of these buffers can vary widely based on the specific application requirements. There can also be the need to have multiple instances of the `Svc::BufferManager`, each instance with different size configurations. Using `Fw::MemAllocator` allows each instance of `Svc::BufferManager` to independently allocate the necessary memory during system initialization.

> [!TIP]
> Many other components in F´ also use `Fw::MemAllocator`, you can search the codebase for `MemAllocator` to find more examples.

A commonly used implementation of `Fw::MemAllocator` is the [`Fw::MallocAllocator`](../../../Fw/Types/MallocAllocator.cpp) which delegates to the C/C++ `malloc()` and `free()` functions.

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
    void MyComponent::setup(Fw::MemAllocator& memAllocator, FwSizeType memorySize /* other params */) {
        this->m_memAllocator = &memAllocator;  // Store the allocator for later use
        this->m_memSize = memorySize;          // Store the requested memory size
        // Allocate memory using the provided allocator
        this->m_memPtr = allocator.allocate(this->getInstance(), this->m_memSize, false /*unused*/);
        if (buffer == nullptr) {
            // Handle allocation failure
        }
    }
    // [... component code can use the allocated memory after setup() has been called in Topology.cpp ...]
    ```

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
        myComponentInstanceOne.setup(mallocAllocator, 1024);
        myComponentInstanceTwo.setup(mallocAllocator, 2048);
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

Once the memory is allocated in the `setup()` method, the component can use the allocated memory as needed until shutdown. The memory will be properly deallocated in the `cleanup()` method when the component is shut down. To see examples of this pattern in practice, refer to components like [`Svc::BufferManager`](../../../Svc/BufferManager/BufferManagerComponentImpl.cpp), [`Svc::FrameAccumulator`](../../../Svc/FrameAccumulator/FrameAccumulator.cpp), and others, in the F´ codebase.
