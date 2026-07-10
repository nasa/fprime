// ======================================================================
// \title  Route.cpp
// \author lestarch-autobot
// \brief  Rule implementations for the Route rule group
//
// These rules exercise the decryptIn port: SA-to-port routing and the
// UNKNOWN_SA / UNKNOWN_PORT error statuses passed forward on decryptOut.
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "Svc/Ccsds/SdlsSaRouter/test/ut/SdlsSaRouterTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Route.KnownSa
// ----------------------------------------------------------------------

bool SdlsSaRouterTester::Route__KnownSa__precondition() const {
    return true;
}

void SdlsSaRouterTester::Route__KnownSa__action() {
    this->clearHistory();

    const FwSizeType pick = this->pickConnectedEntry();
    const U16 sa = this->m_mapSas[pick];
    const FwIndexType expectedPort = this->m_mapPorts[pick];

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_decryptIn(0, sa, buffer, context);

    ASSERT_from_saDecryptOut_SIZE(1);
    ASSERT_from_saDecryptOut(0, sa, buffer, context);
    ASSERT_EQ(this->m_lastSaDecryptOutPort, expectedPort);
    ASSERT_from_decryptOut_SIZE(0);
}

// ----------------------------------------------------------------------
// Route.UnknownSa
// ----------------------------------------------------------------------

bool SdlsSaRouterTester::Route__UnknownSa__precondition() const {
    return this->shadow.shadow_outstanding.size() < SdlsCfg::SaRouterMaxOutstandingBuffers;
}

void SdlsSaRouterTester::Route__UnknownSa__action() {
    this->clearHistory();

    // Pick an SA outside the configured map
    U16 sa = 0;
    do {
        sa = static_cast<U16>(STest::Pick::lowerUpper(0, 0xFFFF));
    } while (this->isMappedSa(sa));
    U8* const storage = this->getFreePoolBuffer();
    ASSERT_NE(storage, nullptr);
    Fw::Buffer buffer = this->makePoolBuffer(storage);
    ComCfg::FrameContext context;

    this->invoke_to_decryptIn(0, sa, buffer, context);

    // The error status is passed forward on decryptOut with the untouched buffer
    ASSERT_from_saDecryptOut_SIZE(0);
    ASSERT_from_decryptOut_SIZE(1);
    ASSERT_from_decryptOut(0, Svc::Ccsds::SdlsStatus::UNKNOWN_SA, buffer, context);
    this->shadow.shadow_outstanding[storage] = ROUTER_ERROR_PORT;
}

// ----------------------------------------------------------------------
// Route.UnknownPort
// ----------------------------------------------------------------------

bool SdlsSaRouterTester::Route__UnknownPort__precondition() const {
    return this->shadow.shadow_outstanding.size() < SdlsCfg::SaRouterMaxOutstandingBuffers;
}

void SdlsSaRouterTester::Route__UnknownPort__action() {
    this->clearHistory();

    // Find an SA routed to the unconnected port
    U16 sa = 0;
    bool found = false;
    for (FwSizeType i = 0; i < SdlsCfg::SaRouterMapEntryCount; i++) {
        if (this->m_mapPorts[i] == UNCONNECTED_PORT) {
            sa = this->m_mapSas[i];
            found = true;
            break;
        }
    }
    ASSERT_TRUE(found);
    U8* const storage = this->getFreePoolBuffer();
    ASSERT_NE(storage, nullptr);
    Fw::Buffer buffer = this->makePoolBuffer(storage);
    ComCfg::FrameContext context;

    this->invoke_to_decryptIn(0, sa, buffer, context);

    // The error status is passed forward on decryptOut with the untouched buffer
    ASSERT_from_saDecryptOut_SIZE(0);
    ASSERT_from_decryptOut_SIZE(1);
    ASSERT_from_decryptOut(0, Svc::Ccsds::SdlsStatus::UNKNOWN_PORT, buffer, context);
    this->shadow.shadow_outstanding[storage] = ROUTER_ERROR_PORT;
}

}  // namespace Ccsds

}  // namespace Svc
