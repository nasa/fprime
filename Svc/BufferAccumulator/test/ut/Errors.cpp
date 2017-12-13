// ====================================================================== 
// \title  Errors.hpp
// \author bocchino
// \brief  Test errors
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#include "Errors.hpp"

namespace ASTERIA {

  namespace Errors {

    // ----------------------------------------------------------------------
    // Tests 
    // ----------------------------------------------------------------------

    void Tester ::
      QueueFull(void)
    {

      Fw::Buffer buffer;

      // Go to Accumulate mode
      ASSERT_EQ(BufferAccumulatorMode::DRAIN, this->component.mode.e);
      this->sendCmd_SetMode(0, 0, BufferAccumulatorMode::ACCUMULATE);
      this->component.doDispatch();
      ASSERT_EQ(BufferAccumulatorMode::ACCUMULATE, this->component.mode.e);
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
      ASSERT_EVENTS_QueueFull_SIZE(1);

      // Send another buffer and expect no new event
      this->invoke_to_bufferSendInFill(0, buffer);
      this->component.doDispatch();
      ASSERT_EVENTS_SIZE(1);

      // Drain one buffer
      this->sendCmd_SetMode(0, 0, BufferAccumulatorMode::DRAIN);
      this->component.doDispatch();
      ASSERT_FROM_PORT_HISTORY_SIZE(1);
      ASSERT_from_bufferSendOutDrain_SIZE(1);
      ASSERT_from_bufferSendOutDrain(0, buffer);

      // Send another buffer and expect an event
      this->invoke_to_bufferSendInFill(0, buffer);
      this->component.doDispatch();
      ASSERT_EVENTS_SIZE(2);
      ASSERT_EVENTS_BufferAccepted_SIZE(1);

      // Drain one buffer
      this->sendCmd_SetMode(0, 0, BufferAccumulatorMode::DRAIN);
      this->component.doDispatch();
      ASSERT_FROM_PORT_HISTORY_SIZE(2);
      ASSERT_from_bufferSendOutDrain_SIZE(2);
      ASSERT_from_bufferSendOutDrain(1, buffer);

      // Send another buffer and expect no new event
      this->invoke_to_bufferSendInFill(0, buffer);
      this->component.doDispatch();
      ASSERT_EVENTS_SIZE(2);
      ASSERT_EVENTS_BufferAccepted_SIZE(1);

      // Send another buffer and expect an event
      this->invoke_to_bufferSendInFill(0, buffer);
      this->component.doDispatch();
      ASSERT_EVENTS_SIZE(3);
      ASSERT_EVENTS_QueueFull_SIZE(2);

    }

  }

}
