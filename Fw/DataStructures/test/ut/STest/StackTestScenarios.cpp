// ======================================================================
// \title  StackTestScenarios.cpp
// \author Rob Bocchino
// \brief  Stack test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/StackTestScenarios.hpp"
#include "Fw/DataStructures/test/ut/STest/StackTestRules.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace StackTest {

namespace Scenarios {

void at(State& state) {
    Rules::pushOK.apply(state);
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::at.apply(state);
    }
}

void clear(State& state) {
    Rules::pushOK.apply(state);
    Rules::clear.apply(state);
}

void peek(State& state) {
    Rules::pushOK.apply(state);
    Rules::pushOK.apply(state);
    Rules::peek.apply(state);
}

void popEmpty(State& state) {
    Rules::popEmpty.apply(state);
}

void popOK(State& state) {
    Rules::pushOK.apply(state);
    Rules::popOK.apply(state);
}

void pushFull(State& state) {
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::pushOK.apply(state);
    }
    Rules::pushFull.apply(state);
}

void pushOK(State& state) {
    Rules::pushOK.apply(state);
}

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps) {
    Rule* rules[] = {&Rules::pushOK, &Rules::pushFull, &Rules::at,   &Rules::peek,
                     &Rules::popOK,  &Rules::popEmpty, &Rules::clear};
    STest::RandomScenario<State> scenario("RandomScenario", rules,
                                          sizeof(rules) / sizeof(STest::RandomScenario<State>*));
    STest::BoundedScenario<State> boundedScenario(name.toChar(), scenario, maxNumSteps);
    const U32 numSteps = boundedScenario.run(state);
    printf("Ran %u steps.\n", numSteps);
}

}  // namespace Scenarios

}  // namespace StackTest

}  // namespace Fw
