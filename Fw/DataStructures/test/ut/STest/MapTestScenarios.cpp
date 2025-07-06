// ======================================================================
// \title  MapTestScenarios.cpp
// \author Rob Bocchino
// \brief  Map test scenarios
// ======================================================================

#include "Fw/DataStructures/test/ut/STest/MapTestRules.hpp"
#include "Fw/DataStructures/test/ut/STest/MapTestScenarios.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Fw {

namespace MapTest {

namespace Scenarios {

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps) {
    Rule* rules[] = {
      //&Rules::at,
      &Rules::clear,
      &Rules::findExisting,
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

}  // namespace MapTest

}  // namespace Fw
