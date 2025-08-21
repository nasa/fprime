// ======================================================================
// \title  FifoQueueTestScenarios.hpp
// \author Rob Bocchino
// \brief  FifoQueue test scenarios
// ======================================================================

#ifndef FifoQueueTestScenarios_HPP
#define FifoQueueTestScenarios_HPP

#include "Fw/DataStructures/test/ut/STest/FifoQueueTestState.hpp"

namespace Fw {

namespace FifoQueueTest {

namespace Scenarios {

void at(State& state);

void clear(State& state);

void dequeueEmpty(State& state);

void dequeueOK(State& state);

void enqueueFull(State& state);

void enqueueOK(State& state);

void peek(State& state);

void random(const Fw::StringBase& name, State& state, U32 maxNumSteps);

}  // namespace Scenarios

}  // namespace FifoQueueTest

}  // namespace Fw

#endif
