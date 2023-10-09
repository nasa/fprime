// ======================================================================
// \title  Errors.hpp
// \author bocchino, mereweth
// \brief  Test errors
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Errors.hpp"

namespace Svc {

namespace Errors {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BufferAccumulatorTester ::PartialDrain() {
  ASSERT_EQ(BufferAccumulator_OpState::DRAIN, this->component.mode.e);
  this->sendCmd_BA_DrainBuffers(0, 0, 1, BufferAccumulator_BlockMode::BLOCK);
  this->component.doDispatch();  // will fail - we are still in
                                 // BufferAccumulator_OpState::DRAIN mode
  ASSERT_EQ(BufferAccumulator_OpState::DRAIN, this->component.mode.e);
  ASSERT_FROM_PORT_HISTORY_SIZE(0);
  ASSERT_EVENTS_BA_AlreadyDraining_SIZE(1);
  ASSERT_EQ(0u, this->component.numDrained);
  ASSERT_EQ(0u, this->component.numToDrain);

  this->sendCmd_BA_SetMode(0, 0, BufferAccumulator_OpState::ACCUMULATE);
  this->component.doDispatch();
  ASSERT_EQ(BufferAccumulator_OpState::ACCUMULATE, this->component.mode.e);
  ASSERT_FROM_PORT_HISTORY_SIZE(0);

  this->sendCmd_BA_DrainBuffers(0, 0, 10, BufferAccumulator_BlockMode::BLOCK);
  this->component.doDispatch();  // will succeed - now we are in ACCUMULATE
  ASSERT_EQ(BufferAccumulator_OpState::ACCUMULATE, this->component.mode.e);
  ASSERT_FROM_PORT_HISTORY_SIZE(
      0);  // would be first buffer out, but we are empty
  ASSERT_EVENTS_BA_DrainStalled_SIZE(1);
  ASSERT_EVENTS_BA_DrainStalled(0, 0u, 10u);
  ASSERT_EVENTS_BA_PartialDrainDone_SIZE(0);  // partial drain not done
  ASSERT_EQ(true, this->component.send);
  ASSERT_EQ(0u, this->component.numDrained);
  ASSERT_EQ(10u, this->component.numToDrain);

  this->sendCmd_BA_DrainBuffers(0, 0, 1, BufferAccumulator_BlockMode::BLOCK);
  this->component
      .doDispatch();  // will fail - we are still doing a partial drain
  ASSERT_EVENTS_BA_StillDraining_SIZE(1);
  ASSERT_EVENTS_BA_StillDraining(0, 0u, 10u);
  ASSERT_EVENTS_BA_PartialDrainDone_SIZE(0);  // partial drain not done
  ASSERT_EQ(BufferAccumulator_OpState::ACCUMULATE, this->component.mode.e);
  ASSERT_FROM_PORT_HISTORY_SIZE(0);
  ASSERT_EQ(true, this->component.send);
  ASSERT_EQ(0u, this->component.numDrained);
  ASSERT_EQ(10u, this->component.numToDrain);
}

void BufferAccumulatorTester ::QueueFull() {
  U8* data = new U8[10];
  const U32 size = 10;
  Fw::Buffer buffer(data, size);

  // Go to Accumulate mode
  ASSERT_EQ(BufferAccumulator_OpState::DRAIN, this->component.mode.e);
  this->sendCmd_BA_SetMode(0, 0, BufferAccumulator_OpState::ACCUMULATE);
  this->component.doDispatch();
  ASSERT_EQ(BufferAccumulator_OpState::ACCUMULATE, this->component.mode.e);
  ASSERT_FROM_PORT_HISTORY_SIZE(0);

  // Fill up the buffer queue
  for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
    this->invoke_to_bufferSendInFill(0, buffer);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
  }

  // Send another buffer and expect an event
  this->invoke_to_bufferSendInFill(0, buffer);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(1);
  ASSERT_EVENTS_BA_QueueFull_SIZE(1);

  // Send another buffer and expect no new event
  this->invoke_to_bufferSendInFill(0, buffer);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(1);

  // Drain one buffer
  this->sendCmd_BA_SetMode(0, 0, BufferAccumulator_OpState::DRAIN);
  this->component.doDispatch();
  ASSERT_FROM_PORT_HISTORY_SIZE(1);
  ASSERT_from_bufferSendOutDrain_SIZE(1);
  ASSERT_from_bufferSendOutDrain(0, buffer);

  // Send another buffer and expect an event
  this->invoke_to_bufferSendInFill(0, buffer);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(2);
  ASSERT_EVENTS_BA_BufferAccepted_SIZE(1);

  // Return the original buffer in order to drain one buffer
  this->invoke_to_bufferSendInReturn(0, buffer);
  this->component.doDispatch();
  ASSERT_FROM_PORT_HISTORY_SIZE(3);
  ASSERT_from_bufferSendOutDrain_SIZE(2);
  ASSERT_from_bufferSendOutDrain(1, buffer);

  // Send another buffer and expect no new event
  this->invoke_to_bufferSendInFill(0, buffer);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(2);
  ASSERT_EVENTS_BA_BufferAccepted_SIZE(1);

  // Send another buffer and expect an event
  this->invoke_to_bufferSendInFill(0, buffer);
  this->component.doDispatch();
  ASSERT_EVENTS_SIZE(3);
  ASSERT_EVENTS_BA_QueueFull_SIZE(2);

  delete[] data;
}

}  // namespace Errors

}  // namespace Svc
