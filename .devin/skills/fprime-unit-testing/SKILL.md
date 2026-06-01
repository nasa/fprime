---
name: fprime-unit-testing
description: >
  Write F Prime component unit tests. Covers scaffold generation via
  `fprime-util impl --ut`, the Tester / TestMain / GTestBase pattern,
  rules-based testing with STest, helper-function design, and the
  CMakeLists.txt registration. Use whenever creating or extending
  unit tests for an F Prime component.
triggers:
  - model
  - user
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

From the component directory, run:

```bash
fprime-util impl --ut
```

This generates template files under `test/ut/`:

| Generated file | Purpose | Keep? |
|---|---|---|
| `<Component>Tester.template.hpp` | Tester class header | **Yes — rename to `<Component>Tester.hpp`** |
| `<Component>Tester.template.cpp` | Tester class impl | **Yes — rename to `<Component>Tester.cpp`** |
| `TestMain.template.cpp` | GTest `main()` entry | **Yes — rename to `TestMain.cpp`** |
| Other `*Ac.*` / `*GTestBase.*` files | Auto-generated bases | **No — delete; they regenerate at build time** |

Rename the template files (remove `.template` from the names) and
delete any auto-generated files that were placed alongside them.

---

## 2 — Understand the generated Tester class

The Tester class inherits from `<Component>GTestBase`, which itself
inherits from the component's auto-generated test harness. The base
class provides:

- **Port invocation helpers**: `invoke_to_<portName>(portNum, ...)`
- **Port history**: `fromPortHistory_<portName>`, push helpers
- **History assertions** (GTest macros):
  - `ASSERT_FROM_PORT_HISTORY_SIZE(n)` — total output port calls
  - `ASSERT_from_<portName>_SIZE(n)` — calls on a specific port
  - `ASSERT_from_<portName>(index, expectedArgs...)` — arg check
- **Event assertions**:
  - `ASSERT_EVENTS_SIZE(n)`
  - `ASSERT_EVENTS_<EventName>(index, expectedArgs...)`
- **Telemetry assertions**:
  - `ASSERT_TLM_<ChannelName>_SIZE(n)`
  - `ASSERT_TLM_<ChannelName>(index, expectedValue)`
- **Command assertions**:
  - `ASSERT_CMD_RESPONSE_SIZE(n)`
  - `ASSERT_CMD_RESPONSE(index, opcode, cmdSeq, response)`
- **History clearing**: `clearHistory()` — call at the start of
  each test action so assertions apply only to the current step
- **Dispatch** (active components): `component.doDispatch()` —
  processes one message off the queue

The Tester constructor must call `initComponents()` then
`connectPorts()`. Store the component under test as a member named
`component`.

### Minimal Tester skeleton

```cpp
// <Component>Tester.hpp
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

    // --- Tests ---
    void testNominal();

  private:
    // --- Handlers for typed from-ports ---
    // One handler per output port on the component.
    // Push entries into port history inside each handler.

    // --- Helpers ---
    void connectPorts();
    void initComponents();

    // --- Variables ---
    <Component> component;
};

}  // namespace <Namespace>
```

---

## 3 — Write the TestMain

