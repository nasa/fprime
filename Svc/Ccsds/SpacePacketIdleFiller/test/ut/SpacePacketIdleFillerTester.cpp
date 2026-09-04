// ======================================================================
// \title  SpacePacketIdleFillerTester.cpp
// \author claradavisb
// \brief  cpp file for SpacePacketIdleFiller component test harness implementation class
// ======================================================================

#include "Svc/Ccsds/SpacePacketIdleFiller/test/ut/SpacePacketIdleFillerTester.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

const FwSizeType SpacePacketIdleFillerTester::MAX_HISTORY_SIZE;
const FwEnumStoreType SpacePacketIdleFillerTester::TEST_INSTANCE_ID;
const FwSizeType SpacePacketIdleFillerTester::TEST_TARGET_SIZE;
const FwSizeType SpacePacketIdleFillerTester::MIN_IDLE_PACKET_SIZE;
const FwSizeType SpacePacketIdleFillerTester::TEST_BUFFER_SIZE;

namespace {

//! Read a big-endian U16 out of a serialized buffer
U16 readU16(const U8* data, FwSizeType offset) {
    return static_cast<U16>((static_cast<U16>(data[offset]) << 8) | static_cast<U16>(data[offset + 1]));
}

}  // namespace

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SpacePacketIdleFillerTester ::SpacePacketIdleFillerTester()
    : SpacePacketIdleFillerGTestBase("SpacePacketIdleFillerTester", SpacePacketIdleFillerTester::MAX_HISTORY_SIZE),
      component("SpacePacketIdleFiller"),
      m_bufferStorage() {
    this->initComponents();
    this->connectPorts();
    this->component.configure(TEST_TARGET_SIZE);
}

SpacePacketIdleFillerTester ::~SpacePacketIdleFillerTester() {}

// ----------------------------------------------------------------------
// Test cases
// ----------------------------------------------------------------------

void SpacePacketIdleFillerTester ::testPadsToTarget() {
    const FwSizeType inputSize = 20;
    const Fw::Buffer sent = this->sendData(inputSize);

    ASSERT_from_dataOut_SIZE(1);
    const Fw::Buffer out = this->emitted(0);
    ASSERT_EQ(out.getSize(), TEST_TARGET_SIZE);
    // The padded buffer is a copy, not the caller's storage
    ASSERT_NE(out.getData(), sent.getData());
    for (FwSizeType i = 0; i < inputSize; i++) {
        ASSERT_EQ(out.getData()[i], sent.getData()[i]) << "Payload byte " << i << " was not preserved";
    }
    ASSERT_EVENTS_SIZE(0);
}

void SpacePacketIdleFillerTester ::testIdlePacketFormat() {
    const FwSizeType inputSize = 20;
    (void)this->sendData(inputSize);

    ASSERT_from_dataOut_SIZE(1);
    const U8* const out = this->emitted(0).getData();
    const FwSizeType gap = TEST_TARGET_SIZE - inputSize;

    // Packet identification: version 0, type 0 (telemetry), no secondary header, APID 0x7FF
    const U16 idApid = static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET);
    ASSERT_EQ(readU16(out, inputSize), idApid);
    // Sequence flags 0b11 (unsegmented) in the top two bits, sequence count unused
    ASSERT_EQ(readU16(out, inputSize + 2), static_cast<U16>(0xC000));
    // Packet data length is the byte count of the data field minus one
    ASSERT_EQ(readU16(out, inputSize + 4), static_cast<U16>(gap - SpacePacketHeader::SERIALIZED_SIZE - 1));

    const U8 idlePattern = this->component.IDLE_DATA_PATTERN;
    for (FwSizeType i = inputSize + SpacePacketHeader::SERIALIZED_SIZE; i < TEST_TARGET_SIZE; i++) {
        ASSERT_EQ(out[i], idlePattern) << "Idle byte " << i << " does not match the fill pattern";
    }
}

void SpacePacketIdleFillerTester ::testMinimumGap() {
    const FwSizeType inputSize = TEST_TARGET_SIZE - MIN_IDLE_PACKET_SIZE;
    (void)this->sendData(inputSize);

    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EVENTS_SIZE(0);
    const U8* const out = this->emitted(0).getData();
    // Header plus exactly one byte of data, so the length token is zero
    ASSERT_EQ(readU16(out, inputSize + 4), static_cast<U16>(0));
    const U8 idlePattern = this->component.IDLE_DATA_PATTERN;
    ASSERT_EQ(out[TEST_TARGET_SIZE - 1], idlePattern);
}

