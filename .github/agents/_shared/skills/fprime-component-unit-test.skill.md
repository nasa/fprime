---
name: fprime-component-unit-test
description: Unit testing phase of F Prime component development. Guides the agent through writing GTest-based unit tests using the autocoded test harness. Covers both traditional tests and rule-based testing. Trigger when implementation is ready for testing or when following TDD (tests before implementation). Keywords: F Prime, unit test, GTest, TesterBase, GTestBase, Tester, fprime-util check, STest, rule-based testing.
---

# Skill: F Prime Component Unit Testing

Unit tests verify each component requirement in isolation using the
autocoded test harness. F Prime generates `TesterBase` and `GTestBase`
classes; you write a `Tester` that drives the component and asserts
correct behavior.

---

## STOP — Confirm Test Scope

Before writing tests, **ask the user**:

1. Which requirements should the tests cover? (Map tests to REQ-*)
2. Should we use **traditional tests** (simple, one-behavior-per-test)
   or **rule-based tests** (state-machine components, broad coverage)?
3. Are there any behaviors that require mocking external dependencies?
4. What edge cases or off-nominal paths should be exercised?
5. Is there a target code coverage percentage?

**Do not invent test scenarios.** Tests verify requirements — if a
behavior isn't in the requirements, ask before testing it.

---

## Step-by-Step Process (Traditional Tests)

### Step 1 — Generate Unit Test Stubs

```bash
fprime-util generate --ut
fprime-util impl --ut
```

This produces template files. If first time:

```bash
cd test/ut/
mv <Component>Tester-template.cpp <Component>Tester.cpp
mv <Component>Tester-template.hpp <Component>Tester.hpp
mv <Component>TestMain-template.cpp <Component>TestMain.cpp
```

### Step 2 — Register Unit Tests in CMakeLists.txt

Ensure the component's `CMakeLists.txt` includes:

```cmake
register_fprime_ut(
    AUTOCODER_INPUTS
        "${CMAKE_CURRENT_LIST_DIR}/<Component>.fpp"
    SOURCES
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/<Component>TestMain.cpp"
        "${CMAKE_CURRENT_LIST_DIR}/test/ut/<Component>Tester.cpp"
)
```

### Step 3 — Write Test Cases

Structure: one test per behavior / requirement.

**Test Main** (`<Component>TestMain.cpp`):

```cpp
#include "<Component>Tester.hpp"

TEST(Nominal, Increment) {
    <Component>Tester tester;
    tester.test_increment();
}

TEST(OffNominal, InvalidCommand) {
    <Component>Tester tester;
    tester.test_invalidCommand();
}
```

**Tester Class** (`<Component>Tester.cpp`):

```cpp
void <Component>Tester::test_increment() {
    // Arrange: set up preconditions
    // (ask the user if setup is unclear)

    // Act: invoke component
    this->invoke_to_myPort(0, args...);
    // For active/queued: dispatch the message
    this->component.doDispatch();

    // Assert: verify behavior
    ASSERT_TLM_Count_SIZE(1);
    ASSERT_TLM_Count(0, expectedValue);
    ASSERT_EVENTS_SIZE(0);  // no unexpected events
}
```

### Step 4 — Use the Assertion Macros

The GTestBase provides these assertion macros:

**Commands:**
```cpp
this->sendCOMMAND_NAME(cmdSeq, arg1, arg2);
this->component.doDispatch();  // for async commands
ASSERT_CMD_RESPONSE_SIZE(1);
ASSERT_CMD_RESPONSE(0, <Component>::OPCODE_COMMAND_NAME, cmdSeq, Fw::CmdResponse::OK);
```

**Telemetry:**
```cpp
ASSERT_TLM_SIZE(n);                    // total telemetry entries
ASSERT_TLM_ChannelName_SIZE(n);        // entries on specific channel
ASSERT_TLM_ChannelName(index, value);  // value at history index
```

**Events:**
```cpp
ASSERT_EVENTS_SIZE(n);                       // total events
ASSERT_EVENTS_EventName_SIZE(n);             // count of specific event
ASSERT_EVENTS_EventName(index, arg1, arg2);  // args at history index
```

**Output ports:**
```cpp
ASSERT_FROM_PORT_HISTORY_SIZE(n);
ASSERT_from_PortName_SIZE(n);
ASSERT_from_PortName(index, arg1, arg2);
```

**Parameters:**
```cpp
this->paramSet_ParamName(value, Fw::ParamValid::VALID);
```

**Time:**
```cpp
this->setTestTime(Fw::Time(seconds, microseconds));
```

### Step 5 — Clear History Between Sub-Tests

```cpp
this->clearHistory();
```

Call this when testing multiple invocations to reset assertion indices.

### Step 6 — Test Off-Nominal Paths

For every command, test:
- Invalid arguments → expect error response + warning event
- Valid arguments → expect OK response

For every port:
- Out-of-range inputs → expect graceful handling (not FW_ASSERT)

**Ask the user** what constitutes invalid input for each interface.

### Step 7 — Run Tests

```bash
fprime-util check
```

All tests must pass. For coverage analysis:

```bash
fprime-util check --coverage
```

Review `*_gcov.txt` files in the component directory.

---

## Rule-Based Testing (for Stateful Components)

Use when the component has internal state that affects behavior (state
machines, counters, lookup tables, modes).

See `docs/how-to/rule-based-testing.md` for the full walkthrough.

### Quick Summary

1. **Scaffold**: `fprime-util new --rule-based-test`
2. **Define shadow state** in `test/ut/TestState/TestState.hpp` —
   mirrors component state needed for preconditions/assertions.
3. **Declare rules** with `FW_RBT_DEFINE_RULE(Tester, Group, Rule)`.
4. **Implement preconditions** — return `true` when the rule applies
   to current shadow state.
5. **Implement actions** — drive component, assert, update shadow.
6. **Write scenarios** in TestMain:
   - Manual sequences for targeted tests
   - Random application for broad coverage

**Ask the user** to enumerate the behaviors/transitions to cover.

---

## Project Structure

```
MyComponent/
├── CMakeLists.txt
├── MyComponent.fpp
├── MyComponent.hpp
├── MyComponent.cpp
└── test/
    └── ut/
        ├── MyComponentTester.hpp
        ├── MyComponentTester.cpp
        ├── MyComponentTestMain.cpp
        ├── TestState/          (rule-based only)
        │   ├── TestState.hpp
        │   └── TestState.cpp
        └── Rules/              (rule-based only)
            ├── Group1.cpp
            └── Group2.cpp
```

---

## Anti-Patterns

- ❌ Writing tests without confirmed requirements to verify against
- ❌ Testing only happy paths — off-nominal coverage is critical
- ❌ Forgetting `doDispatch()` for async commands/ports
- ❌ Asserting only on count (`SIZE`) without checking values
- ❌ Using `DISABLED_` or `GTEST_SKIP` without justification
- ❌ Modifying component internals directly (test through the
  interface only)
- ❌ Guessing expected values — derive from requirements or ask user