```cpp
#include "Fw/Test/UnitTest.hpp"
#include "STest/Random/Random.hpp"
#include "<Namespace>/<Component>/test/ut/<Component>Tester.hpp"

namespace <Namespace> {

TEST(<Component>, Nominal) {
    COMMENT("Describe what this test verifies.");
    REQUIREMENT("REQ-ID");           // link to requirement if available
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

Key points:
- Use the `TEST(GroupName, TestName)` macro from GTest.
- Use `COMMENT(...)` and `REQUIREMENT(...)` from `Fw/Test/UnitTest.hpp`
  to document what is being tested and which requirement is covered.
- Always seed the STest random number generator.

---

## 4 — Write tests using helper functions

**Every test method should read as a sequence of meaningful actions**,
not raw port calls and assertion macros. Extract helpers into the
Tester class.

### Pattern: action helpers

```cpp
void <Component>Tester::sendScheduleTick() {
    this->clearHistory();
    const U32 context = STest::Pick::any();
    this->invoke_to_schedIn(0, context);
    this->component.doDispatch();   // active components only
}
```

### Pattern: assertion helpers

```cpp
void <Component>Tester::assertTelemetryIdle() {
    ASSERT_TLM_Counter_SIZE(0);
    ASSERT_EVENTS_SIZE(0);
}
```

### Pattern: combined setup + verify

```cpp
void <Component>Tester::testNominal() {
    sendScheduleTick();
    ASSERT_TLM_Counter_SIZE(1);
    ASSERT_TLM_Counter(0, 1);
    ASSERT_EVENTS_SIZE(0);
}
```

The goal: **a reader who knows the component's FPP model should
understand the test without looking up GTestBase internals.**

---

## 5 — Rules-based testing (preferred for complex components)

When a component has multiple interacting ports or stateful behavior,
use the STest rules-based framework for thorough coverage. This
extends the basic Tester with an abstract state machine and
precondition/action rules that STest can run randomly.

### 5.1 — File layout

```
<Component>/
  test/ut/
    <Component>Tester.hpp          # Tester class (base)
    <Component>Tester.cpp
    <Component>TestMain.cpp        # GTest entry + TEST() macros
    AbstractState.hpp              # Abstract model of component state
    TestState/
      TestState.hpp                # Extends Tester, declares rule methods
    Rules/
      Rules.hpp                    # RULES_DEF_RULE macro + rule declarations
      <RuleGroup>.hpp              # Per-group Tester class + test methods
      <RuleGroup>.cpp              # Preconditions, actions, test bodies
      Testers.hpp                  # Extern declarations of all group testers
      Testers.cpp                  # Definitions of all group testers
    Scenarios/
      Random.hpp                   # Random scenario runner
      Random.cpp
```

### 5.2 — AbstractState

Model the component's logical state as a plain C++ struct:

```cpp
class AbstractState {
  public:
    static constexpr FwSizeType MIN_BUFFER_SIZE = 1;
    static constexpr FwSizeType MAX_BUFFER_SIZE = 1024;

    enum class BufferGetStatus { VALID, INVALID };

    BufferGetStatus bufferGetStatus{BufferGetStatus::VALID};
    TestUtils::OnChangeChannel<U32> NumSuccessfulAllocations{0};
    // ... mirror every piece of observable state
};
```

Store it in the Tester: `AbstractState abstractState;`

### 5.3 — TestState

TestState extends the Tester and declares precondition/action method
pairs for every rule. Use the macro helper:

```cpp
#define TEST_STATE_DEF_RULE(GROUP_NAME, RULE_NAME)          \
    bool precondition__##GROUP_NAME##__##RULE_NAME() const; \
    void action__##GROUP_NAME##__##RULE_NAME();

class TestState : public <Component>Tester {
  public:
    TEST_STATE_DEF_RULE(PortGroupA, NominalCase)
    TEST_STATE_DEF_RULE(PortGroupA, ErrorCase)
    // ...
};
```

### 5.4 — Rules.hpp

Declare STest rule structs that delegate to TestState methods:

```cpp
#include "STest/Rule/Rule.hpp"
#include "<path>/TestState/TestState.hpp"

