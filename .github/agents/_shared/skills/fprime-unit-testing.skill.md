---
name: fprime-unit-testing
description: >
  Write F Prime component unit tests. Covers scaffold generation via
  `fprime-util impl --ut`, the Tester / TestMain / GTestBase pattern,
  rules-based testing with STest, helper-function design, and
  CMakeLists.txt registration. Use whenever creating or extending
  unit tests for an F Prime component.
---

# Skill: F Prime Unit Testing

Step-by-step procedure for writing high-quality unit tests for an
F Prime component. Follow every section in order.

---

## 0 — Prerequisites

| Item | Required |
|---|---|
| Component FPP model compiled | Yes |
| Build cache generated (`fprime-util generate --ut`) | Yes |
| Component implementation compiles | Yes |

---

## 1 — Scaffold the UT files

```bash
fprime-util impl --ut
```

Generates template files under `test/ut/`:

| Generated file | Purpose | Action |
|---|---|---|
| `<Component>Tester.template.hpp` | Tester class header | Rename → `<Component>Tester.hpp` |
| `<Component>Tester.template.cpp` | Tester class impl | Rename → `<Component>Tester.cpp` |
| `TestMain.template.cpp` | GTest `main()` entry | Rename → `TestMain.cpp` |
| `*Ac.*` / `*GTestBase.*` | Auto-generated bases | **Delete** — regenerated at build time |

---

## 2 — Tester class and GTestBase

The Tester inherits `<Component>GTestBase`. The base provides:

- **Port invocation**: `invoke_to_<portName>(portNum, ...)`
- **Port history assertions**:
  `ASSERT_FROM_PORT_HISTORY_SIZE(n)`,
  `ASSERT_from_<portName>_SIZE(n)`,
  `ASSERT_from_<portName>(index, expectedArgs...)`
- **Event assertions**:
  `ASSERT_EVENTS_SIZE(n)`,
  `ASSERT_EVENTS_<EventName>(index, expectedArgs...)`
- **Telemetry assertions**:
  `ASSERT_TLM_<ChannelName>_SIZE(n)`,
  `ASSERT_TLM_<ChannelName>(index, expectedValue)`
- **Command assertions**:
  `ASSERT_CMD_RESPONSE_SIZE(n)`,
  `ASSERT_CMD_RESPONSE(index, opcode, cmdSeq, response)`
- **`clearHistory()`** — call at the start of each action
- **`component.doDispatch()`** — required after async invocations on active/queued components

### Minimal Tester skeleton

```cpp
#include "<Component>GTestBase.hpp"
#include "<Namespace>/<Component>/<Component>.hpp"

namespace <Namespace> {

class <Component>Tester : public <Component>GTestBase {
  public:
    static constexpr U32 MAX_HISTORY_SIZE = 10;
    static constexpr FwEnumStoreType TEST_INSTANCE_ID = 0;
    static constexpr FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;

    <Component>Tester();
    ~<Component>Tester();

    void testNominal();

  private:
    void connectPorts();
    void initComponents();
    <Component> component;
};

}  // namespace <Namespace>
```

Constructor must call `initComponents()` then `connectPorts()`.

---

## 3 — TestMain

```cpp
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "<Namespace>/<Component>/test/ut/<Component>Tester.hpp"

namespace <Namespace> {

TEST(<Component>, Nominal) {
    COMMENT("Describe what this test verifies.");
    REQUIREMENT("REQ-ID");
    <Component>Tester tester;
    tester.testNominal();
}

}  // namespace <Namespace>

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
```

Always seed `STest::Random`. Use `COMMENT(...)` and `REQUIREMENT(...)` from `Fw/Test/UnitTest.hpp`.

---

## 4 — Helper functions

**Every test method should read as a sequence of meaningful actions,
not raw port calls and assertion macros.**

### Action helper

```cpp
void <Component>Tester::sendScheduleTick() {
    this->clearHistory();
    const U32 context = STest::Pick::any();
    this->invoke_to_schedIn(0, context);
    this->component.doDispatch();   // active components only
}
```

### Assertion helper

```cpp
void <Component>Tester::assertTelemetryIdle() {
    ASSERT_TLM_Counter_SIZE(0);
    ASSERT_EVENTS_SIZE(0);
}
```

### Combined test

```cpp
void <Component>Tester::testNominal() {
    sendScheduleTick();
    ASSERT_TLM_Counter_SIZE(1);
    ASSERT_TLM_Counter(0, 1);
    ASSERT_EVENTS_SIZE(0);
}
```

---

## 5 — Rules-based testing (preferred for complex components)

