// ======================================================================
// \title  SetTestScenarios.hpp
// \author Rob Bocchino
// \brief  Set test scenarios
// ======================================================================

#ifndef SetTestScenarios_HPP
#define SetTestScenarios_HPP

#include "Fw/DataStructures/test/ut/STest/SetTestState.hpp"

namespace Fw {

namespace SetTest {

namespace Scenarios {

void clear(State& state);

void find(State& state);

void findExisting(State& state);

void insertExisting(State& state);

void insertFull(State& state);

void insertNotFull(State& state);

void remove(State& state);

void removeExisting(State& state);

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps);

}  // namespace Scenarios

}  // namespace SetTest

}  // namespace Fw

#endif
