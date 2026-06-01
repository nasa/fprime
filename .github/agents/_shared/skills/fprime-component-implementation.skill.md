---
name: fprime-component-implementation
description: C++ implementation phase of F Prime component development. Guides the agent through implementing handler functions generated from the FPP model. Must follow fprime-cpp-design.skill.md rules (CPP-1 through CPP-34). Trigger when the FPP model is confirmed and the next step is to write the C++ implementation. Keywords: F Prime, C++, implementation, handler, component, flight software, autocoded.
---

# Skill: F Prime Component Implementation (C++)

Implementation fills in the **handler stubs** generated from the FPP
model. The autocoder produces base classes with pure-virtual handlers;
you implement the derived class.

---

## STOP — Prerequisites

Before implementing:

1. **FPP model must be confirmed** by the user (see
   `fprime-component-design-fpp.skill.md`).
2. **C++ design rules** (`fprime-cpp-design.skill.md`) are mandatory.
   Review before writing any code.
3. **Ask the user** if any implementation detail is ambiguous:
   - Algorithm specifics (control law, filtering, state transitions)
   - Interaction ordering (what happens first when a command arrives?)
   - Error-handling strategy (emit event? return error? assert?)
   - Thread safety considerations for shared state
   - Any hardware or OS dependencies

---

## Step-by-Step Process

### Step 1 — Generate Implementation Stubs

```bash
fprime-util impl
```

This produces `<Component>-template.cpp` and `<Component>-template.hpp`
files. If this is the first time:

```bash
mv <Component>-template.cpp <Component>.cpp
mv <Component>-template.hpp <Component>.hpp
```

If iterating on an existing design, copy new handler stubs from the
template into your existing files.

### Step 2 — Implement the Header File

The `.hpp` file declares:

- Private member variables (state)
- Any private helper methods
- Constructor and destructor

**Rules:**

- Initialize all member variables (CPP-19)
- Use fixed-size types: `U32`, `FwSizeType`, etc. (CPP-3, CPP-28)
- No dynamic memory — size all arrays at compile time (CPP-1)
- No STL containers (`std::vector`, `std::map`, etc.) — use
  `Fw/DataStructures` or fixed arrays (CPP-22, CPP-25)
- Mark copy/move as deleted for components (CPP-17)

```cpp
class MyComponent final : public MyComponentComponentBase {
  public:
    MyComponent(const char* const compName);
    ~MyComponent();

    // Delete copy/move
    MyComponent(const MyComponent&) = delete;
    MyComponent& operator=(const MyComponent&) = delete;

  private:
    // --- Handlers (from FPP) ---
    void myPort_handler(FwIndexType portNum, ...) override;
    void MY_COMMAND_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, ...) override;

    // --- State ---
    U32 m_count{0};
};
```

### Step 3 — Implement Port Handlers

Each input port in the FPP model produces a pure-virtual handler:

```cpp
void MyComponent::myPort_handler(FwIndexType portNum, <args>) {
    // Implementation here
}
```

**Ask the user** if the behavior for any handler is not obvious from
requirements.

### Step 4 — Implement Command Handlers

Each command produces a handler that **must** call `cmdResponse_out`:

```cpp
void MyComponent::MY_COMMAND_cmdHandler(
    FwOpcodeType opCode,
    U32 cmdSeq,
    <args>
) {
    // Validate inputs (DO NOT FW_ASSERT on command args — CPP-4)
    if (!isValid(arg1)) {
        this->log_WARNING_HI_InvalidArg(arg1);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    // Perform action
    ...

    // Report success
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}
```

**Critical**: Never `FW_ASSERT` on ground-reachable inputs (CPP-4).
Validate and return an error response instead.

### Step 5 — Emit Telemetry and Events

Use the autocoded helper functions:

```cpp
// Telemetry
this->tlmWrite_ChannelName(value);

// Events
this->log_ACTIVITY_HI_SomethingHappened(arg1, arg2);
```

### Step 6 — Implement Initialization (if needed)

Override `init()` or add a custom setup method for one-time init:

```cpp
void MyComponent::init(FwEnumStoreType queueDepth, FwEnumStoreType instance) {
    MyComponentComponentBase::init(queueDepth, instance);
    // One-time setup here (CPP-1: allocations are allowed here)
}
```

### Step 7 — Build and Fix Errors

```bash
fprime-util build
```

Iterate until compilation succeeds. Common issues:
- Missing `#include` for types used in handlers
- Incorrect argument types (check the generated base class)
- Missing `cmdResponse_out` call in command handlers

### Step 8 — Review Against C++ Design Rules

Before considering implementation complete, verify compliance with:

- CPP-1: No dynamic memory after init
- CPP-3/28: Fixed-size and `Fw*` types only
- CPP-4: No `FW_ASSERT` on untrusted inputs
- CPP-19: All variables initialized
- CPP-24: `Fw::String` over `char*`
- CPP-5: Compiles as C++14

---

## Autocoded Functions Reference

The base class provides these protected methods:

| Category | Naming Pattern | Example |
|---|---|---|
| Output port invoke | `<portName>_out(portNum, args...)` | `dataOut_out(0, buffer)` |
| Telemetry write | `tlmWrite_<ChannelName>(value)` | `tlmWrite_Count(m_count)` |
| Event emit | `log_<SEVERITY>_<EventName>(args...)` | `log_WARNING_HI_Overflow(size)` |
| Command response | `cmdResponse_out(opCode, cmdSeq, response)` | — |
| Parameter get | `paramGet_<ParamName>()` | `paramGet_Threshold()` |
| Port connected check | `isConnected_<portName>_OutputPort(portNum)` | — |

---

## Anti-Patterns

- ❌ Guessing at algorithm behavior — ask the user
- ❌ Using `FW_ASSERT` on command arguments or hardware inputs
- ❌ Forgetting `cmdResponse_out` (command will hang in dispatcher)
- ❌ Using `new`/`delete` in handler code
- ❌ Using `std::string`, `std::vector`, or other STL containers
- ❌ Leaving member variables uninitialized
- ❌ Implementing behavior not covered by a requirement
