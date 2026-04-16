# How-To: Write Rule-Based Tests for F Prime Components

This guide shows how to write Rule-Based Testing (RBT) unit tests for an F Prime component. It uses the concrete example in `Svc/Examples/RuleBasedTesting/RuleDemo`.

Rule-Based Testing is an immensely powerful methodology for unit testing which allows UTs to be constructed from a set of building blocks ("_Rules_") assembled into many different ways ("_Scenarios_"). Rules describe what can be tested and when, and Scenarios apply these rules in various sequences. Each rule models behavior with:

1. A precondition that says when the rule applies
2. An action that drives the component and checks outcomes

The test runner repeatedly picks applicable rules in random order, so one test explores many state sequences, and each subsequent test explores different sequences. This methodology provides broad coverage and high confidence in component behavior.

---

## Prerequisites

Before you start, you should have:

- Basic experience with F Prime unit tests (see LedBlinker tutorial)
- Basic experience with FPP component modeling
- A generated UT build (`fprime-util generate --ut`)

---

## Example Component

This example uses an FPP state machine because mode-management behavior maps naturally to states and transitions. RBT itself is not limited to state-machine components. You can apply the same RBT pattern to non-state-machine components, such as `Svc/Ccsds/ApidManager/test/ut`.

This guide uses `RuleDemo`, an active component that manages mode transitions with an internal FPP state machine.

The state machine is a good fit for this component, but it is not a requirement for RBT.

Key files:

- `Svc/Examples/RuleBasedTesting/RuleDemo/RuleDemo.fpp`
- `Svc/Examples/RuleBasedTesting/RuleDemo/RuleDemo.hpp`
- `Svc/Examples/RuleBasedTesting/RuleDemo/RuleDemo.cpp`

The state machine definition in `RuleDemo.fpp`:

```python
state machine ModeManagement {

    signal goOps
    signal goScience
    signal goSafe

    action enterSafeMode
    action enterOpsMode
    action enterScienceMode

    guard isScienceReady

    state SAFE {
        entry do { enterSafeMode }
        on goOps enter OPERATIONS
    }
    state OPERATIONS {
        entry do { enterOpsMode }
        on goScience if isScienceReady enter SCIENCE
        on goSafe enter SAFE
    }
    state SCIENCE {
        entry do { enterScienceMode }
        on goOps enter OPERATIONS
        on goSafe enter SAFE
    }

    initial enter SAFE
}
```

---

## Test Structure

The UT implementation follows four pieces:

1. Tester class with rule declarations
2. Shadow state model
3. Rule implementation files
4. RandomScenario test main

### 1. Tester Class

In `RuleDemoTester.hpp`, declare rules with `FW_RBT_DEFINE_RULE` within your `ComponentTester` class:

```cpp
class RuleDemoTester final : public RuleDemoGTestBase {
    // ... other declarations ...

  public:
    FW_RBT_DEFINE_RULE(RuleDemoTester, SwitchMode, SafeToOps);
    FW_RBT_DEFINE_RULE(RuleDemoTester, SwitchMode, OpsToSafe);
    FW_RBT_DEFINE_RULE(RuleDemoTester, SwitchMode, OpsToScienceNotReady);
    FW_RBT_DEFINE_RULE(RuleDemoTester, GetMode, MatchesShadow);
};
```

### 2. Shadow Test State

In `test/ut/TestState/TestState.hpp`, mirror the set of state and behavior you assert:

```cpp
class RuleDemoTestState {
  public:
    ModeEnum m_mode = ModeEnum::SAFE;
    bool m_isScienceReady = false;

    // Note: these helpers are not strictly necessary, you could directly act on the member
    // variables, but it is good practice for when the behavior becomes more complex.
    bool isSafe() const;
    bool isOps() const;
    bool isScience() const;
    void setSafe();
    void setOps();
    void setScience();
};
```

This follows the same pattern used in ApidManager-style RBT: keep test state separate from rule methods.

For comparison, `Svc/Ccsds/ApidManager/test/ut` demonstrates this same shadow-state and rule pattern on a component that does not rely on an internal FPP state machine.

