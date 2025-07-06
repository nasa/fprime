// ======================================================================
// \title  StackTestScenarios.hpp
// \author Rob Bocchino
// \brief  Stack test scenarios
// ======================================================================

#ifndef StackTestScenarios_HPP
#define StackTestScenarios_HPP

#include "Fw/DataStructures/test/ut/STest/StackTestState.hpp"

namespace Fw {

namespace StackTest {

namespace Scenarios {

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps);

}  // namespace Scenarios

}  // namespace StackTest

}  // namespace Fw

#endif
