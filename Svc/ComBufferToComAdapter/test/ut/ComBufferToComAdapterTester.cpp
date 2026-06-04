// ======================================================================
// \title  ComBufferToComAdapterTester.cpp
// \brief  cpp file for ComBufferToComAdapter component test harness
// ======================================================================

#include "ComBufferToComAdapterTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ComBufferToComAdapterTester ::ComBufferToComAdapterTester()
    : ComBufferToComAdapterGTestBase("ComBufferToComAdapterTester", MAX_HISTORY_SIZE),
      component("ComBufferToComAdapter") {
    this->initComponents();
    this->connectPorts();
}

ComBufferToComAdapterTester ::~ComBufferToComAdapterTester() {}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ComBufferToComAdapterTester ::testNominal() {
    const U8 payload[] = {0x11, 0x22, 0x33, 0x44, 0x55};
    Fw::ComBuffer data(payload, sizeof(payload));

    const U32 context = 0xABCD1234;
    this->invoke_to_comBufferSendIn(0, data, ComCfg::Apid::FW_PACKET_TELEM, context);

    ASSERT_from_comOut_SIZE(1);
    const FromPortEntry_comOut& entry = this->fromPortHistory_comOut->at(0);

    // The legacy buffer is the descriptor followed by the original payload
    const FwPacketDescriptorType descriptor = static_cast<FwPacketDescriptorType>(ComCfg::Apid::FW_PACKET_TELEM);
    Fw::ComBuffer expected;
    ASSERT_EQ(expected.serializeFrom(descriptor), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(expected.serializeFrom(payload, sizeof(payload), Fw::Serialization::OMIT_LENGTH), Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(entry.context, context);
    ASSERT_EQ(entry.data.getSize(), expected.getSize());
    ASSERT_EQ(memcmp(entry.data.getBuffAddr(), expected.getBuffAddr(), expected.getSize()), 0);
}

void ComBufferToComAdapterTester ::testEmptyPayload() {
    Fw::ComBuffer data;  // empty payload

    this->invoke_to_comBufferSendIn(0, data, ComCfg::Apid::FW_PACKET_LOG, 0);

    ASSERT_from_comOut_SIZE(1);
    const FromPortEntry_comOut& entry = this->fromPortHistory_comOut->at(0);

    // The buffer contains only the descriptor
    ASSERT_EQ(entry.data.getSize(), static_cast<FwSizeType>(sizeof(FwPacketDescriptorType)));
}

// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void ComBufferToComAdapterTester ::from_comOut_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    this->pushFromPortEntry_comOut(data, context);
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void ComBufferToComAdapterTester ::connectPorts() {
    this->connect_to_comBufferSendIn(0, this->component.get_comBufferSendIn_InputPort(0));
    this->component.set_comOut_OutputPort(0, this->get_from_comOut(0));
}

void ComBufferToComAdapterTester ::initComponents() {
    this->init();
    this->component.init(TEST_INSTANCE_ID);
}

}  // namespace Svc
