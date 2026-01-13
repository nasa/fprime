// ======================================================================
// \title  AosFramerTester.cpp
// \author thomas-bc
// \brief  cpp file for AosFramer component test harness implementation class
// ======================================================================

#include "AosFramerTester.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TMTrailerSerializableAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AosFramerTester ::AosFramerTester()
    : AosFramerGTestBase("AosFramerTester", AosFramerTester::MAX_HISTORY_SIZE), component("AosFramer") {
    this->initComponents();
    this->connectPorts();

    // Configure our Framer
    this->component.configure(ComCfg::AosMaxFrameFixedSize, true);
}

AosFramerTester ::~AosFramerTester() {}

// ----------------------------------------------------------------------
// Base Operational Tests (Same as TM)
// ----------------------------------------------------------------------

void AosFramerTester ::testComStatusPassthrough() {
    // Send a status message to the component
    Fw::Success inputStatus = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, inputStatus);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, inputStatus);  // at index 0, received SUCCESS
    inputStatus = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, inputStatus);
    ASSERT_from_comStatusOut_SIZE(2);
    ASSERT_from_comStatusOut(1, inputStatus);  // at index 1, received FAILURE
}

void AosFramerTester ::testNominalFraming() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));

    ComCfg::FrameContext defaultContext;
    defaultContext.set_sendNow(true);

    // Fill the buffer with some data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Invoke the dataIn handler
    this->invoke_to_dataIn(0, buffer, defaultContext);

    // Check that the dataOut handler was called with the correct data
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(0).context;
    const FwSizeType expectedFrameSize = ComCfg::AosMaxFrameFixedSize;
    ASSERT_EQ(outBuffer.getSize(), expectedFrameSize);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_vcId(), defaultContext.get_vcId());

    U16 outScId = this->getFrameScId(outBuffer.getData());
    U8 outVcId = this->getFrameVcId(outBuffer.getData());
    U8 outTfVn = this->getFrameTfVn(outBuffer.getData());
    U32 outVcCount = this->getFrameVcCount(outBuffer.getData());

    const U8 expectedTfVn = 0b01;
    ASSERT_EQ(outTfVn, expectedTfVn);
    const U16 expectedScId = ComCfg::SpacecraftId;
    ASSERT_EQ(outScId, expectedScId);
    ASSERT_EQ(outVcId, defaultContext.get_vcId());
    ASSERT_EQ(outVcCount, 0);
    ASSERT_EQ(this->component.m_vcs[0].virtualFrameCount, outVcCount + 1);

    // Idle data should be filled at the offset of header + payload + the Space Packet Idle Packet header
    FwSizeType expectedIdleDataOffset = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE + sizeof(bufferData) +
                                        SpacePacketHeader::SERIALIZED_SIZE;

    // The frame is composed of the payload + a SpacePacket Idle Packet (Header + idle_pattern)
    const U8 idlePattern = this->component.SPP_IDLE_DATA_PATTERN;
    const FwSizeType ideDataEndOffset = ComCfg::AosMaxFrameFixedSize - AOSTrailer::SERIALIZED_SIZE;
    for (FwSizeType i = expectedIdleDataOffset; i < ideDataEndOffset; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], idlePattern)
            << "Idle data at index " << i << " does not match expected idle pattern";
    }
}

void AosFramerTester ::testSeqCountWrapAround() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));

    ComCfg::FrameContext defaultContext;
    defaultContext.set_sendNow(true);

    // Fill the buffer with some data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Intentionally set the sequence count to 2^28 - 5 and iterate 10 times
    // to test the wrap around of the sequence counts
    this->component.m_vcs[0].virtualFrameCount = (1 << 28) - 5;
    U32 countWrapAround = (1 << 28) - 5;  // will wrap around to 0 after 2^28
    for (U32 iter = 0; iter < 10; iter++) {
        this->component.m_vcs[0].frame.state = AosFramer::BufferOwnershipState::OWNED;  // reset state to OWNED
        this->invoke_to_dataIn(0, buffer, defaultContext);
        ASSERT_from_dataOut_SIZE(iter + 1);
        Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(iter).data;
        U32 outVcCount = this->getFrameVcCount(outBuffer.getData());
        ASSERT_EQ(outVcCount, countWrapAround & 0x0FFFFFFF);
        countWrapAround++;
    }
}

