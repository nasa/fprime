// ======================================================================
// \title  DataFlow.cpp
// \author lestarch-autobot
// \brief  Rule implementations for the DataFlow rule group
//
// These rules exercise the decrypted data path (saDecryptIn -> decryptOut),
// the ownership return path (decryptReturnIn -> saDecryptReturnOut), and the
// deallocation path (saBufferReturnIn -> bufferReturnOut).
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "Svc/Ccsds/SdlsSaRouter/test/ut/SdlsSaRouterTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// DataFlow.DecryptData
// ----------------------------------------------------------------------

bool SdlsSaRouterTester::DataFlow__DecryptData__precondition() const {
    return this->shadow.shadow_outstanding.size() < SdlsCfg::SaRouterMaxOutstandingBuffers;
}

void SdlsSaRouterTester::DataFlow__DecryptData__action() {
    this->clearHistory();

    U8* const storage = this->getFreePoolBuffer();
    ASSERT_NE(storage, nullptr);
    const FwIndexType portNum = static_cast<FwIndexType>(STest::Pick::lowerUpper(0, SdlsCfg::SaRouterPortCount - 1));
    Fw::Buffer buffer(storage, TEST_BUFFER_SIZE);
    ComCfg::FrameContext context;

    // Pick a random status to verify pass-forward alongside the data
    const Svc::Ccsds::SdlsStatus status = (STest::Pick::lowerUpper(0, 1) == 0)
                                              ? Svc::Ccsds::SdlsStatus::SUCCESS
                                              : Svc::Ccsds::SdlsStatus::DECRYPTION_FAILURE;

    this->invoke_to_saDecryptIn(portNum, status, buffer, context);

    ASSERT_from_decryptOut_SIZE(1);
    ASSERT_from_decryptOut(0, status, buffer, context);
    this->shadow.shadow_outstanding[storage] = portNum;
}

// ----------------------------------------------------------------------
// DataFlow.DecryptReturn
// ----------------------------------------------------------------------

bool SdlsSaRouterTester::DataFlow__DecryptReturn__precondition() const {
    return !this->shadow.shadow_outstanding.empty();
}

void SdlsSaRouterTester::DataFlow__DecryptReturn__action() {
    this->clearHistory();

    const U8* const storage = this->shadow.shadow_getRandomOutstanding();
    const FwIndexType expectedPort = this->shadow.shadow_outstanding[storage];
    Fw::Buffer buffer(const_cast<U8*>(storage), TEST_BUFFER_SIZE);
    ComCfg::FrameContext context;

    this->invoke_to_decryptReturnIn(0, buffer, context);

    if (expectedPort == ROUTER_ERROR_PORT) {
        // Buffer was forwarded by the router itself on a routing error: returned upstream
        ASSERT_from_saDecryptReturnOut_SIZE(0);
        ASSERT_from_bufferReturnOut_SIZE(1);
        ASSERT_from_bufferReturnOut(0, buffer, context);
    } else {
        ASSERT_from_saDecryptReturnOut_SIZE(1);
        ASSERT_from_saDecryptReturnOut(0, buffer, context);
        ASSERT_EQ(this->m_lastSaDecryptReturnOutPort, expectedPort);
    }
    this->shadow.shadow_outstanding.erase(storage);
}

// ----------------------------------------------------------------------
// DataFlow.BufferReturn
// ----------------------------------------------------------------------

bool SdlsSaRouterTester::DataFlow__BufferReturn__precondition() const {
    return true;
}

void SdlsSaRouterTester::DataFlow__BufferReturn__action() {
    this->clearHistory();

    const FwIndexType portNum = static_cast<FwIndexType>(STest::Pick::lowerUpper(0, SdlsCfg::SaRouterPortCount - 1));
    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_saBufferReturnIn(portNum, buffer, context);

    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_from_bufferReturnOut(0, buffer, context);
}

}  // namespace Ccsds

}  // namespace Svc
