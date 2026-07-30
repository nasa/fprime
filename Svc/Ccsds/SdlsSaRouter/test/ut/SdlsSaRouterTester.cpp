// ======================================================================
// \title  SdlsSaRouterTester.cpp
// \author lestarch-autobot
// \brief  cpp file for SdlsSaRouter component test harness implementation class
// ======================================================================

#include "Svc/Ccsds/SdlsSaRouter/test/ut/SdlsSaRouterTester.hpp"
#include "STest/Pick/Pick.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SdlsSaRouterTester ::SdlsSaRouterTester()
    : SdlsSaRouterGTestBase("SdlsSaRouterTester", SdlsSaRouterTester::MAX_HISTORY_SIZE), component("SdlsSaRouter") {
    this->initComponents();
    this->connectPortsCustom();

    // Mirror the compile-time map for computing expected routing
    const SdlsCfg::SaMap saMap;
    for (SdlsCfg::SaMap::SizeType i = 0; i < SdlsCfg::SaMap::SIZE; i++) {
        this->m_mapSas[i] = saMap[i].get_securityAssociationIndex();
        this->m_mapPorts[i] = saMap[i].get_portIndex();
    }
}

SdlsSaRouterTester ::~SdlsSaRouterTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Handler overrides for typed from ports
// ----------------------------------------------------------------------

void SdlsSaRouterTester ::from_saDataOut_handler(FwIndexType portNum,
                                                 U16 securityAssociationIndex,
                                                 Fw::Buffer& data,
                                                 const ComCfg::FrameContext& context) {
    this->m_lastSaDataOutPort = portNum;
    this->pushFromPortEntry_saDataOut(securityAssociationIndex, data, context);
}