void AosFramerTester ::testDataReturn() {
    U8 bufferData[10];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));

    ComCfg::FrameContext defaultContext;
    defaultContext.set_sendNow(true);

    // Send a buffer that is not the internal buffer of the component, and expect an assertion
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataReturnIn(0, buffer, defaultContext), "AosFramer.cpp");

    // Now send the expected buffer and expect state to go back to OWNED
    this->component.m_vcs[0].frame.state = AosFramer::BufferOwnershipState::NOT_OWNED;
    Fw::Buffer internalBuffer(this->component.m_vcs[0].frame.backer, sizeof(this->component.m_vcs[0].frame.backer));
    this->invoke_to_dataReturnIn(0, internalBuffer, defaultContext);
    ASSERT_EQ(this->component.m_vcs[0].frame.state, AosFramer::BufferOwnershipState::OWNED);
}

void AosFramerTester ::testBufferOwnershipState() {
    U8 bufferData[10];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));

    ComCfg::FrameContext context;
    context.set_sendNow(true);

    // force state to be NOT_OWNED and test that assertion is triggered
    this->component.m_vcs[0].frame.state = AosFramer::BufferOwnershipState::NOT_OWNED;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "AosFramer.cpp");
    this->component.m_vcs[0].frame.state = AosFramer::BufferOwnershipState::OWNED;
    this->invoke_to_dataIn(0, buffer, context);  // this should work now
    ASSERT_EQ(this->component.m_vcs[0].frame.state, AosFramer::BufferOwnershipState::NOT_OWNED);
}

// ----------------------------------------------------------------------
// Extended Operation Tests
// ----------------------------------------------------------------------

void AosFramerTester ::testLongPacket() {
    // This will need 2 full & 1 partial AOS Frames to send
    U8 bufferData[2048];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));

    ComCfg::FrameContext context;
    // Use sendNow to get that final frame ASAP
    context.set_sendNow(true);

    // Fill the buffer with some data
    // This isn't actually a space packet so this test can't decode it for a length
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Invoke the dataIn handler
    this->invoke_to_dataIn(0, buffer, context);

    const FwSizeType expectedFrameSize = ComCfg::AosMaxFrameFixedSize;

    for (U8 frame = 0; frame < 3; frame++) {
        ASSERT_from_dataOut_SIZE(frame + 1);
        Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(frame).data;
        ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(frame).context;
        ASSERT_EQ(outBuffer.getSize(), expectedFrameSize);
        ASSERT_EQ(this->fromPortHistory_dataOut->at(frame).context.get_vcId(), context.get_vcId());

        U16 outScId = this->getFrameScId(outBuffer.getData());
        U8 outVcId = this->getFrameVcId(outBuffer.getData());
        U8 outTfVn = this->getFrameTfVn(outBuffer.getData());
        U32 outVcCount = this->getFrameVcCount(outBuffer.getData());
        U16 outFramePointer = this->getFramePacketPointer(outBuffer.getData());

        const U8 expectedTfVn = 0b01;
        ASSERT_EQ(outTfVn, expectedTfVn);
        const U16 expectedScId = ComCfg::SpacecraftId;
        ASSERT_EQ(outScId, expectedScId);
        ASSERT_EQ(outVcId, context.get_vcId());
        ASSERT_EQ(outVcCount, frame);
        ASSERT_EQ(this->component.m_vcs[0].virtualFrameCount, outVcCount + 1);

        // Check in on the M_PDU
        if (frame == 0) {
            // First Frame is the start of our big buffer (no idle padding)
            ASSERT_EQ(outFramePointer, 0);
        } else if (frame == 1) {
            // Second Frame is an exclusively continuing packet (M_PDU pointer is all ones)
            ASSERT_EQ(outFramePointer, 0xFFFF);
        } else {
            // Third Frame is the final (w/ a ton of idle padding)

            const U32 payloadPerFrame = expectedFrameSize - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE -
                                        AOSTrailer::SERIALIZED_SIZE;
            U16 expectedFramePointer = sizeof(bufferData) % payloadPerFrame;

            ASSERT_EQ(outFramePointer, expectedFramePointer);

            // The frame is composed of the payload + a SpacePacket Idle Packet (Header + idle_pattern)
            const U8 idlePattern = this->component.SPP_IDLE_DATA_PATTERN;
            const FwSizeType ideDataEndOffset = ComCfg::AosMaxFrameFixedSize - AOSTrailer::SERIALIZED_SIZE;
            for (FwSizeType i = expectedFramePointer; i < ideDataEndOffset; ++i) {
                ASSERT_EQ(outBuffer.getData()[i], idlePattern)
                    << "Idle data at index " << i << " does not match expected idle pattern";
            }
        }

        // Return this buffer so the framer can reset
        this->invoke_to_dataReturnIn(0, outBuffer, outContext);
    }
}

