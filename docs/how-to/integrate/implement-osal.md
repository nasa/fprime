# Implement an OS Abstraction Layer

This guide provides step-by-step instructions for implementing a new OS Abstraction Layer (OSAL) for F´. The F´ OSAL provides a uniform interface to operating system services, allowing F´ to run on multiple operating systems without modification to the source code. For more information on the architecture and design of the OSAL, refer to the [OSAL Software Design Document](../../../Os/docs/sdd.md).

---

## Prerequisites

Before starting, you should have:

* An understanding of the [OSAL architecture](../../../Os/docs/sdd.md#5-implementation-architecture), specifically the [delegate pattern](../../../Os/docs/sdd.md#51-delegate-pattern).
* A working F´ build environment for testing.
* Familiarity with [F´ libraries](../develop/develop-fprime-libraries.md) and [F´ platform files](../develop/develop-fprime-libraries.md#optional-platform-folder-and-platform-files).
* Knowledge of your target OS APIs (e.g., mutex, task creation, file I/O).

---

## Overview

The OSAL uses a delegate pattern to decouple F´ application code from platform-specific OS calls. Each OSAL _service_ (Mutex, File, Task, etc.) has three layers: an **interface class** defining the contract, a **wrapper class** (e.g., `Os::Mutex`) that application code uses, and a **platform-specific implementation** that the wrapper delegates to. The build system selects the implementation at link time. Before proceeding, read the [OSAL Software Design Document](../../../Os/docs/sdd.md) — in particular the [delegate pattern](../../../Os/docs/sdd.md#51-delegate-pattern) and [implementation architecture](../../../Os/docs/sdd.md#5-implementation-architecture) sections — as this guide assumes familiarity with those concepts.

This guide walks through implementing an OSAL for a hypothetical OS called **"MyOs"**, and walks through the implementation of the `Os::Mutex` service as the example. The same process applies for all other OSAL services. Reference implementations are linked in the [Additional Resources](#additional-resources) section below.

---

## Step 1 — Set Up the Library

An OSAL implementation is packaged as an [F´ library](../develop/develop-fprime-libraries.md). Create the following directory structure:

```
fprime-my-os/
├── FprimeMyOs/
│   └── Os/
│       ├── CMakeLists.txt
│       ├── Mutex.hpp
│       ├── Mutex.cpp
│       └── DefaultMutex.cpp
└── library.cmake
```

The `library.cmake` file at the root adds the OSAL module directory:

```cmake
# library.cmake
add_fprime_subdirectory("${CMAKE_CURRENT_LIST_DIR}/FprimeMyOs/Os")
```

> [!IMPORTANT]
> All module directories for libraries are namespaced under `FprimeMyOs/` to avoid collisions with core F´ modules and other libraries.

---

## Step 2 — Implement an OSAL Module

Each OSAL service is implemented through a CMake/C++ module that requires an **implementation class** that inherits from the corresponding interface. The interface header files in `fprime/Os/` define the contract (pure virtual methods) that must be implemented. 

To keep track of state through the OS's native constructs (e.g. a mutex handle, such as `pthread_mutex_t` in Posix), the implementation class contains a **m_handle** member that wraps the native OS primitives.

### 2.1 — Define the Handle and Implementation Class

Create `FprimeMyOs/Os/Mutex.hpp`:

```c++
// FprimeMyOs/Os/Mutex.hpp
#ifndef MYOS_MUTEX_HPP
#define MYOS_MUTEX_HPP

#include <Os/Mutex.hpp>
#include <myos/mutex.h>  // MyOs native mutex API

namespace Os {
namespace MyOs {
namespace Mutex {

// Handle wraps the native OS mutex primitive
struct MyOsMutexHandle : public MutexHandle {
    myos_mutex_t m_mutex;  // MyOs native mutex type
};

// Implementation of MutexInterface using MyOs APIs
class MyOsMutex : public MutexInterface {
  public:
    MyOsMutex();
    ~MyOsMutex() override;

    MutexHandle* getHandle() override;
    Status take() override;
    Status release() override;

  private:
    MyOsMutexHandle m_handle;  // Handle wrapping the native OS mutex
};

}  // namespace Mutex
}  // namespace MyOs
}  // namespace Os
#endif
```

> [!TIP]
> Look at each interface header in `Os/` (e.g., `Os/Mutex.hpp`, `Os/File.hpp`, `Os/Task.hpp`) to see the exact set of pure virtual methods that need to be implemented for each. Each interface also defines a `Status` enum — your implementation must return the appropriate [status values](../../../Os/docs/sdd.md#24-error-handling).

### 2.2 — Implement the Methods

Create `FprimeMyOs/Os/Mutex.cpp`:

```c++
// FprimeMyOs/Os/Mutex.cpp
#include <FprimeMyOs/Os/Mutex.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {
namespace MyOs {
namespace Mutex {

MyOsMutex::MyOsMutex() : Os::MutexInterface(), m_handle() {
    int status = myos_mutex_init(&this->m_handle.m_mutex);
    FW_ASSERT(status == 0, static_cast<FwAssertArgType>(status));
}

MyOsMutex::~MyOsMutex() {
    myos_mutex_destroy(&this->m_handle.m_mutex);
}

MyOsMutex::Status MyOsMutex::take() {
    int status = myos_mutex_lock(&this->m_handle.m_mutex);
    switch (status) {
        case 0:
            return Status::OP_OK;
        case MYOS_EBUSY:
            return Status::ERROR_BUSY;
        case MYOS_EDEADLK:
            return Status::ERROR_DEADLOCK;
        default:
            return Status::ERROR_OTHER;
    }
}

MyOsMutex::Status MyOsMutex::release() {
    int status = myos_mutex_unlock(&this->m_handle.m_mutex);
    // TODO: Map MyOs error codes to appropriate Status values
    return (status == 0) ? Status::OP_OK : Status::ERROR_OTHER;
}

MutexHandle* MyOsMutex::getHandle() {
    return &this->m_handle;
}

}  // namespace Mutex
}  // namespace MyOs
}  // namespace Os
```

> [!CAUTION]
> When an `Os::Mutex` wrapper object is created, the underlying implementation object `MyOsMutex` is constructed in-place using placement new into a fixed-size byte array of size `FW_MUTEX_HANDLE_MAX_SIZE`. This constant must be defined in `PlatformCfg.fpp` in the library configuration. The compiler will help identify the required size based on the `sizeof(MyOsMutex)`.

---

## Step 2.5 — Optional: Override for Compile-Time Selection (Performance Optimization)

By default, F´ uses **link-time selection** with runtime virtual dispatch through the delegate pattern (see [OSAL SDD §5.2](../../../Os/docs/sdd.md#52-selection-mechanisms-link-time-vs-compile-time)). For performance-critical platforms (e.g., bare-metal embedded systems with tight timing constraints), projects can override the default configuration to enable compile-time selection that eliminates virtual dispatch overhead and enables aggressive compiler optimizations including function inlining and link-time optimization (LTO).

> [!NOTE]
> The F´ OSAL provides default configuration headers (e.g., `default/config/OsDelegateRawTime.hpp`) that enable aliasing and default to link-time selection. These are part of F´'s core configuration. This step describes how **projects** can override these defaults for their specific deployments.

### Why Compile-Time Selection?

See [OSAL SDD §5.2.2 "Compile-Time Selection"](../../../Os/docs/sdd.md#522-compile-time-selection-performance-optimization) for a detailed explanation of the trade-offs between link-time and compile-time selection. In summary:

- **Link-time selection (default)** uses virtual dispatch (~10-50 cycles overhead) but provides runtime flexibility
- **Compile-time selection** enables inlining and LTO, eliminating call overhead for performance-critical code

This optimization is most valuable for bare-metal systems with high-frequency operations like telemetry timestamping.

### How Projects Override for Compile-Time Selection

Projects override compile-time selection by providing their own configuration header that replaces F´'s default. For `RawTime`, create a project-specific override of `config/OsDelegateRawTime.hpp`:

**Step 1:** In your project's config directory, create `config/OsDelegateRawTime.hpp`:

```c++
// my-project/config/OsDelegateRawTime.hpp
#ifndef CONFIG_OS_DELEGATERAWTIME_HPP
#define CONFIG_OS_DELEGATERAWTIME_HPP

// Guard against circular dependencies (same as F´ default)
#if defined(OS_RAWTIME_HPP_) || defined(OS_RAWTIMEINTERFACE_HPP_) || defined(OS_DELEGATERAWTIME_HPP_)
#error "config/OsDelegateRawTime.hpp must not include Os OSAL headers - circular dependency detected"
#endif

// Forward-declare your concrete implementation class
namespace MyPlatform {
    class MyRawTime;
}

namespace Os {
    // Alias Os::RawTime directly to your implementation (no delegate wrapper)
    using RawTime = MyPlatform::MyRawTime;
}

// Point to your implementation header (will be included after RawTimeInterface.hpp)
#define OS_RAW_TIME_HEADER "MyPlatform/Os/RawTime.hpp"

#endif  // CONFIG_OS_DELEGATERAWTIME_HPP
```

**Step 2:** Register the config header in your project's `config/CMakeLists.txt`:

```cmake
register_fprime_config(
    # ... project config name & other options 
    CONFIGURATION_OVERRIDES
        "${CMAKE_CURRENT_LIST_DIR}/OsDelegateRawTime.hpp"
        # ... other project override config headers
)
```

**Step 3:** Ensure your implementation class inherits from `RawTimeInterface`:

```c++
// MyPlatform/Os/RawTime.hpp
#include <Os/RawTimeInterface.hpp>

namespace MyPlatform {

class MyRawTime : public Os::RawTimeInterface {
    // Implementation of RawTimeInterface methods
    // Now called directly without delegation wrapper
    // ...
};

}  // namespace MyPlatform
```

### Important Constraints

See [OSAL SDD §5.2.2](../../../Os/docs/sdd.md#522-compile-time-selection-performance-optimization) for the complete list of constraints. Key points:

- **ABI Compatibility**: All files must use the same configuration
- **No Circular Dependencies**: Config header must not include Os OSAL headers
- **Interface Inheritance Required**: Implementation must inherit from interface
- **Skip the Delegate Factory**: No `Default*.cpp` needed for compile-time selection

### Which Services Support Compile-Time Selection?

See [OSAL SDD §5.2.2](../../../Os/docs/sdd.md#522-compile-time-selection-performance-optimization) for OS services which support aliasing. 

---

## Step 3 — Register the Delegate Factory

> [!NOTE]
> If you enabled compile-time selection in Step 2.5, you can skip creating `DefaultMutex.cpp` (or `DefaultRawTime.cpp` etc.) since the delegate factory is not used. Jump directly to Step 4.

Create `FprimeMyOs/Os/DefaultMutex.cpp`. This file provides the `getDelegate()` factory function that the `Os::Mutex` wrapper calls to construct the implementation:

```c++
// FprimeMyOs/Os/DefaultMutex.cpp
#include <Os/Delegate.hpp>
#include <FprimeMyOs/Os/Mutex.hpp>

namespace Os {

MutexInterface* MutexInterface::getDelegate(MutexHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<MutexInterface, Os::MyOs::Mutex::MyOsMutex>(
        aligned_new_memory);
}

}  // namespace Os
```

`Os::Delegate::makeDelegate` is a helper that performs the placement-new with compile-time checks that verify:

- `MyOsMutex` derives from `MutexInterface`
- `sizeof(MyOsMutex)` fits within `MutexHandleStorage`
- `alignof(MyOsMutex)` is compatible with `FW_HANDLE_ALIGNMENT`

> [!NOTE]
> Some interfaces require additional parameters in their `getDelegate` signature:
> - `FileInterface` takes an optional `const FileInterface* to_copy` parameter for copy construction
> - `RawTimeInterface` takes optional `const RawTimeInterface* to_copy` and `RawTimeSource source` parameters to support both copy construction and selectable timer sources
> 
> The `makeDelegate` helper supports all these signatures. Check each interface header for the exact `getDelegate` signature required.

---

## Step 4 — Register the Module in CMake

Create `FprimeMyOs/Os/CMakeLists.txt` and use the `register_os_implementation` function to register the implementation:

```cmake
# FprimeMyOs/Os/CMakeLists.txt
restrict_platforms(MyOs)

register_os_implementation("Mutex" MyOs) # args: "ServiceName" "PlatformSuffix"
```

This single `register_os_implementation` call:

1. Expects the files `Mutex.hpp`, `Mutex.cpp`, and `DefaultMutex.cpp` in the current directory.
2. Creates an `Os_Mutex_MyOs_Implementation` target with the implementation sources (`Mutex.cpp`, `Mutex.hpp`).
3. Creates an `Os_Mutex_MyOs` target with `DefaultMutex.cpp` (the delegate factory), registered as an implementation of `Os_Mutex`.

If the module depends on other modules, pass them as additional arguments:

```cmake
register_os_implementation("Task" MyOs MyOs_Shared Fw_Time)
```

If multiple interfaces share a `Default*.cpp`, list them together:

```cmake
register_os_implementation("Mutex;ConditionVariable" MyOs MyOs_Shared)
register_os_implementation("File;FileSystem;Directory" MyOs MyOs_Shared)
```

The `restrict_platforms(MyOs)` call ensures these implementations only build when the `MyOs` platform is selected.

---

## Step 5 - Unit Tests

Thorough Unit Testing is outside the scope of this How-To guide, but is critical to ensure the correctness of your implementation. F´ provides OS-agnostic unit tests for most OSAL service under `fprime/Os/test/`. These tests can be run against your implementation to validate that it behaves correctly according to the interface contract. You may also add additional tests if desired.

---

## Step 6 — Repeat for All desired OSAL Services

The process described above for `Os::Mutex` is the same for every OSAL module. For each module:

1. Create `<Module>.hpp` with a handle struct and implementation class inheriting from the interface.
2. Create `<Module>.cpp` implementing the virtual methods using your OS's native APIs.
3. Create `Default<Module>.cpp` implementing the `getDelegate()` factory.
4. Register with `register_os_implementation` in `CMakeLists.txt`.

> [!NOTE]
> F´ provides two platform-independent queue implementations: `Os_Generic_PriorityQueue` (mutex-based — the default on most platforms) and `Os_Generic_LocklessPriorityQueue` (lockless, ISR-safe — opt-in for targets that send or receive from interrupt context). You do not need to write an OS-specific queue unless neither generic implementation is suitable for your target.

The full set of [OSAL modules](../../../Os/docs/sdd.md#2-core-services) that can be implemented is:

| Module | Interface | Key Methods |
|---|---|---|
| **Mutex** | `MutexInterface` | `take()`, `release()` |
| **CountingSemaphore** | `CountingSemaphoreInterface` | `wait()`, `waitTimeout()`, `tryWait()`, `post()` |
| **ConditionVariable** | `ConditionVariableInterface` | `pend()`, `notify()`, `notifyAll()` |
| **Task** | `TaskInterface` | `start()`, `join()`, `delay()` |
| **Queue** | `QueueInterface` | `send()`, `receive()` |
| **File** | `FileInterface` | `open()`, `read()`, `write()`, `close()`, `seek()` |
| **FileSystem** | `FileSystemInterface` | `rename()`, `remove()`, `stat()` |
| **Directory** | `DirectoryInterface` | `open()`, `read()`, `close()` |
| **Console** | `ConsoleInterface` | `write()` |
| **RawTime** | `RawTimeInterface` | `now()`, `getTimeInterval()` (getDelegate takes `RawTimeSource`) |
| **Cpu** | `CpuInterface` | `getCount()`, `getTicks()` |
| **Memory** | `MemoryInterface` | `getUsage()` |

Refer to each interface header in `Os/` for the full method signatures and status enums.

---

## Step 7 — Use the Implementation on a Platform

Some guidance is available in the [Porting Guide](porting-guide.md), and a full How-To will soon follow on how to port F Prime to a new platform.

The bottom line is that to use your new OSAL implementation, you need to add the `Os_<Module>_<Suffix>` target to the `CHOOSES_IMPLEMENTATIONS` list in your platform CMake files. Examples are available:

- [F Prime Zephyr Support Package](https://github.com/fprime-community/fprime-zephyr/tree/main/cmake/platform)
- [F Prime VxWorks Support Package](https://github.com/fprime-community/fprime-vxworks/tree/devel/cmake/platform)

---

## Best Practices

- **Start with Stubs.** Only implement the modules you need. The [Stub backend](../../../Os/Stub) returns `NOT_SUPPORTED` for unimplemented services, allowing you to bring up an OSAL incrementally.
- **Map error codes consistently.** Create a shared error-translation module (like `Os/Posix/error.hpp`) that converts your OS's native error codes to the F´ Os `Status` enums. This keeps error handling centralized and testable.
- **Watch the handle size.** Each implementation object must fit in the fixed-size handle storage. If your native OS primitives are large, override `FW_MUTEX_HANDLE_MAX_SIZE` (or equivalent) in your platform configuration.
- **Use other implementations as references.** The `Os/Posix/` directory is the most complete OSAL implementation in F´ and serves as the canonical example for all modules. Other implementations exist (see links above, or in the resources below)
- **Test with the F´ OSAL unit tests.** The framework provides OS-agnostic unit tests under `Os/test/` that exercise the OSAL interfaces. Run these tests against your implementation to validate correctness.

---

## Additional Resources

- [OSAL Software Design Document](../../../Os/docs/sdd.md) — Architecture details and design rationale.
- [Develop an F´ Library](../develop/develop-fprime-libraries.md) — Library structure, toolchain, and platform files.
- [Porting Guide](porting-guide.md) — High-level checklist for porting F´ to a new platform.
- [Posix OSAL implementation](https://github.com/nasa/fprime/tree/devel/Os/Posix) — Reference implementation for POSIX systems.
- [fprime-zephyr](https://github.com/fprime-community/fprime-zephyr) — OSAL implementation for Zephyr RTOS.
- [fprime-vxworks](https://github.com/fprime-community/fprime-vxworks) - OSAL implementation for VxWorks RTOS.
- [fprime-freertos](https://github.com/fprime-community/fprime-freertos/tree/main/FreeRTOS/Os) - OSAL implementation for FreeRTOS.