void SdlsSaRouterTester ::from_saDataReturnOut_handler(FwIndexType portNum,
                                                       Fw::Buffer& data,
                                                       const ComCfg::FrameContext& context) {
    this->m_lastSaDataReturnOutPort = portNum;
    this->pushFromPortEntry_saDataReturnOut(data, context);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void SdlsSaRouterTester ::connectPortsCustom() {
    // Connect typed input ports
    this->connect_to_dataIn(0, this->component.get_dataIn_InputPort(0));
    this->connect_to_dataReturnIn(0, this->component.get_dataReturnIn_InputPort(0));
    for (FwIndexType i = 0; i < SdlsCfg::SaRouterPortCount; i++) {
        this->connect_to_saBufferReturnIn(i, this->component.get_saBufferReturnIn_InputPort(i));
        this->connect_to_saDataIn(i, this->component.get_saDataIn_InputPort(i));
    }

    // Connect special ports
    this->component.set_logOut_OutputPort(0, this->get_from_logOut(0));
    this->component.set_logTextOut_OutputPort(0, this->get_from_logTextOut(0));
    this->component.set_timeCaller_OutputPort(0, this->get_from_timeCaller(0));

    // Connect typed output ports, leaving saDataOut[UNCONNECTED_PORT] unconnected
    this->component.set_bufferReturnOut_OutputPort(0, this->get_from_bufferReturnOut(0));
    this->component.set_dataOut_OutputPort(0, this->get_from_dataOut(0));
    for (FwIndexType i = 0; i < SdlsCfg::SaRouterPortCount; i++) {
        if (i != UNCONNECTED_PORT) {
            this->component.set_saDataOut_OutputPort(i, this->get_from_saDataOut(i));
        }
        this->component.set_saDataReturnOut_OutputPort(i, this->get_from_saDataReturnOut(i));
    }
}

Fw::Buffer SdlsSaRouterTester ::makePoolBuffer(U8* storage) {
    // Buffers carry a unique allocation context (as assigned by a buffer manager); the
    // router tracks outstanding buffers by this context
    const U32 poolIndex = static_cast<U32>((storage - &this->m_pool[0][0]) / TEST_BUFFER_SIZE);
    Fw::Buffer buffer(storage, TEST_BUFFER_SIZE);
    buffer.setContext(poolIndex);
    return buffer;
}

bool SdlsSaRouterTester ::isMappedSa(U16 sa) const {
    for (FwSizeType i = 0; i < SdlsCfg::SaRouterMapEntryCount; i++) {
        if (this->m_mapSas[i] == sa) {
            return true;
        }
    }
    return false;
}

FwSizeType SdlsSaRouterTester ::pickConnectedEntry() const {
    FwSizeType candidates[SdlsCfg::SaRouterMapEntryCount];
    FwSizeType count = 0;
    for (FwSizeType i = 0; i < SdlsCfg::SaRouterMapEntryCount; i++) {
        if (this->m_mapPorts[i] != UNCONNECTED_PORT) {
            candidates[count] = i;
            count++;
        }
    }
    FW_ASSERT(count > 0);
    return candidates[STest::Pick::lowerUpper(0, static_cast<U32>(count - 1))];
}

U8* SdlsSaRouterTester ::getFreePoolBuffer() {
    for (FwSizeType i = 0; i < SdlsCfg::SaRouterMaxOutstandingBuffers; i++) {
        if (this->shadow.shadow_outstanding.count(this->m_pool[i]) == 0) {
            return this->m_pool[i];
        }
    }
    return nullptr;
}

// ----------------------------------------------------------------------
// Directed test scenarios
// ----------------------------------------------------------------------

void SdlsSaRouterTester ::fillOutstandingTable() {
    for (FwSizeType i = 0; i < SdlsCfg::SaRouterMaxOutstandingBuffers; i++) {
        U8* const storage = this->getFreePoolBuffer();
        ASSERT_NE(storage, nullptr);
        Fw::Buffer buffer = this->makePoolBuffer(storage);
        ComCfg::FrameContext context;
        this->invoke_to_saDataIn(0, Svc::Ccsds::SdlsStatus::SUCCESS, buffer, context);
        this->shadow.shadow_outstanding[storage] = 0;
    }
}

void SdlsSaRouterTester ::testTableFullDataIn() {
    this->fillOutstandingTable();
    this->clearHistory();

    // Pick an SA outside the configured map so routing fails and tracking is attempted
    U16 unmappedSa = 100;
    while (this->isMappedSa(unmappedSa)) {
        unmappedSa++;
    }
    U8 storage[TEST_BUFFER_SIZE] = {};
    Fw::Buffer buffer(storage, sizeof storage);
    buffer.setContext(static_cast<U32>(SdlsCfg::SaRouterMaxOutstandingBuffers) + 1);
    ComCfg::FrameContext context;

    this->invoke_to_dataIn(0, unmappedSa, buffer, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_from_bufferReturnOut(0, buffer, context);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TrackingTableFull_SIZE(1);
}

void SdlsSaRouterTester ::testUntrackedDataReturn() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE] = {};
    Fw::Buffer buffer(storage, sizeof storage);
    buffer.setContext(static_cast<U32>(SdlsCfg::SaRouterMaxOutstandingBuffers) + 1);
    ComCfg::FrameContext context;

    this->invoke_to_dataReturnIn(0, buffer, context);

    ASSERT_from_saDataReturnOut_SIZE(0);
    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_from_bufferReturnOut(0, buffer, context);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_UntrackedBufferReturned_SIZE(1);
}

void SdlsSaRouterTester ::testTableFullSaDataIn() {
    this->fillOutstandingTable();
    this->clearHistory();

    const FwIndexType portNum = static_cast<FwIndexType>(STest::Pick::lowerUpper(0, SdlsCfg::SaRouterPortCount - 1));
    U8 storage[TEST_BUFFER_SIZE] = {};
    Fw::Buffer buffer(storage, sizeof storage);
    buffer.setContext(static_cast<U32>(SdlsCfg::SaRouterMaxOutstandingBuffers) + 1);
    ComCfg::FrameContext context;

    this->invoke_to_saDataIn(portNum, Svc::Ccsds::SdlsStatus::SUCCESS, buffer, context);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_from_saDataReturnOut_SIZE(1);
    ASSERT_from_saDataReturnOut(0, buffer, context);
    ASSERT_EQ(this->m_lastSaDataReturnOutPort, portNum);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_TrackingTableFull_SIZE(1);
}

}  // namespace Ccsds

}  // namespace Svc
