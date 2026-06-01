---
name: fprime-component-development
description: Top-level flight development process for F Prime components. Orchestrates the full lifecycle from requirements through integration testing. Trigger on any task involving creating a new F Prime component or adding significant functionality to an existing one. Keywords: F Prime, component, development, lifecycle, flight software, requirements, design, implementation, unit test, integration test.
---

# Skill: F Prime Component Development Process

The flight development process for F Prime components follows a strict
sequential lifecycle. Each phase produces artifacts that feed the next.
**Never skip a phase or guess at requirements/design decisions — always
ask the user.**

For the canonical process description, see the
[F´ Development Process](https://github.com/nasa/fprime/blob/devel/docs/user-manual/overview/development-practice.md).

---

## Process Overview

```
1. Requirements  →  2. Design (FPP)  →  3. Implementation (C++)  →  4. Unit Test  →  5. Integration Test
```

Each phase has a dedicated skill with detailed guidance:

| Phase | Skill | Key Output |
|---|---|---|
| 1. Requirements | `fprime-component-requirements.skill.md` | Written requirements list |
| 2. Design | `fprime-component-design-fpp.skill.md` | `.fpp` model file |
| 3. Implementation | `fprime-component-implementation.skill.md` | `.cpp` / `.hpp` files |
| 4. Unit Test | `fprime-component-unit-test.skill.md` | `test/ut/` test suite |
| 5. Integration Test | `fprime-component-integration-test.skill.md` | `test/int/` pytest suite |

---

## Cardinal Rules

1. **Ask, don't guess.** If you are unsure about a requirement,
   interface, behavior, naming convention, or deployment context —
   **stop and ask the user**. Wrong assumptions in flight software are
   costly.

2. **Each phase gates the next.** Do not begin implementation before
   the FPP model is confirmed. However, you can write tests against a
   known/confirmed model following the test-driven development
   pattern (see `docs/how-to/test-driven-development.md`).

3. **Reference the C++ design skill.** All implementation must comply
   with `fprime-cpp-design.skill.md` (CPP-1 through CPP-34). Consult
   it before writing any C++ code.

4. **Follow Test-Driven Development when possible.** The recommended
   F Prime workflow is: design in FPP → write tests → implement to
   pass tests. See the
   [TDD guide](https://github.com/nasa/fprime/blob/devel/docs/how-to/test-driven-development.md).

5. **Document as you go.** Each component should have an SDD
   (`docs/sdd.md`) that records requirements coverage, port
   descriptions, and design rationale.

---

## When to Use This Skill

- Creating a new component from scratch
- Adding significant new functionality to an existing component
  (new ports, commands, telemetry, state machines)
- Migrating or refactoring a component where the interface changes

For small bug fixes or implementation-only changes that don't alter
the component's interface, you may skip directly to the
implementation and unit test phases — but confirm with the user first.

---

## Quick Reference: Key Commands

| Action | Command |
|---|---|
| Create new component | `fprime-util new --component` |
| Create new port | `fprime-util new --port` |
| Generate impl stubs | `fprime-util impl` |
| Generate UT stubs | `fprime-util impl --ut` |
| Build component | `fprime-util build` |
| Run unit tests | `fprime-util check` |
| Run with coverage | `fprime-util check --coverage` |
| Generate UT build | `fprime-util generate --ut` |
| Scaffold rule-based tests | `fprime-util new --rule-based-test` |