Use rule-based testing when the component has internal state or
multiple interacting ports. Full guide:
[`docs/how-to/rule-based-testing.md`](https://github.com/nasa/fprime/blob/devel/docs/how-to/rule-based-testing.md).

### 5.1 — When to use

| Criteria | Simple tests | Rules-based |
|---|---|---|
| Few ports, no state machine | Preferred | Overkill |
| Multiple interacting ports | Possible | **Preferred** |
| Stateful behavior (counters, modes) | Difficult | **Preferred** |
| Need random / fuzzing coverage | Not possible | **Required** |

### 5.2 — Scaffold

```bash
fprime-util new --rule-based-test
```

This creates `test/ut/TestState/` and `test/ut/Rules/` directories.

### 5.3 — File layout

```
<Component>/test/ut/
    <Component>Tester.{hpp,cpp}     # Tester class (includes shadow + rules)
    <Component>TestMain.cpp         # TEST() macros + scenarios
    TestState/TestState.{hpp,cpp}   # Shadow state class
    Rules/<GroupName>.cpp           # Rule implementations per group
```

### 5.4 — Shadow test state

Mirror the component's internal state in `test/ut/TestState/TestState.hpp`.
Only mirror what preconditions and assertions need. Update it in
lockstep with expected component behavior.

```cpp
class <Component>TestState {
  public:
    std::map<U32, U16> shadow_counts;  // mirrors component internal table
    bool shadow_isTableFull = false;

    // Shadow operations that mirror component behavior
    U16  shadow_getAndIncrement(U32 id);
    U32  shadow_getRandomTrackedId() const;
    U32  shadow_getRandomUntrackedId() const;
};
```

Declare it as a member of the Tester: `<Component>TestState shadow;`

### 5.5 — Declare rules in the Tester

Use `FW_RBT_DEFINE_RULE` from `TestUtils/RuleBasedTesting.hpp`. Each
invocation declares a precondition method, an action method, and a
rule struct that delegates to them on the Tester (so F Prime assert
macros like `ASSERT_EVENTS_*` work inside rule bodies via `this`).

```cpp
#include "TestUtils/RuleBasedTesting.hpp"

class <Component>Tester : public <Component>GTestBase {
  public:
    <Component> component;
    <Component>TestState shadow;

    FW_RBT_DEFINE_RULE(<Component>Tester, GetCount, Existing);
    FW_RBT_DEFINE_RULE(<Component>Tester, GetCount, NewOk);
    FW_RBT_DEFINE_RULE(<Component>Tester, GetCount, NewTableFull);
    FW_RBT_DEFINE_RULE(<Component>Tester, Validate, Ok);
    FW_RBT_DEFINE_RULE(<Component>Tester, Validate, Failure);
    // ...
};
```

### 5.6 — Implement rules

Create `test/ut/Rules/<GroupName>.cpp` per group. Each rule has two
methods on the Tester:

```cpp
// Precondition: side-effect-free, queries shadow state
bool <Component>Tester::GetCount__Existing__precondition() const {
    return !this->shadow.shadow_counts.empty();
}

// Action: drive component, assert results, update shadow
void <Component>Tester::GetCount__Existing__action() {
    this->clearHistory();
    U32 id = this->shadow.shadow_getRandomTrackedId();
    U16 returned = this->invoke_to_getCountIn(0, id, 0);
    U16 expected = this->shadow.shadow_getAndIncrement(id);
    ASSERT_EQ(returned, expected);
    ASSERT_EVENTS_SIZE(0);
}
```

### 5.7 — Test main with scenarios

```cpp
// Targeted test: fixed sequence for known behavior
TEST(<Component>, GetCounts) {
    <Component>Tester tester;
    <Component>Tester::GetCount__NewOk ruleNewOk;
    <Component>Tester::GetCount__Existing ruleExisting;
    ruleNewOk.apply(tester);
    ruleExisting.apply(tester);
}

// Randomized test: random rule application for broad coverage
TEST(<Component>, RandomizedTesting) {
    <Component>Tester tester;
    <Component>Tester::GetCount__Existing     ruleGetExisting;
    <Component>Tester::GetCount__NewOk        ruleGetNewOk;
    <Component>Tester::GetCount__NewTableFull ruleGetFull;
    <Component>Tester::Validate__Ok           ruleValOk;
    <Component>Tester::Validate__Failure      ruleValFail;

    STest::Rule<<Component>Tester>* rules[] = {
        &ruleGetExisting, &ruleGetNewOk, &ruleGetFull,
        &ruleValOk, &ruleValFail,
    };
    STest::RandomScenario<<Component>Tester> random(
        "Random", rules, FW_NUM_ARRAY_ELEMENTS(rules));
    STest::BoundedScenario<<Component>Tester> bounded(
        "Bounded Random", random, 10000);
    bounded.run(tester);
}
```

Scenario types: `RandomScenario` (picks applicable rule at random),
`BoundedScenario` (wraps another and stops after N steps),
`SequenceScenario` (fixed order).

---

## 6 — CMakeLists.txt

```cmake
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/<Component>.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/<Component>TestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/<Component>Tester.cpp"
    # For rules-based tests, add:
    #   "${CMAKE_CURRENT_LIST_DIR}/test/ut/TestState/TestState.cpp"
    #   "${CMAKE_CURRENT_LIST_DIR}/test/ut/Rules/<GroupName>.cpp"
  DEPENDS
    STest
  UT_AUTO_HELPERS
)
```

- **`UT_AUTO_HELPERS`**: autocodes `connectPorts`/`initComponents` from FPP. Omit only if custom port wiring is needed.
- **`DEPENDS STest`**: required when using `STest::Pick`, `STest::Rule`, or scenario classes.

---

## 7 — Build and run

```bash
fprime-util build --ut       # build
fprime-util check            # run
fprime-util check --coverage # run with coverage
```

---

## 8 — Quality checklist

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
