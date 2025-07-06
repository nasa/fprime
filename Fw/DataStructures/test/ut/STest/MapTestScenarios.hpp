// ======================================================================
// \title  MapTestScenarios.hpp
// \author Rob Bocchino
// \brief  Map test scenarios
// ======================================================================

#ifndef MapTestScenarios_HPP
#define MapTestScenarios_HPP

#include "Fw/DataStructures/test/ut/STest/MapTestState.hpp"

namespace Fw {

namespace MapTest {

namespace Scenarios {

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps);

}  // namespace Scenarios

}  // namespace MapTest

}  // namespace Fw

#endif
