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

Use STest rules-based framework when the component has multiple
interacting ports or stateful behavior.

### 5.1 — File layout

```
<Component>/test/ut/
    <Component>Tester.{hpp,cpp}
    <Component>TestMain.cpp
    AbstractState.hpp
    TestState/TestState.hpp
    Rules/Rules.hpp
    Rules/<RuleGroup>.{hpp,cpp}
    Rules/Testers.{hpp,cpp}
    Scenarios/Random.{hpp,cpp}
```

### 5.2 — AbstractState

Model observable component state as a plain C++ struct:

```cpp
class AbstractState {
  public:
    static constexpr FwSizeType MAX_BUFFER_SIZE = 1024;
    enum class BufferGetStatus { VALID, INVALID };
    BufferGetStatus bufferGetStatus{BufferGetStatus::VALID};
    TestUtils::OnChangeChannel<U32> NumSuccessfulAllocations{0};
};
```

Store as `AbstractState abstractState;` in the Tester.

### 5.3 — TestState

Extends Tester; declares precondition/action pairs per rule:

```cpp
#define TEST_STATE_DEF_RULE(GROUP_NAME, RULE_NAME)          \
    bool precondition__##GROUP_NAME##__##RULE_NAME() const; \
    void action__##GROUP_NAME##__##RULE_NAME();

class TestState : public <Component>Tester {
  public:
    TEST_STATE_DEF_RULE(PortGroupA, NominalCase)
    TEST_STATE_DEF_RULE(PortGroupA, ErrorCase)
};
```

### 5.4 — Rules.hpp

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

RULES_DEF_RULE(PortGroupA, NominalCase)
RULES_DEF_RULE(PortGroupA, ErrorCase)
```

### 5.5 — Rule implementation

```cpp
bool TestState::precondition__PortGroupA__NominalCase() const {
    return this->abstractState.bufferGetStatus ==
           AbstractState::BufferGetStatus::VALID;
}

void TestState::action__PortGroupA__NominalCase() {
    this->clearHistory();
    const auto portNum = static_cast<FwIndexType>(
        STest::Pick::startLength(0, NumPorts));
    this->invoke_to_somePort(portNum, args...);
    this->doDispatch();
    ASSERT_EVENTS_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    ++this->abstractState.NumSuccessfulOps.value;
}
```

### 5.6 — Random scenario

```cpp
void Tester::run(U32 maxNumSteps) {
    STest::Rule<TestState>* rules[] = { &ruleA, &ruleB, &ruleC };
    STest::RandomScenario<TestState> scenario(
        "Random", rules,
        sizeof(rules) / sizeof(STest::Rule<TestState>*));
    STest::BoundedScenario<TestState> bounded(
        "BoundedRandom", scenario, maxNumSteps);
    const U32 numSteps = bounded.run(this->testState);
    printf("Ran %u steps.\n", numSteps);
}
```

Register in TestMain:
```cpp
TEST(Scenarios, Random) {
    COMMENT("Random scenario with all rules.");
    Scenarios::Random::Tester tester;
    tester.run(10000);
}
```

### 5.7 — When to use rules-based vs. simple

| Criteria | Simple | Rules-based |
|---|---|---|
| Few ports, no state machine | Preferred | Overkill |
| Multiple interacting ports | Possible | **Preferred** |
| Stateful behavior | Difficult | **Preferred** |
| Need random / fuzzing coverage | Not possible | **Required** |

---

## 6 — CMakeLists.txt

```cmake
register_fprime_ut(
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/<Component>.fpp"
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/<Component>TestMain.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test/ut/<Component>Tester.cpp"
    # Add Rules/*.cpp and Scenarios/*.cpp for rules-based tests
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