### 3. Rule Implementations

In `test/ut/Rules/SwitchMode.cpp`, each rule has precondition and action. For example, the `SafeToOps` rule models the `SAFE -> OPERATIONS` transition. It can only apply when the shadow state is `SAFE`, and the action drives the transition, checks the result, and updates shadow state.

```cpp
bool RuleDemoTester::SwitchMode__SafeToOps__precondition() const {
    // The rule only applies if the shadow state is SAFE, so check that as the precondition
    return this->shadow.isSafe();
}

void RuleDemoTester::SwitchMode__SafeToOps__action() {
    this->clearHistory();
    // Drive the component through the mode switch
    this->invoke_to_switchMode(0, ModeEnum::OPERATIONS);
    this->dispatchAll(); // dispatch async message (because this is an active component)
    // Mirror the expected state change in the shadow test state
    this->shadow.setOps();

    // Assert the component getMode port reports the expected test state
    const ModeEnum currentMode = this->invoke_to_getMode(0);
    ASSERT_EQ(currentMode, ModeEnum::OPERATIONS);
    ASSERT_EQ(currentMode, this->shadow.m_mode);
}
```

The guard case (`OPERATIONS -> SCIENCE` while not ready):

```cpp
bool RuleDemoTester::SwitchMode__OpsToScienceNotReady__precondition() const {
    return this->shadow.shadow_isOps() && (!this->shadow.shadow_isScienceReady);
}

void RuleDemoTester::SwitchMode__OpsToScienceNotReady__action() {
    this->clearHistory();

    this->invoke_to_switchMode(0, ModeEnum::SCIENCE);
    this->dispatchAll();

    const ModeEnum currentMode = this->invoke_to_getMode(0);
    ASSERT_EQ(currentMode, ModeEnum::OPERATIONS);
    ASSERT_EQ(currentMode, this->shadow.m_mode);
}
```

> [!TIP]
> It is good practice to assert against the shadow state as well as the expected mode. This way, if the test fails, you can determine whether the component state diverged from the shadow model (indicating a potential bug in the component), or whether the test failed because the shadow model was wrong (indicating a potential bug in the test).

### 4. Random Scenario Main

In `test/ut/RuleDemoTestMain.cpp`, run 1000 iterations:

```cpp
std::vector<STest::Rule<RuleBasedTesting::RuleDemoTester>*> rules = {
    &safeToOps,
    &opsToSafe,
    &opsToScienceNotReady,
    &getModeMatchesShadow,
};

STest::RandomScenario<RuleBasedTesting::RuleDemoTester> scenario("RuleDemo", rules);
for (U32 i = 0; i < 1000; i++) {
    scenario.step(tester);
}
```

For additional scenario types, see `STest/STest/Scenario/`.

---

## Step-by-Step Guide

### Step 1: Confirm the model and test-facing ports in FPP

For this example, model the state transitions in FPP and expose the ports that tests call.

```python
active component RuleDemo {
    state machine instance sm: ModeManagement

    async input port switchMode: ModeSetter
    sync input port getMode: ModeGetter
}
```

### Step 2: Add test-state and rule directories

Create directories under `test/ut`:

```bash
cd MyComponent/test/ut
fprime-util new --rule-based-test
```

This is implemented as a separate command from `fprime-util impl --ut` to give users the option to add RBT structure to an existing UT build.

### Step 3: Add a shadow model in TestState

Create `test/ut/TestState/TestState.hpp` and `test/ut/TestState/TestState.cpp`.

```cpp
class RuleDemoTestState {
  public:
    ModeEnum shadow_mode = ModeEnum::SAFE;
    bool shadow_isScienceReady = false;

    bool shadow_isSafe() const;
    bool shadow_isOps() const;
    bool shadow_isScience() const;

    void shadow_setSafe();
    void shadow_setOps();
    void shadow_setScience();
};
```

### Step 4: Declare rules in the tester header

Update `RuleDemoTester.hpp` with the shadow include, macro include, and rule declarations.

