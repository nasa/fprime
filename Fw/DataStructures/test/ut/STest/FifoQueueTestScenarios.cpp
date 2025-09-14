// ======================================================================
// \title  FifoQueueTestScenarios.cpp
// \author Rob Bocchino
// \brief  FifoQueue test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestScenarios.hpp"
#include "Fw/DataStructures/test/ut/STest/FifoQueueTestRules.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace FifoQueueTest {

namespace Scenarios {

void at(State& state) {
    Rules::enqueueOK.apply(state);
    Rules::at.apply(state);
}

void clear(State& state) {
    Rules::enqueueOK.apply(state);
    ASSERT_EQ(state.queue.getSize(), 1);
    Rules::clear.apply(state);
    ASSERT_EQ(state.queue.getSize(), 0);
}

void dequeueEmpty(State& state) {
    Rules::dequeueEmpty.apply(state);
}

void dequeueOK(State& state) {
    Rules::enqueueOK.apply(state);
    Rules::dequeueOK.apply(state);
}

void enqueueFull(State& state) {
    for (FwSizeType i = 0; i < State::capacity; i++) {
        Rules::enqueueOK.apply(state);
    }
    Rules::enqueueFull.apply(state);
}

void enqueueOK(State& state) {
    Rules::enqueueOK.apply(state);
}

void peek(State& state) {
    Rules::peek.apply(state);
}

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps) {
    Rule* rules[] = {&Rules::at,          &Rules::clear,     &Rules::dequeueEmpty, &Rules::dequeueOK,
                     &Rules::enqueueFull, &Rules::enqueueOK, &Rules::peek};
    STest::RandomScenario<State> scenario("RandomScenario", rules,
                                          sizeof(rules) / sizeof(STest::RandomScenario<State>*));
    STest::BoundedScenario<State> boundedScenario(name.toChar(), scenario, maxNumSteps);
    const U32 numSteps = boundedScenario.run(state);
    printf("Ran %u steps.\n", numSteps);
}

}  // namespace Scenarios

}  // namespace FifoQueueTest

}  // namespace Fw
