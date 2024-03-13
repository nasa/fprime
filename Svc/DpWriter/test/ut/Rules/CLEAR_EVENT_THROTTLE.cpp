// ======================================================================
// \title  CLEAR_EVENT_THROTTLE.cpp
// \author Rob Bocchino
// \brief  CLEAR_EVENT_THROTTLE class implementation
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "Svc/DpWriter/test/ut/Rules/CLEAR_EVENT_THROTTLE.hpp"
#include "Svc/DpWriter/test/ut/Rules/Testers.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Rule definitions
// ----------------------------------------------------------------------

bool TestState ::precondition__CLEAR_EVENT_THROTTLE__OK() const {
    return true;
}

void TestState ::action__CLEAR_EVENT_THROTTLE__OK() {
    // Clear history
    this->clearHistory();
    // Send the command
    const FwEnumStoreType instance = static_cast<FwEnumStoreType>(STest::Pick::any());
    const U32 cmdSeq = STest::Pick::any();
    this->sendCmd_CLEAR_EVENT_THROTTLE(instance, cmdSeq);
    this->component.doDispatch();
    // Check the command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpWriterComponentBase::OPCODE_CLEAR_EVENT_THROTTLE, cmdSeq, Fw::CmdResponse::OK);
    // Check the concrete state
    ASSERT_EQ(this->component.DpWriterComponentBase::m_BufferTooSmallForDataThrottle, 0);
    ASSERT_EQ(this->component.DpWriterComponentBase::m_BufferTooSmallForPacketThrottle, 0);
    ASSERT_EQ(this->component.DpWriterComponentBase::m_FileOpenErrorThrottle, 0);
    ASSERT_EQ(this->component.DpWriterComponentBase::m_FileWriteErrorThrottle, 0);
    ASSERT_EQ(this->component.DpWriterComponentBase::m_InvalidBufferThrottle, 0);
    ASSERT_EQ(this->component.DpWriterComponentBase::m_InvalidHeaderHashThrottle, 0);
    ASSERT_EQ(this->component.DpWriterComponentBase::m_InvalidHeaderThrottle, 0);
    // Update the abstract state
    this->abstractState.m_bufferTooSmallForDataEventCount = 0;
    this->abstractState.m_bufferTooSmallForPacketEventCount = 0;
    this->abstractState.m_fileOpenErrorEventCount = 0;
    this->abstractState.m_fileWriteErrorEventCount = 0;
    this->abstractState.m_invalidBufferEventCount = 0;
    this->abstractState.m_invalidHeaderEventCount = 0;
    this->abstractState.m_invalidHeaderHashEventCount = 0;
}

namespace CLEAR_EVENT_THROTTLE {

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::OK() {
    for (FwSizeType i = 0; i <= DpWriterComponentBase::EVENTID_INVALIDBUFFER_THROTTLE; i++) {
        Testers::bufferSendIn.ruleInvalidBuffer.apply(this->testState);
    }
    this->ruleOK.apply(this->testState);
    Testers::bufferSendIn.ruleInvalidBuffer.apply(this->testState);
}

}  // namespace CLEAR_EVENT_THROTTLE

}  // namespace Svc
