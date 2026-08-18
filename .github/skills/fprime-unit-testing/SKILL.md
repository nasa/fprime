---
name: fprime-unit-testing
description: >
  Write F Prime component unit tests. Covers scaffold generation via
  `fprime-util impl --ut`, the Tester / TestMain / GTestBase pattern,
  rules-based testing with STest, helper-function design, and the
  CMakeLists.txt registration. Use whenever creating or extending
  unit tests for an F Prime component.
---

# Skill: F Prime Unit Testing

Step-by-step procedure for writing high-quality unit tests for an
F Prime component. The full reference for scaffolding, assertion
macros, helper functions, TestMain structure, and CMakeLists
registration is in the
[unit testing guide](https://github.com/nasa/fprime/blob/devel/docs/user-manual/overview/unit-testing.md).
Follow the guide for implementation details; this skill adds the
workflow order and quality criteria.

---

## 0 — Prerequisites

| Item | Required |
|---|---|
| Component FPP model compiled | Yes |
| Build cache generated (`fprime-util generate --ut`) | Yes |
| Component implementation compiles | Yes |

---

## 1 — Scaffold, implement, register, run

1. **Scaffold**: `fprime-util impl --ut` — rename `.template` files,
   delete auto-generated `*Ac.*` / `*GTestBase.*`.
2. **Tester class**: inherit `<Component>GTestBase`, add test methods
   and helper functions. See
   [Tester class structure](https://github.com/nasa/fprime/blob/devel/docs/user-manual/overview/unit-testing.md#tester-class-structure)
   and [helper functions](https://github.com/nasa/fprime/blob/devel/docs/user-manual/overview/unit-testing.md#helper-functions).
3. **TestMain**: `TEST()` macros with `COMMENT(...)` /
   `REQUIREMENT(...)`, seed `STest::Random`. See
   [TestMain](https://github.com/nasa/fprime/blob/devel/docs/user-manual/overview/unit-testing.md#testmain).
4. **CMakeLists.txt**: `register_fprime_ut(...)` with
   `UT_AUTO_HELPERS` and `DEPENDS STest`. See
   [CMakeLists.txt registration](https://github.com/nasa/fprime/blob/devel/docs/user-manual/overview/unit-testing.md#cmakeliststxt-registration).
5. **Build & run**: `fprime-util build --ut`, `fprime-util check`,
   `fprime-util check --coverage`.

---

## 2 — Key patterns to enforce

- **`clearHistory()`** at the start of every test action.
- **`component.doDispatch()`** after every async invocation on
  active/queued components.
- **Helper functions** for all port invocations and assertion groups —
  no raw `invoke_to_*` + `ASSERT_*` in test methods.
- **`STest::Pick::any()`** for port numbers, IDs, sizes where the
  specific value does not matter.

---

## 3 — Rules-based testing (preferred for complex components)

| Criteria | Simple tests | Rules-based |
|---|---|---|
| Few ports, no state machine | Preferred | Overkill |
| Multiple interacting ports | Possible | **Preferred** |
| Stateful behavior (counters, modes) | Difficult | **Preferred** |
| Need random / fuzzing coverage | Not possible | **Required** |

**Core constructs** (all from `TestUtils/RuleBasedTesting.hpp`):

- `FW_RBT_DEFINE_RULE(TesterClass, Group, Rule)` — declares a
  precondition/action pair and a rule struct on the Tester.
- **Shadow state** — a Tester member mirroring observable component
  state; queried in preconditions, updated in actions.
- **Scenarios** — `RandomScenario` (picks applicable rule at random),
  `BoundedScenario` (wraps another, stops after N steps),
  `SequenceScenario` (fixed order).

Scaffold with `fprime-util new --rule-based-test`. For file layout,
implementation patterns, and full examples see the
[rules-based testing guide](https://github.com/nasa/fprime/blob/devel/docs/how-to/rule-based-testing.md)
and the `Svc/Ccsds/ApidManager` exemplar.

**Build note:** add every `Rules/<GroupName>.cpp` and
`TestState/TestState.cpp` to the `SOURCES` list in CMakeLists.txt
and include `DEPENDS STest`.

---

## 4 — Quality checklist

- [ ] Every input port has at least one test
- [ ] Every command has nominal + error-path tests
- [ ] Every event asserted (emitted and not-emitted)
- [ ] Every telemetry channel asserted after its update action
- [ ] `clearHistory()` at start of each test action
- [ ] `doDispatch()` after every async invocation on active/queued components
- [ ] Helper functions used — no raw `invoke_to_*` + `ASSERT_*` in test methods
- [ ] `STest::Pick` for port numbers, IDs, sizes where specific value doesn't matter
- [ ] Boundary values tested (min/max buffer size, port index 0 and max)
- [ ] Rules-based testing for stateful / multi-port components (≥ 1000 random steps)
- [ ] No dynamic memory after construction
- [ ] `REQUIREMENT("...")` and `COMMENT("...")` macros present
- [ ] `fprime-util check` passes
