// ======================================================================
// \title  ComToComBufferAdapterTester.cpp
// \brief  cpp file for ComToComBufferAdapter component test harness
// ======================================================================

#include "ComToComBufferAdapterTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ComToComBufferAdapterTester ::ComToComBufferAdapterTester()
    : ComToComBufferAdapterGTestBase("ComToComBufferAdapterTester", MAX_HISTORY_SIZE),
      component("ComToComBufferAdapter") {
    this->initComponents();
    this->connectPorts();
}

ComToComBufferAdapterTester ::~ComToComBufferAdapterTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ComToComBufferAdapterTester ::testNominal() {
    // Build a legacy buffer: [descriptor=FW_PACKET_TELEM][payload...]
    const FwPacketDescriptorType descriptor = static_cast<FwPacketDescriptorType>(ComCfg::Apid::FW_PACKET_TELEM);
    const U8 payload[] = {0x11, 0x22, 0x33, 0x44, 0x55};

    Fw::ComBuffer legacy;
    ASSERT_EQ(legacy.serializeFrom(descriptor), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(legacy.serializeFrom(payload, sizeof(payload), Fw::Serialization::OMIT_LENGTH), Fw::FW_SERIALIZE_OK);

    const U32 context = 0xABCD1234;
    this->invoke_to_comIn(0, legacy, context);

    // One packet should have been forwarded with the descriptor stripped
    ASSERT_from_comBufferSendOut_SIZE(1);
    const FromPortEntry_comBufferSendOut& entry = this->fromPortHistory_comBufferSendOut->at(0);

    // The descriptor is now carried as the APID argument
    ASSERT_EQ(entry.packetType, ComCfg::Apid(ComCfg::Apid::FW_PACKET_TELEM));
    // The call context is preserved
    ASSERT_EQ(entry.context, context);
    // The forwarded payload no longer contains the descriptor
    ASSERT_EQ(entry.data.getSize(), static_cast<FwSizeType>(sizeof(payload)));
    ASSERT_EQ(memcmp(entry.data.getBuffAddr(), payload, sizeof(payload)), 0);
}

void ComToComBufferAdapterTester ::testEmptyPayload() {
    // A legacy buffer that contains only the descriptor (no payload bytes)
    const FwPacketDescriptorType descriptor = static_cast<FwPacketDescriptorType>(ComCfg::Apid::FW_PACKET_LOG);

    Fw::ComBuffer legacy;
    ASSERT_EQ(legacy.serializeFrom(descriptor), Fw::FW_SERIALIZE_OK);

    this->invoke_to_comIn(0, legacy, 0);

    ASSERT_from_comBufferSendOut_SIZE(1);
    const FromPortEntry_comBufferSendOut& entry = this->fromPortHistory_comBufferSendOut->at(0);
    ASSERT_EQ(entry.packetType, ComCfg::Apid(ComCfg::Apid::FW_PACKET_LOG));
    ASSERT_EQ(entry.data.getSize(), static_cast<FwSizeType>(0));
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void ComToComBufferAdapterTester ::from_comBufferSendOut_handler(FwIndexType portNum,
                                                                 Fw::ComBuffer& data,
                                                                 const ComCfg::Apid& packetType,
                                                                 U32 context) {
    this->pushFromPortEntry_comBufferSendOut(data, packetType, context);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void ComToComBufferAdapterTester ::connectPorts() {
    this->connect_to_comIn(0, this->component.get_comIn_InputPort(0));
    this->component.set_comBufferSendOut_OutputPort(0, this->get_from_comBufferSendOut(0));
}

void ComToComBufferAdapterTester ::initComponents() {
    this->init();
    this->component.init(TEST_INSTANCE_ID);
}

}  // namespace Svc
