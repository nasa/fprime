// ======================================================================
// \title  TestState.hpp
// \author Rob Bocchino
// \brief  Test state for testing DpWriter
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef Svc_TestState_HPP
#define Svc_TestState_HPP

#include "Svc/DpWriter/test/ut/DpWriterTester.hpp"

#define TEST_STATE_DEF_RULE(GROUP_NAME, RULE_NAME)          \
    bool precondition__##GROUP_NAME##__##RULE_NAME() const; \
    void action__##GROUP_NAME##__##RULE_NAME();

namespace Svc {

class TestState : public DpWriterTester {
  public:
    // ----------------------------------------------------------------------
    // Rule definitions
    // ----------------------------------------------------------------------

    TEST_STATE_DEF_RULE(BufferSendIn, BufferTooSmallForData)
    TEST_STATE_DEF_RULE(BufferSendIn, BufferTooSmallForPacket)
    TEST_STATE_DEF_RULE(BufferSendIn, FileOpenError)
    TEST_STATE_DEF_RULE(BufferSendIn, FileWriteError)
    TEST_STATE_DEF_RULE(BufferSendIn, InvalidBuffer)
    TEST_STATE_DEF_RULE(BufferSendIn, InvalidHeader)
    TEST_STATE_DEF_RULE(BufferSendIn, InvalidHeaderHash)
    TEST_STATE_DEF_RULE(BufferSendIn, OK)
    TEST_STATE_DEF_RULE(CLEAR_EVENT_THROTTLE, OK)
    TEST_STATE_DEF_RULE(FileOpenStatus, Error)
    TEST_STATE_DEF_RULE(FileOpenStatus, OK)
    TEST_STATE_DEF_RULE(FileWriteStatus, Error)
    TEST_STATE_DEF_RULE(FileWriteStatus, OK)
    TEST_STATE_DEF_RULE(SchedIn, OK)
};

}  // namespace Svc

#endif
