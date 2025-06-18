// ======================================================================
// \title  StackTestScenarios.cpp
// \author Rob Bocchino
// \brief  Stack test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/StackTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/StackTestScenarios.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace StackTest {

namespace Scenarios {

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps) {
    (void) name;
    (void) state;
    (void) maxNumSteps;
#if 0
    Rule* rules[] = {&Rules::enqueueOK, &Rules::enqueueFull,  &Rules::at,
                     &Rules::dequeueOK, &Rules::dequeueEmpty, &Rules::clear};
    STest::RandomScenario<State> scenario("RandomScenario", rules,
                                          sizeof(rules) / sizeof(STest::RandomScenario<State>*));
    STest::BoundedScenario<State> boundedScenario(name.toChar(), scenario, maxNumSteps);
    const U32 numSteps = boundedScenario.run(state);
    printf("Ran %u steps.\n", numSteps);
#endif
}

}  // namespace Scenarios

}  // namespace StackTest

}  // namespace Fw
