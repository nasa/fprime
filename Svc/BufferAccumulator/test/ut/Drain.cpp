// ======================================================================
// \title  Drain.hpp
// \author bocchino, mereweth
// \brief  Test drain mode
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Drain.hpp"

#include <cstring>
#include <sys/time.h>

namespace Svc {

namespace Drain {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void BufferAccumulatorTester ::OK() {
  ASSERT_EQ(BufferAccumulator_OpState::DRAIN, this->component.mode.e);
  Fw::Buffer buffers[MAX_NUM_BUFFERS];
  // Buffer needs a valid pointer
  U8* data = new U8[10];
  const U32 size = 10;
  for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
    ASSERT_from_bufferSendOutDrain_SIZE(i);
    const U32 bufferID = i;
    Fw::Buffer b(data, size, bufferID);
    buffers[i] = b;
    this->invoke_to_bufferSendInFill(0, buffers[i]);
    this->component.doDispatch();
    ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
    ASSERT_from_bufferSendOutDrain(i, buffers[i]);
    this->invoke_to_bufferSendInReturn(0, buffers[i]);
    this->component.doDispatch();
    ASSERT_from_bufferSendOutReturn(i, buffers[i]);
  }

  delete[] data;
}

void BufferAccumulatorTester ::PartialDrainOK() {
  this->sendCmd_BA_SetMode(0, 0, BufferAccumulator_OpState::ACCUMULATE);
  this->component.doDispatch();
  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, BufferAccumulator::OPCODE_BA_SETMODE, 0,
                      Fw::CmdResponse::OK);
  ASSERT_EQ(BufferAccumulator_OpState::ACCUMULATE, this->component.mode.e);
  ASSERT_FROM_PORT_HISTORY_SIZE(0);

  Fw::Buffer buffers[MAX_NUM_BUFFERS];
  U8* data = new U8[10];
  const U32 size = 10;
  for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
    const U32 bufferID = i;
    Fw::Buffer b(data, size, bufferID);
    buffers[i] = b;
    this->invoke_to_bufferSendInFill(0, buffers[i]);
    this->component.doDispatch();

    this->sendCmd_BA_DrainBuffers(0, 0, 1, BufferAccumulator_BlockMode::BLOCK);
    this->component.doDispatch();
    ASSERT_EVENTS_BA_PartialDrainDone_SIZE(i + 1);
    ASSERT_EVENTS_BA_PartialDrainDone(i, 1u);
    // + 1 for first BufferAccumulator_OpState::ACCUMULATE command; + 1 for
    // buffer drained immediately
    ASSERT_CMD_RESPONSE_SIZE(i + 2);
    ASSERT_CMD_RESPONSE(i + 1, BufferAccumulator::OPCODE_BA_DRAINBUFFERS, 0,
                        Fw::CmdResponse::OK);
    // check that one buffer drained
    ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
    ASSERT_from_bufferSendOutDrain(i, buffers[i]);

    this->invoke_to_bufferSendInReturn(0, buffers[i]);
    this->component.doDispatch();
    ASSERT_from_bufferSendOutReturn(i, buffers[i]);

    ASSERT_EVENTS_BA_PartialDrainDone_SIZE(i + 1);
    ASSERT_EVENTS_BA_PartialDrainDone(i, 1u);
    // + 1 for first BufferAccumulator_OpState::ACCUMULATE command; + 1 for
    // buffer drained immediately
    ASSERT_CMD_RESPONSE_SIZE(i + 2);
    // check that ONLY one buffer drained
    ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
    ASSERT_from_bufferSendOutDrain(i, buffers[i]);
  }

  this->clearHistory();
  // refill buffers
  for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
    const U32 bufferID = i;
    Fw::Buffer b(data, size, bufferID);
    buffers[i] = b;
    this->invoke_to_bufferSendInFill(0, buffers[i]);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
  }

  ASSERT_CMD_RESPONSE_SIZE(0);
  for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
    ASSERT_from_bufferSendOutDrain_SIZE(i);

    this->sendCmd_BA_DrainBuffers(0, 0, 1, BufferAccumulator_BlockMode::BLOCK);
    this->component.doDispatch();
    ASSERT_CMD_RESPONSE_SIZE(i + 1);
    ASSERT_CMD_RESPONSE(i, BufferAccumulator::OPCODE_BA_DRAINBUFFERS, 0,
                        Fw::CmdResponse::OK);

    const U32 bufferID = i;
    Fw::Buffer b(data, size, bufferID);
    buffers[i] = b;

    // check that one buffer drained
    ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
    ASSERT_from_bufferSendOutDrain(i, buffers[i]);
    this->invoke_to_bufferSendInReturn(0, buffers[i]);
    this->component.doDispatch();
    ASSERT_from_bufferSendOutReturn(i, buffers[i]);

    ASSERT_EVENTS_BA_PartialDrainDone_SIZE(i + 1);
    ASSERT_EVENTS_BA_PartialDrainDone(i, 1u);

    // check that ONLY one buffer drained
    ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
    ASSERT_from_bufferSendOutDrain(i, buffers[i]);
    ASSERT_CMD_RESPONSE_SIZE(i + 1);
  }

  this->clearHistory();
  // refill buffers
  for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
    const U32 bufferID = i;
    Fw::Buffer b(data, size, bufferID);
    buffers[i] = b;
    this->invoke_to_bufferSendInFill(0, buffers[i]);
    this->component.doDispatch();
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
  }

  ASSERT_EQ(BufferAccumulator_OpState::ACCUMULATE, this->component.mode.e);
  ASSERT_FROM_PORT_HISTORY_SIZE(0);
  ASSERT_EQ(0u, this->component.numDrained);
  ASSERT_EQ(0u, this->component.numToDrain);

  ASSERT_EVENTS_BA_PartialDrainDone_SIZE(0);
  this->sendCmd_BA_DrainBuffers(0, 0, MAX_NUM_BUFFERS,
                                BufferAccumulator_BlockMode::BLOCK);
  this->component.doDispatch();
  ASSERT_CMD_RESPONSE_SIZE(0);

  for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
    const U32 bufferID = i;
    Fw::Buffer b(data, size, bufferID);
    buffers[i] = b;

    if (i + 1 < MAX_NUM_BUFFERS) {
      ASSERT_EQ(i + 1, this->component.numDrained);
      ASSERT_EQ(MAX_NUM_BUFFERS, this->component.numToDrain);
      ASSERT_EVENTS_BA_PartialDrainDone_SIZE(0);
    }

    // check that one buffer drained
    ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
    ASSERT_from_bufferSendOutDrain(i, buffers[i]);
    this->invoke_to_bufferSendInReturn(0, buffers[i]);
    this->component.doDispatch();
    ASSERT_from_bufferSendOutReturn(i, buffers[i]);
  }

  ASSERT_CMD_RESPONSE_SIZE(1);
  ASSERT_CMD_RESPONSE(0, BufferAccumulator::OPCODE_BA_DRAINBUFFERS, 0,
                      Fw::CmdResponse::OK);
  ASSERT_EVENTS_BA_PartialDrainDone_SIZE(1);
  ASSERT_EVENTS_BA_PartialDrainDone(0, MAX_NUM_BUFFERS);

  delete[] data;
}

}  // namespace Drain

}  // namespace Svc