void AosFramerTester ::testShortPackets() {
    U8 bufferData[100];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));

    ComCfg::FrameContext defaultContext;
    defaultContext.set_sendNow(true);

    // Fill the buffer with some data
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Invoke the dataIn handler
    this->invoke_to_dataIn(0, buffer, defaultContext);

    // Check that the dataOut handler was called with the correct data
    ASSERT_from_dataOut_SIZE(1);
    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ComCfg::FrameContext outContext = this->fromPortHistory_dataOut->at(0).context;
    const FwSizeType expectedFrameSize = ComCfg::AosMaxFrameFixedSize;
    ASSERT_EQ(outBuffer.getSize(), expectedFrameSize);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_vcId(), defaultContext.get_vcId());

    U16 outScId = this->getFrameScId(outBuffer.getData());
    U8 outVcId = this->getFrameVcId(outBuffer.getData());
    U8 outTfVn = this->getFrameTfVn(outBuffer.getData());
    U32 outVcCount = this->getFrameVcCount(outBuffer.getData());

    const U8 expectedTfVn = 0b01;
    ASSERT_EQ(outTfVn, expectedTfVn);
    const U16 expectedScId = ComCfg::SpacecraftId;
    ASSERT_EQ(outScId, expectedScId);
    ASSERT_EQ(outVcId, defaultContext.get_vcId());
    ASSERT_EQ(outVcCount, 0);
    ASSERT_EQ(this->component.m_vcs[0].virtualFrameCount, outVcCount + 1);

    // Idle data should be filled at the offset of header + payload + the Space Packet Idle Packet header
    FwSizeType expectedIdleDataOffset = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE + sizeof(bufferData) +
                                        SpacePacketHeader::SERIALIZED_SIZE;

    // The frame is composed of the payload + a SpacePacket Idle Packet (Header + idle_pattern)
    const U8 idlePattern = this->component.SPP_IDLE_DATA_PATTERN;
    const FwSizeType ideDataEndOffset = ComCfg::AosMaxFrameFixedSize - AOSTrailer::SERIALIZED_SIZE;
    for (FwSizeType i = expectedIdleDataOffset; i < ideDataEndOffset; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], idlePattern)
            << "Idle data at index " << i << " does not match expected idle pattern";
    }
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

U8 AosFramerTester::getFrameTfVn(U8* frameData) {
    // Most 2 bits of 1st octet
    return static_cast<U8>(frameData[0] & 0xC0) >> 6;
}

U16 AosFramerTester::getFrameScId(U8* frameData) {
    U16 scid = 0;

    scid |= static_cast<U16>(frameData[1]) >> 6;
    scid |= static_cast<U16>(frameData[0] & 0x3F) << 2;
    scid |= static_cast<U16>(frameData[5] & 0x30) << (8 - 4);

    return scid;
}

U8 AosFramerTester::getFrameVcId(U8* frameData) {
    // Least 6 bits of 2nd octet
    return static_cast<U8>(frameData[1] & 0x3F);
}

U32 AosFramerTester::getFrameVcCount(U8* frameData) {
    // 3 octets at 3rd octet
    U32 vc_count = 0;

    vc_count |= static_cast<U32>(frameData[2]) << 16;
    vc_count |= static_cast<U32>(frameData[3]) << 8;
    vc_count |= static_cast<U32>(frameData[4]) << 0;

    // VC Frame Count Cycle in use flag
    if (frameData[5] & 0x40) {
        // Lowest 4 bits of 6th octet
        vc_count |= static_cast<U32>(frameData[5] & 0x0F) << 24;
    }

    return vc_count;
}

U16 AosFramerTester::getFramePacketPointer(U8* frameData) {
    // 2 octets at 7th octet
    U16 offset = 0;

    offset |= static_cast<U16>(frameData[6]) << 8;
    offset |= static_cast<U16>(frameData[7]) << 0;

    return offset;
}

}  // namespace Ccsds

}  // namespace Svc
