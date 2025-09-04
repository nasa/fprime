// ======================================================================
// \title  SetTestScenarios.cpp
// \author Rob Bocchino
// \brief  Set test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/SetTestScenarios.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestRules.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace SetTest {

namespace Scenarios {

void clear(State& state) {
    Rules::insertNotFull.apply(state);
    ASSERT_EQ(state.set.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.set.getSize(), 0);
}

void find(State& state) {
    Rules::find.apply(state);
    state.useStoredElement = true;
    Rules::insertNotFull.apply(state);
    Rules::find.apply(state);
}

void findExisting(State& state) {
    Rules::insertNotFull.apply(state);
    Rules::findExisting.apply(state);
}

void insertExisting(State& state) {
    Rules::insertNotFull.apply(state);
    Rules::insertExisting.apply(state);
}

void insertFull(State& state) {
    state.useStoredElement = true;
    for (FwSizeType i = 0; i < State::capacity; i++) {
        state.storedElement = static_cast<State::ElementType>(i);
        Rules::insertNotFull.apply(state);
    }
    state.useStoredElement = false;
    Rules::insertFull.apply(state);
}

void insertNotFull(State& state) {
    Rules::insertNotFull.apply(state);
}

void remove(State& state) {
    state.useStoredElement = true;
    Rules::insertNotFull.apply(state);
    Rules::remove.apply(state);
    Rules::remove.apply(state);
}

void removeExisting(State& state) {
    Rules::insertNotFull.apply(state);
    Rules::removeExisting.apply(state);
}

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps) {
    Rule* rules[] = {&Rules::clear,      &Rules::find,          &Rules::findExisting, &Rules::insertExisting,
                     &Rules::insertFull, &Rules::insertNotFull, &Rules::remove,       &Rules::removeExisting};
    STest::RandomScenario<State> scenario("RandomScenario", rules,
                                          sizeof(rules) / sizeof(STest::RandomScenario<State>*));
    STest::BoundedScenario<State> boundedScenario(name.toChar(), scenario, maxNumSteps);
    const U32 numSteps = boundedScenario.run(state);
    printf("Ran %u steps.\n", numSteps);
}

}  // namespace Scenarios

}  // namespace SetTest

}  // namespace Fw