#define RULES_DEF_RULE(GROUP_NAME, RULE_NAME)                         \
    namespace GROUP_NAME {                                            \
    struct RULE_NAME : public STest::Rule<TestState> {                \
        RULE_NAME() : Rule<TestState>(#GROUP_NAME "." #RULE_NAME) {}  \
        bool precondition(const TestState& state) {                   \
            return state.precondition__##GROUP_NAME##__##RULE_NAME(); \
        }                                                             \
        void action(TestState& state) {                               \
            state.action__##GROUP_NAME##__##RULE_NAME();              \
        }                                                             \
    };                                                                \
    }

namespace <Namespace> {
namespace Rules {

RULES_DEF_RULE(PortGroupA, NominalCase)
RULES_DEF_RULE(PortGroupA, ErrorCase)
// ...

}  // namespace Rules
}  // namespace <Namespace>
```

### 5.5 — Rule group file (e.g., PortGroupA.cpp)

Implement preconditions and actions on TestState, then write the
per-group Tester that composes rules:

```cpp
// --- Precondition ---
bool TestState::precondition__PortGroupA__NominalCase() const {
    return this->abstractState.bufferGetStatus ==
           AbstractState::BufferGetStatus::VALID;
}

// --- Action ---
void TestState::action__PortGroupA__NominalCase() {
    this->clearHistory();
    // Invoke port, dispatch, assert results
    const auto portNum = static_cast<FwIndexType>(
        STest::Pick::startLength(0, NumPorts));
    this->invoke_to_somePort(portNum, args...);
    this->doDispatch();
    ASSERT_EVENTS_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    // Update abstract state
    ++this->abstractState.NumSuccessfulOps.value;
}

// --- Per-group Tester ---
namespace PortGroupA {
void Tester::NominalCase() {
    this->ruleNominalCase.apply(this->testState);
    Testers::schedIn.ruleOK.apply(this->testState);
}
}  // namespace PortGroupA
```

### 5.6 — Random scenario

Wire all rules into a bounded random walk:

```cpp
void Tester::run(U32 maxNumSteps) {
    STest::Rule<TestState>* rules[] = {
        &ruleA, &ruleB, &ruleC, ...
    };
    STest::RandomScenario<TestState> scenario(
        "Random", rules,
        sizeof(rules) / sizeof(STest::Rule<TestState>*));
    STest::BoundedScenario<TestState> bounded(
        "BoundedRandom", scenario, maxNumSteps);
    const U32 numSteps = bounded.run(this->testState);
    printf("Ran %u steps.\n", numSteps);
}
```

Register the random scenario in TestMain:

```cpp
TEST(Scenarios, Random) {
    COMMENT("Random scenario with all rules.");
    const FwSizeType numSteps = 10000;
    Scenarios::Random::Tester tester;
    tester.run(numSteps);
}
```

### 5.7 — When to use rules-based vs. simple tests

| Criteria | Simple tests | Rules-based |
|---|---|---|
| Few ports, no state machine | Preferred | Overkill |
| Multiple interacting ports | Possible | **Preferred** |
| Stateful behavior (counters, modes) | Difficult | **Preferred** |
| Need random / fuzzing coverage | Not possible | **Required** |

---

## 6 — Register UTs in CMakeLists.txt

```cmake
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/<Component>.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/<Component>TestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/<Component>Tester.cpp"
    # Add Rules/*.cpp and Scenarios/*.cpp if using rules-based testing
  DEPENDS
    STest      # Required for STest::Pick, STest::Rule, etc.
  UT_AUTO_HELPERS # Uses auto-generated test helper code from FPP
)
```

Notes:
- **`UT_AUTO_HELPERS`**: Use this flag (preferred). It tells the build
  system to autocode the connectPorts/initComponents helpers from the
  FPP model. Only omit this and hand-write helpers if you need custom
  port wiring (e.g., connecting only a subset of ports).
- **`DEPENDS STest`**: Always include when using `STest::Pick`,
  `STest::Rule`, `STest::Random`, or any scenario classes.

---

## 7 — Build and run

```bash
# Build UTs
fprime-util build --ut

# Run UTs
fprime-util check

# Run with coverage
fprime-util check --coverage
```

---

## 8 — Quality checklist

Before marking the test complete, verify:

- [ ] **Every input port** on the component has at least one test.
- [ ] **Every command** has a nominal test and an error-path test.
- [ ] **Every event** is asserted at least once (both emitted and
      not-emitted cases where relevant).
- [ ] **Every telemetry channel** is asserted after the action that
      updates it.
- [ ] **`clearHistory()`** is called at the start of each test action
      so assertions don't leak across steps.
- [ ] **`doDispatch()`** is called after every async port invocation
      or command send on active/queued components.
- [ ] **Helper functions** are used — raw `invoke_to_*` +
      `ASSERT_*` sequences do not appear directly in test methods.
- [ ] **`STest::Pick`** is used instead of hard-coded values for port
      numbers, IDs, and sizes where the specific value doesn't matter.
- [ ] **Boundary values** are tested (e.g., min/max buffer size, port
      index 0 and max).
- [ ] **Rules-based testing** is used for components with stateful
      behavior or multiple interacting ports, including a random
      scenario with ≥ 1000 steps.
- [ ] **No dynamic memory** in test code after construction (no
      `new`/`malloc`); use stack-allocated buffers.
- [ ] **`REQUIREMENT("...")`** macros link tests to requirements where
      applicable.
- [ ] **`COMMENT("...")`** macros describe each TEST() case.
- [ ] **Code compiles and all tests pass**: `fprime-util check`.
