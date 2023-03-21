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

#include <string.h>

#include "Drain.hpp"

#include <stdio.h> // TODO(mereweth@jpl.nasa.gov) - remove the debug prints
#include <sys/time.h>

//#define DEBUG_PRINT(x,...) printf(x,##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(x,...)

namespace Svc {

  namespace Drain {

    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    void Tester ::
      OK(void)
    {
      ASSERT_EQ(BufferAccumulator::DRAIN, this->component.mode);
      Fw::Buffer buffers[MAX_NUM_BUFFERS];
      const U32 managerID = 42;
      const U64 data = 0;
      const U32 size = 10;
      for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
        ASSERT_from_bufferSendOutDrain_SIZE(i);
        const U32 bufferID = i;
        Fw::Buffer b(managerID, bufferID, data, size);
        buffers[i] = b;
        this->invoke_to_bufferSendInFill(0, buffers[i]);
        this->component.doDispatch();
        ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
        ASSERT_from_bufferSendOutDrain(i, buffers[i]);
        this->invoke_to_bufferSendInReturn(0, buffers[i]);
        this->component.doDispatch();
        ASSERT_from_bufferSendOutReturn(i, buffers[i]);
      }
    }

    void Tester ::
      PartialDrainOK(void)
    {
        this->sendCmd_BA_SetMode(0, 0, BufferAccumulator::ACCUMULATE);
        this->component.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(1);
        ASSERT_CMD_RESPONSE(0, BufferAccumulator::OPCODE_BA_SETMODE, 0,
                            Fw::COMMAND_OK);
        ASSERT_EQ(BufferAccumulator::ACCUMULATE, this->component.mode);
        ASSERT_FROM_PORT_HISTORY_SIZE(0);

        Fw::Buffer buffers[MAX_NUM_BUFFERS];
        const U32 managerID = 42;
        const U64 data = 0;
        const U32 size = 10;
        for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
            const U32 bufferID = i;
            Fw::Buffer b(managerID, bufferID, data, size);
            buffers[i] = b;
            this->invoke_to_bufferSendInFill(0, buffers[i]);
            this->component.doDispatch();

            DEBUG_PRINT("PartialDrainOK fill/draining buffer %d, one by one\n", i);

            this->sendCmd_BA_DrainBuffers(0, 0, 1, BufferAccumulator::BLOCK);
            this->component.doDispatch();
            ASSERT_EVENTS_BA_PartialDrainDone_SIZE(i + 1);
            ASSERT_EVENTS_BA_PartialDrainDone(i, 1u);
            // + 1 for first ACCUMULATE command; + 1 for buffer drained immediately
            ASSERT_CMD_RESPONSE_SIZE(i + 2);
            ASSERT_CMD_RESPONSE(i + 1,
                                BufferAccumulator::OPCODE_BA_DRAINBUFFERS, 0,
                                Fw::COMMAND_OK);
            // check that one buffer drained
            ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
            ASSERT_from_bufferSendOutDrain(i, buffers[i]);

            this->invoke_to_bufferSendInReturn(0, buffers[i]);
            this->component.doDispatch();
            ASSERT_from_bufferSendOutReturn(i, buffers[i]);

            DEBUG_PRINT("PartialDrainOK checking that only one buffer fill/drained %d, one by one\n", i);
            ASSERT_EVENTS_BA_PartialDrainDone_SIZE(i + 1);
            ASSERT_EVENTS_BA_PartialDrainDone(i, 1u);
            // + 1 for first ACCUMULATE command; + 1 for buffer drained immediately
            ASSERT_CMD_RESPONSE_SIZE(i + 2);
            // check that ONLY one buffer drained
            ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
            ASSERT_from_bufferSendOutDrain(i, buffers[i]);
        }

        this->clearHistory();
        // refill buffers
        for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
            const U32 bufferID = i;
            Fw::Buffer b(managerID, bufferID, data, size);
            buffers[i] = b;
            this->invoke_to_bufferSendInFill(0, buffers[i]);
            this->component.doDispatch();
            ASSERT_FROM_PORT_HISTORY_SIZE(0);
        }

        ASSERT_CMD_RESPONSE_SIZE(0);
        for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
            DEBUG_PRINT("PartialDrainOK draining buffer %d, one by one\n", i);
            ASSERT_from_bufferSendOutDrain_SIZE(i);

            this->sendCmd_BA_DrainBuffers(0, 0, 1, BufferAccumulator::BLOCK);
            this->component.doDispatch();
            ASSERT_CMD_RESPONSE_SIZE(i + 1);
            ASSERT_CMD_RESPONSE(i, BufferAccumulator::OPCODE_BA_DRAINBUFFERS, 0,
                                Fw::COMMAND_OK);

            const U32 bufferID = i;
            Fw::Buffer b(managerID, bufferID, data, size);
            buffers[i] = b;

            // check that one buffer drained
            ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
            ASSERT_from_bufferSendOutDrain(i, buffers[i]);
            this->invoke_to_bufferSendInReturn(0, buffers[i]);
            this->component.doDispatch();
            ASSERT_from_bufferSendOutReturn(i, buffers[i]);

            ASSERT_EVENTS_BA_PartialDrainDone_SIZE(i + 1);
            ASSERT_EVENTS_BA_PartialDrainDone(i, 1u);

            DEBUG_PRINT("PartialDrainOK checking that only one buffer drained %d, one by one\n", i);
            // check that ONLY one buffer drained
            ASSERT_from_bufferSendOutDrain_SIZE(i + 1);
            ASSERT_from_bufferSendOutDrain(i, buffers[i]);
            ASSERT_CMD_RESPONSE_SIZE(i + 1);
        }

        this->clearHistory();
        // refill buffers
        for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
            const U32 bufferID = i;
            Fw::Buffer b(managerID, bufferID, data, size);
            buffers[i] = b;
            this->invoke_to_bufferSendInFill(0, buffers[i]);
            this->component.doDispatch();
            ASSERT_FROM_PORT_HISTORY_SIZE(0);
        }

        DEBUG_PRINT("PartialDrainOK draining all buffers at once\n");
        ASSERT_EQ(BufferAccumulator::ACCUMULATE, this->component.mode);
        ASSERT_FROM_PORT_HISTORY_SIZE(0);
        ASSERT_EQ(0u, this->component.numDrained);
        ASSERT_EQ(0u, this->component.numToDrain);

        ASSERT_EVENTS_BA_PartialDrainDone_SIZE(0);
        this->sendCmd_BA_DrainBuffers(0, 0, MAX_NUM_BUFFERS, BufferAccumulator::BLOCK);
        this->component.doDispatch();
        ASSERT_CMD_RESPONSE_SIZE(0);

        for (U32 i = 0; i < MAX_NUM_BUFFERS; ++i) {
            DEBUG_PRINT("PartialDrainOK draining all; returning %d, one by one\n", i);

            const U32 bufferID = i;
            Fw::Buffer b(managerID, bufferID, data, size);
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
                            Fw::COMMAND_OK);
        ASSERT_EVENTS_BA_PartialDrainDone_SIZE(1);
        ASSERT_EVENTS_BA_PartialDrainDone(0, MAX_NUM_BUFFERS);
    }

  } // namespace Drain

} // namespace Svc
