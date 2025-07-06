// ======================================================================
// \title  SetTestScenarios.cpp
// \author Rob Bocchino
// \brief  Set test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/SetTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/SetTestScenarios.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace SetTest {

namespace Scenarios {

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps) {
    Rule* rules[] = {
        &Rules::clear,
        &Rules::find,
        &Rules::findExisting,
        &Rules::insertFull, &Rules::insertNotFull,
        &Rules::remove,
        &Rules::removeExisting
    };
    STest::RandomScenario<State> scenario("RandomScenario", rules,
                                          sizeof(rules) / sizeof(STest::RandomScenario<State>*));
    STest::BoundedScenario<State> boundedScenario(name.toChar(), scenario, maxNumSteps);
    const U32 numSteps = boundedScenario.run(state);
    printf("Ran %u steps.\n", numSteps);
}

}  // namespace Scenarios

}  // namespace SetTest

}  // namespace Fw
