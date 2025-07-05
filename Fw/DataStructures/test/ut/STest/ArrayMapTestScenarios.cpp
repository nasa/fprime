// ======================================================================
// \title  ArrayMapTestScenarios.cpp
// \author Rob Bocchino
// \brief  ArrayMap test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/ArrayMapTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/ArrayMapTestScenarios.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace ArrayMapTest {

namespace Scenarios {

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps) {
    (void)name;
    (void)state;
    (void)maxNumSteps;
    Rule* rules[] = {
      //&Rules::at,
      &Rules::clear,
      &Rules::insertFull,
      &Rules::insertNotFull
      //&Rules::remove,
      //&Rules::removeExisting
    };
    STest::RandomScenario<State> scenario("RandomScenario", rules,
                                          sizeof(rules) / sizeof(STest::RandomScenario<State>*));
    STest::BoundedScenario<State> boundedScenario(name.toChar(), scenario, maxNumSteps);
    const U32 numSteps = boundedScenario.run(state);
    printf("Ran %u steps.\n", numSteps);
}

}  // namespace Scenarios

}  // namespace ArrayMapTest

}  // namespace Fw
