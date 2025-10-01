// ======================================================================
// \title  CLEAR_EVENT_THROTTLE.cpp
// \author Rob Bocchino
// \brief  CLEAR_EVENT_THROTTLE class implementation
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#include "Svc/DpWriter/test/ut/Rules/CLEAR_EVENT_THROTTLE.hpp"
#include "STest/Pick/Pick.hpp"
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
    this->doDispatch();
    // Check the command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, DpWriterTester::getOpCodeClearEventThrottle(), cmdSeq, Fw::CmdResponse::OK);
    // Check the concrete state
    ASSERT_EQ(this->getBufferTooSmallForDataThrottleCount(), 0);
    ASSERT_EQ(this->getBufferTooSmallForPacketThrottleCount(), 0);
    ASSERT_EQ(this->getFileOpenErrorThrottleCount(), 0);
    ASSERT_EQ(this->getFileWriteErrorThrottleCount(), 0);
    ASSERT_EQ(this->getInvalidBufferThrottleCount(), 0);
    ASSERT_EQ(this->getInvalidHeaderHashThrottleCount(), 0);
    ASSERT_EQ(this->getInvalidHeaderThrottleCount(), 0);
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
    for (FwSizeType i = 0; i <= DpWriterTester::getInvalidBufferThrottle(); i++) {
        Testers::bufferSendIn.ruleInvalidBuffer.apply(this->testState);
    }
    this->ruleOK.apply(this->testState);
    Testers::bufferSendIn.ruleInvalidBuffer.apply(this->testState);
}

}  // namespace CLEAR_EVENT_THROTTLE

}  // namespace Svc
