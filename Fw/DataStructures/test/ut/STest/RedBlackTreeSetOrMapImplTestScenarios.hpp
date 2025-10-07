// ======================================================================
// \title  RedBlackTreeSetOrMapImplTestScenarios.hpp
// \author Rob Bocchino
// \brief  RedBlackTreeSetOrMapImpl test scenarios
// ======================================================================

#ifndef RedBlackTreeSetOrMapImplTestScenarios_HPP
#define RedBlackTreeSetOrMapImplTestScenarios_HPP

#include "Fw/DataStructures/test/ut/STest/RedBlackTreeSetOrMapImplTestState.hpp"

namespace Fw {

namespace RedBlackTreeSetOrMapImplTest {

namespace Scenarios {

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps);

}  // namespace Scenarios

}  // namespace RedBlackTreeSetOrMapImplTest

}  // namespace Fw

#endif
