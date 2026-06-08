---
name: fprime-component-implementation
description: C++ implementation phase of F Prime component development. Guides the agent through implementing handler functions generated from the FPP model. Must follow fprime-cpp-design.skill.md rules (CPP-1 through CPP-34). Trigger when the FPP model is confirmed and the next step is to write the C++ implementation. Keywords: F Prime, C++, implementation, handler, component, flight software, autocoded.
---

# Skill: F Prime Component Implementation (C++)

Implementation fills in the **handler stubs** generated from the FPP
model. The autocoder produces base classes with pure-virtual handlers;
you implement the derived class.

For the full reference on autocoded functions (port handlers, command
handlers, telemetry, events, parameters, initialization), see
[Autocoded Functions and Component Classes](https://github.com/nasa/fprime/blob/devel/docs/user-manual/framework/autocoded-functions.md).

**Use F Prime design patterns** where possible — standard solutions
exist for common needs:

- Rate Group Pattern — `docs/user-manual/design-patterns/rate-group.md`
- Health Checking — `docs/user-manual/design-patterns/health-checking.md`
- Manager-Worker — `docs/user-manual/design-patterns/manager-worker.md`
- Application-Manager-Driver — `docs/user-manual/design-patterns/app-man-drv.md`
- Common Port Patterns — `docs/user-manual/design-patterns/common-port-patterns.md`

Follow [F Prime Style Guidelines](https://github.com/nasa/fprime/wiki/F%C2%B4-Style-Guidelines)
for naming and code style.

---

## Prerequisites

The FPP model must be confirmed (see
`fprime-component-design-fpp.skill.md`) and C++ design rules
(`fprime-cpp-design.skill.md`, CPP-1 through CPP-34) are mandatory.
The confirmed requirements and FPP model should provide all the
information needed for implementation.

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

### Step 2 — Implement Handlers

Implement all pure-virtual handlers generated from the FPP model.
See [Autocoded Functions](https://github.com/nasa/fprime/blob/devel/docs/user-manual/framework/autocoded-functions.md)
for handler naming conventions, argument types, and the full API
provided by the base class.

Key rules to follow during implementation:

- **No dynamic memory** after construction (CPP-1) — size all
  arrays at compile time
- **Fixed-size types**: `U32`, `FwSizeType`, etc. (CPP-3, CPP-28)
- **No STL containers** — use `Fw/DataStructures` or fixed arrays
  (CPP-22, CPP-25)
- **No `FW_ASSERT` on untrusted inputs** (CPP-4) — validate and
  return an error response instead
- **All variables initialized** (CPP-19)
- **`Fw::String` over `char*`** (CPP-24)
- **Every command handler must call `cmdResponse_out`** — omitting
  it hangs the command in the dispatcher
- **Mark copy/move as deleted** for components (CPP-17)

### Step 3 — Build and Fix Errors

```bash
fprime-util build
```

Iterate until compilation succeeds. Common issues:
- Missing `#include` for types used in handlers
- Incorrect argument types (check the generated base class)
- Missing `cmdResponse_out` call in command handlers

### Step 4 — Review Against C++ Design Rules

Before considering implementation complete, verify compliance with
`fprime-cpp-design.skill.md` (CPP-1 through CPP-34).

---

## Anti-Patterns

- Using `FW_ASSERT` on command arguments or hardware inputs
- Forgetting `cmdResponse_out` (command will hang in dispatcher)
- Using `new`/`delete` in handler code
- Using `std::string`, `std::vector`, or other STL containers
- Leaving member variables uninitialized
- Implementing behavior not covered by a requirement
