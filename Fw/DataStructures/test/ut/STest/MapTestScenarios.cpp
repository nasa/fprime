// ======================================================================
// \title  MapTestScenarios.cpp
// \author Rob Bocchino
// \brief  Map test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/MapTestScenarios.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestRules.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace MapTest {

namespace Scenarios {

void clear(State& state) {
    Rules::insertNotFull.apply(state);
    ASSERT_EQ(state.map.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.map.getSize(), 0);
}

void find(State& state) {
    Rules::find.apply(state);
    state.useStoredKey = true;
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
    state.useStoredKey = true;
    for (FwSizeType i = 0; i < State::capacity; i++) {
        state.storedKey = static_cast<State::KeyType>(i);
        Rules::insertNotFull.apply(state);
    }
    state.useStoredKey = false;
    Rules::insertFull.apply(state);
}

void insertNotFull(State& state) {
    Rules::insertNotFull.apply(state);
}

void remove(State& state) {
    state.useStoredKey = true;
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

}  // namespace MapTest

}  // namespace Fw
