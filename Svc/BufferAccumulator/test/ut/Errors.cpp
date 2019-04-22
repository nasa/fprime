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

    void Tester ::
      QueueFull(void)
    {

      Fw::Buffer buffer;

      // Go to Accumulate mode
      ASSERT_EQ(BufferAccumulator::DRAIN, this->component.mode);
      this->sendCmd_BA_SetMode(0, 0, BufferAccumulator::ACCUMULATE);
      this->component.doDispatch();
      ASSERT_EQ(BufferAccumulator::ACCUMULATE, this->component.mode);
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
      this->sendCmd_BA_SetMode(0, 0, BufferAccumulator::DRAIN);
      this->component.doDispatch();
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      ASSERT_from_bufferSendOutDrain_SIZE(1);
      ASSERT_from_bufferSendOutDrain(0, buffer);

      // Send another buffer and expect an event
      this->invoke_to_bufferSendInFill(0, buffer);
      this->component.doDispatch();
      ASSERT_EVENTS_SIZE(2);
      ASSERT_EVENTS_BA_BufferAccepted_SIZE(1);

      // Drain one buffer
      this->sendCmd_BA_SetMode(0, 0, BufferAccumulator::DRAIN);
      this->component.doDispatch();
      ASSERT_FROM_PORT_HISTORY_SIZE(2);
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

    }

  }

}