```diff
--- a/Svc/Examples/RuleBasedTesting/RuleDemo/test/ut/RuleDemoTester.hpp
+++ b/Svc/Examples/RuleBasedTesting/RuleDemo/test/ut/RuleDemoTester.hpp
@@
+#include "RuleBasedTesting/RuleDemo/test/ut/TestState/TestState.hpp"
+#include "TestUtils/RuleBasedTesting.hpp"
@@
+RuleDemoTestState shadow;
@@
+FW_RBT_DEFINE_RULE(RuleDemoTester, SwitchMode, SafeToOps);
+FW_RBT_DEFINE_RULE(RuleDemoTester, SwitchMode, OpsToSafe);
+FW_RBT_DEFINE_RULE(RuleDemoTester, SwitchMode, OpsToScienceNotReady);
+FW_RBT_DEFINE_RULE(RuleDemoTester, GetMode, MatchesShadow);
```

### Step 5: Implement switchMode rules in Rules/SwitchMode.cpp

Implement preconditions and actions.

```cpp
bool RuleDemoTester::SwitchMode__SafeToOps__precondition() const {
    return this->shadow.shadow_isSafe();
}

void RuleDemoTester::SwitchMode__SafeToOps__action() {
    this->clearHistory();
    this->invoke_to_switchMode(0, ModeEnum::OPERATIONS);
    this->dispatchAll();
    ASSERT_EQ(this->invoke_to_getMode(0), ModeEnum::OPERATIONS);
    this->shadow.shadow_setOps();
}
```

### Step 6: Implement an invariant rule in Rules/GetMode.cpp

Use a broad rule that is always applicable and checks state consistency.

```cpp
bool RuleDemoTester::GetMode__MatchesShadow__precondition() const {
    return true;
}

void RuleDemoTester::GetMode__MatchesShadow__action() {
    this->clearHistory();
    ASSERT_EQ(this->invoke_to_getMode(0), this->shadow.shadow_mode);
}
```

### Step 7: Add dispatch helper and RandomScenario main

For active components, flush the queue after async invocations.

```cpp
void RuleDemoTester::dispatchAll() {
    RuleDemoComponentBase::MsgDispatchStatus status = RuleDemoComponentBase::MSG_DISPATCH_OK;
    while (status == RuleDemoComponentBase::MSG_DISPATCH_OK) {
        status = this->component.doDispatch();
        FW_ASSERT(status != RuleDemoComponentBase::MSG_DISPATCH_ERROR);
    }
}
```

In `RuleDemoTestMain.cpp`, run 1000 random steps:

```cpp
STest::RandomScenario<RuleBasedTesting::RuleDemoTester> scenario("RuleDemo", rules);
for (U32 i = 0; i < 1000; i++) {
    scenario.step(tester);
}
```

### Step 8: Register all UT sources in CMake

Update `RuleDemo/CMakeLists.txt` so UT builds include tester, rules, and test state.

```diff
--- a/Svc/Examples/RuleBasedTesting/RuleDemo/CMakeLists.txt
+++ b/Svc/Examples/RuleBasedTesting/RuleDemo/CMakeLists.txt
@@
 register_fprime_ut(
@@
     SOURCES
         "${CMAKE_CURRENT_LIST_DIR}/test/ut/RuleDemoTestMain.cpp"
         "${CMAKE_CURRENT_LIST_DIR}/test/ut/RuleDemoTester.cpp"
+        "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/SwitchMode.cpp"
+        "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/GetMode.cpp"
+        "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestState/TestState.cpp"
@@
 )
```

---

## Best Practices

- Keep preconditions pure and fast.
- Keep shadow state minimal and explicit.
- Clear history at the start of each action.
- Use one rule per behavior property.
- Add at least one broadly applicable rule (for example, a state-consistency check).

---

## References

- `Svc/Examples/RuleBasedTesting/RuleDemo/test/ut`
- `TestUtils/RuleBasedTesting.hpp`
- `STest/STest/Rule/Rule.hpp`
- `STest/STest/Scenario/`
- https://nasa.github.io/fpp/fpp-users-guide.html
- `docs/user-manual/framework/state-machines.md`
