# How-To: Write Rule-Based Tests for F Prime Components

This guide shows how to write Rule-Based Testing (RBT) unit tests for an F Prime component.

Rule-Based Testing is a methodology for unit testing where unit tests are constructed from a set of building blocks (_Rules_) assembled in many different ways (_Scenarios_). Rules describe what to test and when. Scenarios apply rules in sequences. Each rule models behavior with:

1. A precondition that says when the rule can be applied
2. An action that performs the test

Rules are then assembled into different sequences to form the test, potentially at random and in very large numbers. This methodology provides broad coverage and high confidence in component behavior. The framework for authoring Rule-Based Testing is provided by the [`fprime/STest/` module](../../STest/README.md).

## Prerequisites

Before you start, you should have:

- Experience with F Prime unit tests (see the [LedBlinker tutorial](https://fprime.jpl.nasa.gov/latest/tutorials-led-blinker/docs/led-blinker/))
- A generated UT build (`fprime-util generate --ut`)

## When to Use Rule-Based Testing

Use RBT when:

- Your component defines a state machine or internal state affecting its behavior
- You want to leverage Rule-Based Testing to write broad coverage tests

Stick with traditional tests when component behavior is purely functional (no state) and coverage is easily obtainable through traditional UTs.

---

## Overview: Test Structure

A rule-based test has four main constructs. Two are specific to RBT:

**1. Shadow State Class** (`test/ut/TestState/`)  
A test-side model mirroring the component internal state. Preconditions query it; actions update it in lockstep with the component.

**2. Rule Implementations** (`test/ut/Rules/`)  
Each rule is a `STest::Rule` C++ struct that has: (1) a `precondition()` method returning `true` when the rule can apply; and (2) an `action()` method that drives the component and asserts test outcomes.

The other two constructs are common to all F´ unit tests:

**3. Tester Class** (`test/ut/MyComponentTester.hpp`)  
Extends the GTest base. For RBT, it includes a `shadow` state member of type `TestState` (defined above in 1.), and defines rules.

**4. Test Main** (`test/ut/MyComponentTestMain.cpp`)  
Instantiates rules and applies them via scenarios. Targeted tests can apply rules in manually-specified sequences; randomized tests apply rules in random order for many iterations.

---

## Step-by-Step Guide

### Example Component: ApidManager

The Step-by-Step guide will walk through writing rule-based tests for the `Svc/Ccsds/ApidManager` component. It is a passive component that maps identifiers (called APIDs) to sequence counts. It is essentially a lookup table that tracks the next sequence count for each APID. It exposes two ports:

- `getApidSeqCountIn`: returns the next sequence count for a given APID
- `validateApidSeqCountIn`: checks that the sequence count given as input matches the next sequence count for a given APID

In short: one port gives out a sequence count, the other validates one that came in.
The FPP model:

```python
passive component ApidManager {

    @ Port to request the next sequence count for a given APID
    guarded input port getApidSeqCountIn: Ccsds.ApidSequenceCount

    @ Port to validate an input sequence count for a given APID
    guarded input port validateApidSeqCountIn: Ccsds.ApidSequenceCount

    # ... events and standard AC ports ...
}
```

### Step 1: Identify the behaviors to cover

List the distinct behaviors the component can exhibit. For `ApidManager`, this would be:

| Behavior To Test | Action To Take | Expected outcome |
|---|---|---|
| Get count for existing APID | `getApidSeqCountIn` with tracked APID | Returns next count, no event |
| Get count for new APID (table has room) | `getApidSeqCountIn` with untracked APID | Returns 0, registers APID, no event |
| Get count for new APID (table is full) | `getApidSeqCountIn` with untracked APID | Returns `SEQUENCE_COUNT_ERROR`, sends `ApidTableFull` event |
| Validate correct count | `validateApidSeqCountIn` with expected count | No event |
| Validate wrong count | `validateApidSeqCountIn` with unexpected count | Sends `UnexpectedSequenceCount` event |

Each row maps to one rule. The internal state of ApidManager is simple and linear: its table goes from empty, to in-filling, to full.  
For components defining state machines, you would usually have at least one rule per transition, and preconditions naturally check against a specific state of the state machine.

### Step 2: Add test-state and rule directories

From your component directory, run:

```bash
fprime-util new --rule-based-test
```

This scaffolds the `test/ut/Rules/` and `test/ut/TestState/` directories. You can also create them manually.

### Step 3: Define the shadow test state

The **shadow test state** is a test-side construct that mirrors the component's internal state. Its purpose is to track the expected state of the component during testing, allowing rules to assert against the expected state, as well as driving the rule preconditions.

Design principles for shadow state:

- **Mirror only what's needed**: Don't replicate the entire component implementation, only the state required for preconditions and assertions
- **Stay synchronized**: Actions update shadow in lockstep with expected component behavior  
- **Provide helper methods if necessary**: The shadow `TestState` is a C++ class and can therefore provide helper methods to work with it

Define your shadow test state in `test/ut/TestState/TestState.hpp`. For our `ApidManager` example, we mirror the APID-to-sequence-count map with an `std::map` (allowed in test code), as well as helpers that mirror the component behavior:

```cpp
class ApidManagerTestState {
  public:
    //! Mirrors the component's internal APID-to-sequence-count map.
    std::map<ComCfg::Apid::T, U16> shadow_seqCounts;

    //! True once shadow_seqCounts has reached MAX_TRACKED_APIDS entries.
    bool shadow_isTableFull = false;

    // Shadow operations that mirror component behavior
    U16  shadow_getAndIncrementSeqCount(ComCfg::Apid::T apid);
    void shadow_validateApidSeqCount(ComCfg::Apid::T apid, U16 seqCount);
    
    // Helper methods for test randomization
    ComCfg::Apid::T shadow_getRandomTrackedApid() const;
    ComCfg::Apid::T shadow_getRandomUntrackedApid() const;
};
```

These methods are implemented in `TestState.cpp` to maintain the shadow state properly.

### Step 4: Declare rules and shadow state member in the ComponentTester class

#### Add Shadow State Member to ComponentTester

Declare the shadow state member in your ComponentTester class in `MyComponent/test/ut/MyComponentTester.hpp`. For our `ApidManager` example, this would be:

```diff
+ #include "Svc/Ccsds/ApidManager/test/ut/TestState/TestState.hpp"

 class ApidManagerTester : public ApidManagerGTestBase {
    // ... other code ...
  public:
     ApidManager component;
+    ApidManagerTestState shadow;
 };
```

This way, our tester contains two parallel models of the component state: the actual component instance (`component`) and the shadow state (`shadow`).

#### Declare Rules

Rules can be declared using the helper macro `FW_RBT_DEFINE_RULE(TesterClass, GroupName, RuleName)`. This macro creates:

- A `GroupName__RuleName__precondition()` method that returns `bool`
- A `GroupName__RuleName__action()` method that drives the test
- A `GroupName__RuleName` C++ struct of type `STest::Rule` which can be instantiated in test cases

Include `TestUtils/RuleBasedTesting.hpp` in your ComponentTester header, then declare one `FW_RBT_DEFINE_RULE` per behavior identified in Step 1. For our `ApidManager` example, this looks like:

```diff
+#include "TestUtils/RuleBasedTesting.hpp"

 class ApidManagerTester : public ApidManagerGTestBase {
   // ... other code ...

+  public:
+    FW_RBT_DEFINE_RULE(ApidManagerTester, GetSeqCount, Existing);
+    FW_RBT_DEFINE_RULE(ApidManagerTester, GetSeqCount, NewOk);
+    FW_RBT_DEFINE_RULE(ApidManagerTester, GetSeqCount, NewTableFull);
+
+    FW_RBT_DEFINE_RULE(ApidManagerTester, ValidateSeqCount, Ok);
+    FW_RBT_DEFINE_RULE(ApidManagerTester, ValidateSeqCount, Failure);
 };
```

The above defines 5 rules. Three in the `GetSeqCount` group, and two in the `ValidateSeqCount` group. It is recommended to split rules into logical groups. Here, the groups are based on the input port they exercise.

### Step 5: Implement rules

Create one `test/ut/Rules/<GroupName>.cpp` file for each rule group. Each rule has two methods that must be implemented:

1. **Precondition** (`bool GroupName__RuleName__precondition()`): Returns `true` when the rule can apply.
2. **Action** (`void GroupName__RuleName__action()`): Drives the component and verifies behavior.

For our `ApidManager` example, the `Rules/GetSeqCount.cpp` rule group exercises `getApidSeqCountIn`:

```cpp
// Rule applies when at least one APID is already tracked
bool ApidManagerTester::GetSeqCount__Existing__precondition() const {
    return !this->shadow.shadow_seqCounts.empty();
}

void ApidManagerTester::GetSeqCount__Existing__action() {
    this->clearHistory();
    // Use shadow helper to get a random APID that is tracked already
    ComCfg::Apid::T apid = this->shadow.shadow_getRandomTrackedApid();
    // Invoke component port and mirror the behavior in the shadow state
    U16 returned = this->invoke_to_getApidSeqCountIn(0, apid, 0);
    U16 expected = this->shadow.shadow_getAndIncrementSeqCount(apid);
    // Assert results and additional properties (e.g. events) as needed
    ASSERT_EQ(returned, expected);
    ASSERT_EVENTS_SIZE(0);
}
```

### Step 6: Write the test main

Rule-based tests typically include two types of test cases:

1. **Targeted tests**: Apply rules in a fixed sequence to exercise specific paths
2. **Randomized tests**: Apply rules in random order for many iterations to explore state space

Create your test main file in `test/ut/MyComponentTestMain.cpp`. For our `ApidManager` example, this would be:

```cpp
// Targeted test: manual sequence to test expected behavior
// Useful at confirming known behavior and catching regressions early
TEST(ApidManager, GetSequenceCounts) {
    ApidManagerTester tester;
    ApidManagerTester::GetSeqCount__NewOk ruleNewOk;
    ApidManagerTester::GetSeqCount__Existing ruleExisting;
    ruleNewOk.apply(tester);     // register a new APID; expect count 0
    ruleExisting.apply(tester);  // retrieve count for the same APID; expect count 1
}

// Randomized test: apply rules in a random sequence for 10,000 iterations.
TEST(ApidManager, RandomizedTesting) {
    U32 numRulesToApply = 10000;
    // Instantiate tester and each rule
    ApidManagerTester tester;
    ApidManagerTester::GetSeqCount__Existing     ruleGetExisting;
    ApidManagerTester::GetSeqCount__NewOk        ruleGetNewOk;
    ApidManagerTester::GetSeqCount__NewTableFull ruleGetNewTableFull;
    ApidManagerTester::ValidateSeqCount__Ok      ruleValidateOk;
    ApidManagerTester::ValidateSeqCount__Failure ruleValidateFailure;
    // Create an array of rule pointers to pass to the scenario
    STest::Rule<ApidManagerTester>* rules[] = {
        &ruleGetExisting, &ruleGetNewOk, &ruleGetNewTableFull,
        &ruleValidateOk,  &ruleValidateFailure,
    };
    // Run the specified rules in a random sequence for 10,000 iterations
    STest::RandomScenario<ApidManagerTester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<ApidManagerTester> bounded("Bounded Random Rules", random, numRulesToApply);
    bounded.run(tester);
}
```

**Scenarios** control how rules are applied:

- `RandomScenario`: Picks an applicable rule at random at each step
- `BoundedScenario`: Wraps another scenario and stops after N steps
- `SequenceScenario`: Applies rules in a fixed order

For additional scenario types, see `STest/STest/Scenario/`.

### Step 7: Register all UT sources in CMake

Add the tester, shadow state, and all rule files to `register_fprime_ut` in your component's `CMakeLists.txt`. For our `ApidManager` example, this would be:

```diff
 register_fprime_ut(
   SOURCES
     "${CMAKE_CURRENT_LIST_DIR}/test/ut/ApidManagerTestMain.cpp"
     "${CMAKE_CURRENT_LIST_DIR}/test/ut/ApidManagerTester.cpp"
+    "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestState/TestState.cpp"
+    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/GetSeqCount.cpp"
+    "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/ValidateSeqCount.cpp"
   AUTOCODER_INPUTS
     "${CMAKE_CURRENT_LIST_DIR}/ApidManager.fpp"
   DEPENDS
     Svc_Ccsds_Types
     STest
   UT_AUTO_HELPERS
 )
```

### Summary for How To Add a new Rule


To summarize, adding a new rule involves:

1. Add a new `FW_RBT_DEFINE_RULE` declaration in the tester header `test/ut/MyComponentTester.hpp`:

    ```diff
    +FW_RBT_DEFINE_RULE(<ComponentName>Tester, <GroupName>, <RuleName>);
    ```

2. Add the following precondition and action method implementations to the corresponding `test/ut/Rules/<GroupName>.cpp` file. For new rule groups, create a new `<GroupName>.cpp` file.

    ```diff
    + bool <ComponentName>Tester::<GroupName>__<RuleName>__precondition() const {
    +     return true; // TODO: optional precondition logic, always true can be an option
    + }
    + void <ComponentName>Tester::<GroupName>__<RuleName>__action() {
    +     // TODO: implement the rule behavior
    + }
    ```

The rule is then ready to be applied in test mains via scenarios as seen in step 6.

>[!TIP]
> To add the action and precondition boilerplate, simply copy-paste an existing rule implementation and modify the RuleName with your new rule name.

## Best Practices

- Keep preconditions side-effect free
- Keep shadow state minimal and explicit. Only mirror what preconditions and assertions actually use.
- Clear history at the start of every action with `this->clearHistory()` in order to enable asserting on what this specific rule did, and not prior behavior
- Write one rule per distinct behavior property, not one rule per port.
- Targeted test sequence (i.e. non-random) are useful to test expected behavior. Randomized sequences is powerful at hammering out edge cases and unexpected interactions.

## Advanced Usage

### Understanding the FW_RBT_DEFINE_RULE Macro

`FW_RBT_DEFINE_RULE(TesterClass, GroupName, RuleName)` expands to three things inside the tester class body:

1. A `bool GroupName__RuleName__precondition() const` method declaration — you implement this in a `.cpp` file.
2. A `void GroupName__RuleName__action()` method declaration — you implement this in a `.cpp` file.
3. A `struct GroupName__RuleName : public STest::Rule<TesterClass>` rule definition whose `precondition()` and `action()` method implementations delegate back to the methods 1. and 2., respectively.

The key design choice is that `precondition` and `action` are implemented to delegate to the tester itself rather than in the rule struct directly. This is what makes F Prime test assert macros like `ASSERT_EVENTS_*` and `ASSERT_TLM_*` work inside rule bodies — those macros expand to `this->...`, and `this` must be the tester instance.

The definition of this macro can be found in [`TestUtils/RuleBasedTesting.hpp`](../../TestUtils/RuleBasedTesting.hpp) for reference.

### Rule Parameterization at Construction

As seen above, the `FW_RBT_DEFINE_RULE` macro is a helper that easily enables the use of the F´ `ASSERT_*` macros inside rule bodies. Because the constructor of the rule is empty, it does not support parameterizing a rule at instantiation time. Parameterization can be useful in some instances and can be achieved by inlining the rule struct and specifying a constructor.

```cpp
class ApidManagerTester : public ApidManagerGTestBase {
  // ... other code ...

  public:
    // --------------------------------------------
    // Parameterized Rule: GetSeqCount__Repeated
    // --------------------------------------------
    struct GetSeqCount__Repeated : public STest::Rule<ApidManagerTester> {
        // Member variable of the rule
        U32 m_iterations;

        // Constructor
        explicit GetSeqCount__Repeated(U32 iterations) : 
            STest::Rule<ApidManagerTester>("GetSeqCount__Repeated"), 
            m_iterations(iterations) {}

        bool precondition(const ApidManagerTester& tester) override {
            return !tester.shadow.shadow_seqCounts.empty();
        }

        void action(ApidManagerTester& tester) override {
            // IMPORTANT: in the rule body here, the F´ macros such as ASSERT_EVENT_*,
            // ASSERT_TLM_*, etc. are NOT available. See note below.
            tester.clearHistory();
            for (U32 i = 0; i < this->m_iterations; i++) {
                ComCfg::Apid::T apid = tester.shadow.shadow_getRandomTrackedApid();
                U16 returned = tester.invoke_to_getApidSeqCountIn(0, apid, 0);
                U16 expected = tester.shadow.shadow_getAndIncrementSeqCount(apid);
                ASSERT_EQ(returned, expected);
            }
        }
    };
}
```

Then instantiate with the desired value in the test main:

```cpp
ApidManagerTester::GetSeqCount__Repeated rule5(5);    // Run 5 iterations
ApidManagerTester::GetSeqCount__Repeated rule25(25);  // Run 25 iterations
rule5.apply(tester);
rule25.apply(tester);
```

>[!IMPORTANT]
> Note that F Prime test assert macros (`ASSERT_EVENTS_*`, etc.) are not available inside manually-written rule structs because `this` refers to the rule, not the tester. Call those assertions through the `tester` reference passed to `action` instead (e.g. `tester.assertEvents_...()`), or inline the rule `precondition()`/`action()` to delegate to a method on the tester, as done by the `FW_RBT_DEFINE_RULE` macro.

---

## References

- [`Svc/Ccsds/ApidManager/test/ut/`](../../Svc/Ccsds/ApidManager/test/ut/)
- [`TestUtils/RuleBasedTesting.hpp`](../../TestUtils/RuleBasedTesting.hpp)
- [`STest/STest/Rule/Rule.hpp`](../../STest/STest/Rule/Rule.hpp)
- [`STest/STest/Scenario/`](../../STest/STest/Scenario/)
