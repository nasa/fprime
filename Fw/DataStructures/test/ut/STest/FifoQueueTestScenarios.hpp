// ======================================================================
// \title  FifoQueueTestScenarios.hpp
// \author Rob Bocchino
// \brief  FifoQueue test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestRules.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace FifoQueueTest {

namespace Scenarios {

Rule* rules[] = {&Rules::enqueueOK, &Rules::enqueueFull, &Rules::clear};

void random(State& state, U32 maxNumSteps) {
    STest::RandomScenario<State> scenario("RandomScenario", rules,
                                          sizeof(rules) / sizeof(STest::RandomScenario<State>*));
    STest::BoundedScenario<State> boundedScenario("BoundedRandomScenario", scenario, maxNumSteps);
    const U32 numSteps = boundedScenario.run(state);
    printf("Ran %u steps.\n", numSteps);
}

}  // namespace Scenarios

}  // namespace FifoQueueTest

}  // namespace Fw