void SpacePacketIdleFillerTester ::testExactFitNoPadding() {
    const Fw::Buffer sent = this->sendData(TEST_TARGET_SIZE);

    ASSERT_from_dataOut_SIZE(1);
    const Fw::Buffer out = this->emitted(0);
    ASSERT_EQ(out.getSize(), TEST_TARGET_SIZE);
    ASSERT_EVENTS_SIZE(0);
    // Every byte is payload; nothing was appended
    for (FwSizeType i = 0; i < TEST_TARGET_SIZE; i++) {
        ASSERT_EQ(out.getData()[i], sent.getData()[i]) << "Payload byte " << i << " was not preserved";
    }
}

void SpacePacketIdleFillerTester ::testInputTooLarge() {
    const FwSizeType inputSize = TEST_TARGET_SIZE + 1;
    const Fw::Buffer sent = this->sendData(inputSize);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_EVENTS_InputTooLarge_SIZE(1);
    ASSERT_EVENTS_InputTooLarge(0, inputSize, TEST_TARGET_SIZE);
    // The caller still gets its buffer back
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), sent.getData());
    // Com status token is released, so the upstream aggregator does not stall
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_comStatusOut->at(0).condition, Fw::Success::SUCCESS);
}

void SpacePacketIdleFillerTester ::testGapTooSmall() {
    const FwSizeType gap = MIN_IDLE_PACKET_SIZE - 1;
    const Fw::Buffer sent = this->sendData(TEST_TARGET_SIZE - gap);

    ASSERT_from_dataOut_SIZE(0);
    ASSERT_EVENTS_GapTooSmall_SIZE(1);
    ASSERT_EVENTS_GapTooSmall(0, gap);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), sent.getData());
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_comStatusOut->at(0).condition, Fw::Success::SUCCESS);
}

void SpacePacketIdleFillerTester ::testContextForwarded() {
    Fw::Buffer buffer(this->m_bufferStorage, static_cast<Fw::Buffer::SizeType>(20));
    ComCfg::FrameContext context;
    context.set_apid(ComCfg::Apid::FW_PACKET_TELEM);
    context.set_vcId(3);
    this->invoke_to_dataIn(0, buffer, context);

    ASSERT_from_dataOut_SIZE(1);
    const ComCfg::FrameContext out = this->fromPortHistory_dataOut->at(0).context;
    ASSERT_EQ(out.get_apid(), ComCfg::Apid::FW_PACKET_TELEM);
    ASSERT_EQ(out.get_vcId(), 3);
}

void SpacePacketIdleFillerTester ::testInputReturned() {
    const Fw::Buffer sent = this->sendData(20);

    // The copy is made synchronously, so the incoming buffer is free right away
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_dataReturnOut->at(0).data.getData(), sent.getData());
}

void SpacePacketIdleFillerTester ::testStorageReusedAfterReturn() {
    (void)this->sendData(20);
    ASSERT_EQ(this->component.m_bufferState, SpacePacketIdleFiller::BufferOwnershipState::NOT_OWNED);

    this->returnEmitted();
    ASSERT_EQ(this->component.m_bufferState, SpacePacketIdleFiller::BufferOwnershipState::OWNED);

    // A second buffer is accepted and padded the same way
    this->clearHistory();
    (void)this->sendData(30);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_EQ(this->emitted(0).getSize(), TEST_TARGET_SIZE);
}

void SpacePacketIdleFillerTester ::testStatusForwarded() {
    Fw::Success status = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, status);

    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_comStatusOut->at(0).condition, Fw::Success::SUCCESS);
}

// ----------------------------------------------------------------------
// Helpers
// ----------------------------------------------------------------------

Fw::Buffer SpacePacketIdleFillerTester ::sendData(FwSizeType size) {
    FW_ASSERT(size <= TEST_BUFFER_SIZE, static_cast<FwAssertArgType>(size));
    for (FwSizeType i = 0; i < size; i++) {
        this->m_bufferStorage[i] = static_cast<U8>(i + 1);  // nonzero, so fill bytes stand out
    }
    Fw::Buffer buffer(this->m_bufferStorage, static_cast<Fw::Buffer::SizeType>(size));
    ComCfg::FrameContext context;
    this->invoke_to_dataIn(0, buffer, context);
    return buffer;
}

Fw::Buffer SpacePacketIdleFillerTester ::emitted(U32 index) {
    return this->fromPortHistory_dataOut->at(index).data;
}

void SpacePacketIdleFillerTester ::returnEmitted() {
    Fw::Buffer buffer = this->emitted(0);
    ComCfg::FrameContext context = this->fromPortHistory_dataOut->at(0).context;
    this->invoke_to_dataReturnIn(0, buffer, context);
}

}  // namespace Ccsds

}  // namespace Svc
