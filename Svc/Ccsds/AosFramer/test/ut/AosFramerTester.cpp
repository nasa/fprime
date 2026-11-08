// ======================================================================
// \title  AosFramerTester.cpp
// \author Will MacCormack (Aos Modifications)
// \brief  cpp file for AosFramer component test harness implementation class
// \details modified from thomas-bc's TmFramer
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

// Many Frames for one packet
void AosFramerTester ::testLongPacket() {
    // This will need 2 full & 1 partial AOS Frames to send
    U8 bufferData[2048];
    Fw::Buffer buffer(bufferData, sizeof(bufferData));

    const FwSizeType frameSize = 1024;
    this->component.configure(frameSize, true);

    ComCfg::FrameContext context;
    // Use sendNow to get that final frame ASAP
    context.set_sendNow(true);

    // Fill the buffer with some data
    // This isn't actually a space packet so this test can't decode it for a length
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Ensure we start w/ no sent frames
    ASSERT_from_dataOut_SIZE(0);

    // Invoke the dataIn handler
    this->invoke_to_dataIn(0, buffer, context);

    // How far into the payload (and what byte we should see there)
    U32 payloadInd = 0;

    for (U8 frame = 0; frame < 3; frame++) {
        ASSERT_from_dataOut_SIZE(frame + 1);
        Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(frame).data;
        ASSERT_EQ(outBuffer.getSize(), frameSize);
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

            const U32 payloadPerFrame =
                frameSize - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;
            U16 expectedFramePointer = sizeof(bufferData) % payloadPerFrame;

            ASSERT_EQ(outFramePointer, expectedFramePointer);
            ASSERT_NE(expectedFramePointer, 0xFFFF);

            // The frame is composed of the payload + a SpacePacket Idle Packet (Header + idle_pattern)
            const U8 idlePattern = this->component.SPP_IDLE_DATA_PATTERN;
            const U16 ideDataEndOffset = frameSize - AOSTrailer::SERIALIZED_SIZE;
            const U16 startOfIdleSPP =
                static_cast<U16>(AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE + outFramePointer);
            const U16 startOfIdleData = static_cast<U16>(startOfIdleSPP + SpacePacketHeader::SERIALIZED_SIZE);

            // Check the SPP header
            SpacePacketHeader spp;
            auto payloadDeserializer = outBuffer.getDeserializer();
            Fw::SerializeStatus status = payloadDeserializer.moveDeserToOffset(startOfIdleSPP);
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

            status = payloadDeserializer.deserializeTo(spp);
            ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

            ASSERT_EQ(spp.get_packetIdentification(), ComCfg::Apid::SPP_IDLE_PACKET);
            ASSERT_EQ(spp.get_packetSequenceControl(), 0x3 << SpacePacketSubfields::SeqFlagsOffset);

            // Token == SPP's payload length - 1
            const U16 expectedLengthToken = static_cast<U16>(ideDataEndOffset - startOfIdleData - 1);
            ASSERT_EQ(spp.get_packetDataLength(), expectedLengthToken);

            // Check the Idle SPP payload
            for (FwSizeType i = startOfIdleData; i < ideDataEndOffset; ++i) {
                ASSERT_EQ(outBuffer.getData()[i], idlePattern)
                    << "Idle data at index " << i << " in range (" << startOfIdleData << ", " << ideDataEndOffset << ")"
                    << " does not match expected idle pattern";
            }
        }

        // Using U32 so I can unwind the addition of U16 max
        U32 payloadStart = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE + outFramePointer;
        U32 payloadStop = frameSize - AOSTrailer::SERIALIZED_SIZE;
        if (frame == 1) {
            // Payload spans whole Frame Payload
            payloadStart -= outFramePointer;
        } else if (frame == 2) {
            // Payload stops at pointer
            payloadStop = payloadStart;
            payloadStart -= outFramePointer;
        }

        for (U32 offset = payloadStart; offset < payloadStop; offset++) {
            ASSERT_EQ(outBuffer.getData()[offset], static_cast<U8>(payloadInd++))
                << "Payload data in frame " << static_cast<U16>(frame) << " at " << offset << " in range ("
                << payloadStart << ", " << payloadStop << ")"
                << " does not match expected idle pattern";
        }

        // Behave as the adapter: return the frame, then report its status. The statuses of the
        // first two frames are consumed to continue the striped packet; only the final frame's
        // status is forwarded, so upstream sees exactly one status for its one message (#5888)
        this->ackFrame(frame);
        ASSERT_from_comStatusOut_SIZE((frame == 2) ? 1 : 0);
    }

    // Make sure we don't send any extra frames (continue into null)
    ASSERT_from_dataOut_SIZE(3);
    Fw::Success expectedStatus = Fw::Success::SUCCESS;
    ASSERT_from_comStatusOut(0, expectedStatus);
}

// Many Packets for one frame
// Also end w/ less than 7 bytes to test that path
void AosFramerTester ::testShortPackets() {
    // Slice this buffer into many packets that all fit into one frame
    U8 bufferData[1008];

    const FwSizeType frameSize = 1024;
    this->component.configure(frameSize, true);

    ComCfg::FrameContext context;
    // Hold off on sendNow until the final packet
    context.set_sendNow(false);

    // Fill the buffer with some data
    // This isn't actually a space packet so this test can't decode it for a length
    for (U32 i = 0; i < sizeof(bufferData); ++i) {
        bufferData[i] = static_cast<U8>(i);
    }

    // Ensure we start w/ no sent frames
    ASSERT_from_dataOut_SIZE(0);

    // Invoke the dataIn handler
    const U8 packets = 10;
    const FwSizeType packetSize = sizeof(bufferData) / packets;
    for (U8 packet = 0; packet < packets; packet++) {
        // Shouldn't send until we are done with packet #10
        ASSERT_from_dataOut_SIZE(0);

        FwSizeType currentPacketSize = packetSize;

        if (packet + 1 == packets) {
            // sendNow the final packet
            context.set_sendNow(true);

            // make sure we use up the whole buffer
            // even if it is not a multiple of packets
            currentPacketSize = sizeof(bufferData) - packet * packetSize;
        }

        Fw::Buffer buffer(bufferData + packet * packetSize, currentPacketSize);

        this->invoke_to_dataIn(0, buffer, context);
    }
    ASSERT_from_dataOut_SIZE(1);

    // How far into the payload (and what byte we should see there)
    U32 payloadInd = 0;

    Fw::Buffer outBuffer = this->fromPortHistory_dataOut->at(0).data;
    ASSERT_EQ(outBuffer.getSize(), frameSize);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).context.get_vcId(), context.get_vcId());

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
    ASSERT_EQ(outVcCount, 0);
    ASSERT_EQ(this->component.m_vcs[0].virtualFrameCount, outVcCount + 1);

    // Check in on the M_PDU
    ASSERT_EQ(outFramePointer, 0);

    // The frame is composed of the payload + a SpacePacket Idle Packet (Header + idle_pattern)
    const U8 idlePattern = this->component.SPP_IDLE_DATA_PATTERN;
    const U16 ideDataEndOffset = frameSize - AOSTrailer::SERIALIZED_SIZE;
    // Cheat and grab the length of the payload (real deframer would decoded each SPP one at a time)
    const U16 startOfIdleSPP = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE + sizeof(bufferData);
    const U16 startOfIdleData = startOfIdleSPP + SpacePacketHeader::SERIALIZED_SIZE;

    // Check the SPP header
    SpacePacketHeader spp;
    auto payloadDeserializer = outBuffer.getDeserializer();
    Fw::SerializeStatus status = payloadDeserializer.moveDeserToOffset(startOfIdleSPP);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    status = payloadDeserializer.deserializeTo(spp);
    ASSERT_EQ(status, Fw::FW_SERIALIZE_OK);

    ASSERT_EQ(spp.get_packetIdentification(), ComCfg::Apid::SPP_IDLE_PACKET);
    ASSERT_EQ(spp.get_packetSequenceControl(), 0x3 << SpacePacketSubfields::SeqFlagsOffset);

    // Token == SPP's payload length - 1
    I32 expectedLengthToken = ideDataEndOffset - startOfIdleData - 1;
    if (expectedLengthToken < 0) {
        expectedLengthToken = 0;
    }

    ASSERT_EQ(spp.get_packetDataLength(), expectedLengthToken);

    // Check the Idle SPP payload
    for (FwSizeType i = startOfIdleData; i < ideDataEndOffset; ++i) {
        ASSERT_EQ(outBuffer.getData()[i], idlePattern)
            << "Idle data at index " << i << " in range (" << startOfIdleData << ", " << ideDataEndOffset << ")"
            << " does not match expected idle pattern";
    }

    // Using U32 so I can unwind the addition of U16 max
    U32 payloadStart = static_cast<U32>(AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE + outFramePointer);
    U32 payloadStop = static_cast<U32>(payloadStart + sizeof(bufferData));

    for (U32 offset = payloadStart; offset < payloadStop; offset++) {
        ASSERT_EQ(outBuffer.getData()[offset], static_cast<U8>(payloadInd++))
            << "Payload data in frame " << static_cast<U16>(0) << " at " << offset << " in range (" << payloadStart
            << ", " << payloadStop << ")"
            << " does not match expected idle pattern";
    }

    // Each of the nine packets that did not produce a frame was answered with one SUCCESS directly
    ASSERT_from_comStatusOut_SIZE(9);

    // Behave as the adapter: return the frame, then report its status. The 6-byte idle remainder
    // striped an SPP idle tail onto the next frame, so the acked SUCCESS is consumed to finish it
    // and check_and_send_vc emits the tenth SUCCESS — ten messages, ten SUCCESSes upstream (#5888)
    this->ackFrame(0);
    ASSERT_from_comStatusOut_SIZE(10);
    Fw::Success expectedStatus = Fw::Success::SUCCESS;
    ASSERT_from_comStatusOut(9, expectedStatus);

    // Make sure we don't send any extra frames (continue into null)
    ASSERT_from_dataOut_SIZE(1);
}

// ----------------------------------------------------------------------
// Status Protocol Tests
// ----------------------------------------------------------------------

void AosFramerTester ::testTailPackingSingleStatus() {
    // The repro from #5888: a packet whose tail spans into a second frame must yield exactly one
    // status upstream, not one per transmitted frame
    const FwSizeType frameSize = 1024;
    this->component.configure(frameSize, true);
    const FwSizeType maxPayload =
        frameSize - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;

    U8 bufferData[2048];
    const FwSizeType packetSize = maxPayload + 86;
    ASSERT_LE(packetSize, sizeof(bufferData));
    for (FwSizeType i = 0; i < packetSize; ++i) {
        bufferData[i] = static_cast<U8>(i);
    }
    Fw::Buffer buffer(bufferData, packetSize);
    ComCfg::FrameContext context;
    context.set_sendNow(true);

    // The packet fills the first frame, which is sent; nothing goes upstream yet, and the message
    // buffer stays with the framer while its tail is outstanding
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(0);

    // The adapter returns the frame and reports its status: the status is consumed to pack the
    // tail, which (with sendNow) is idle-filled and emitted as the second frame — still nothing
    // upstream, and the fully packed message buffer is returned to its sender
    this->ackFrame(0);
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_comStatusOut_SIZE(0);
    ASSERT_from_dataReturnOut_SIZE(1);
    // and it is the message's own buffer and context that come back
    ASSERT_from_dataReturnOut(0, buffer, context);

    // The second frame's status finds no tail outstanding and is forwarded: exactly one status
    // for the one message
    this->ackFrame(1);
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_comStatusOut_SIZE(1);
    Fw::Success expectedStatus = Fw::Success::SUCCESS;
    ASSERT_from_comStatusOut(0, expectedStatus);

    // Without sendNow, the consumed status leaves the tail in a partial frame and one SUCCESS is
    // emitted upstream asking for more data: still exactly one status for the message
    ComCfg::FrameContext noSendNow;
    noSendNow.set_sendNow(false);
    this->invoke_to_dataIn(0, buffer, noSendNow);
    // The first frame of this second packet is full and sent; the tail is outstanding
    ASSERT_from_dataOut_SIZE(3);
    this->ackFrame(2);
    // The consumed status packed the tail into a partial frame, which asks upstream for more data;
    // the message buffer is returned once its tail is packed
    ASSERT_from_dataOut_SIZE(3);
    ASSERT_from_comStatusOut_SIZE(2);
    ASSERT_from_comStatusOut(1, expectedStatus);
    ASSERT_from_dataReturnOut_SIZE(2);
    ASSERT_from_dataReturnOut(1, buffer, noSendNow);

    // A SUCCESS for an outstanding tail before the frame buffer has been returned violates the
    // interface ordering (dataReturnOut before the comStatusOut it applies to) the handler asserts on
    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataOut_SIZE(4);
    Fw::Success early = Fw::Success::SUCCESS;
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_comStatusIn(0, early), "AosFramer.cpp");

    // Returning the frame without acking it leaves the tail outstanding: a new message in that
    // state trips the dataIn guard for a topology whose adapter status bypasses the framer
    Fw::Buffer lastFrame = this->fromPortHistory_dataOut->at(3).data;
    ComCfg::FrameContext lastContext = this->fromPortHistory_dataOut->at(3).context;
    this->invoke_to_dataReturnIn(0, lastFrame, lastContext);
    ASSERT_DEATH_IF_SUPPORTED(this->invoke_to_dataIn(0, buffer, context), "AosFramer.cpp");
}

void AosFramerTester ::testTailPackingFailureThenRecovery() {
    // A FAILURE with a tail outstanding is forwarded (pausing the upstream queue) without packing;
    // the recovery SUCCESS is consumed to resume striping the tail
    const FwSizeType frameSize = 1024;
    this->component.configure(frameSize, true);
    const FwSizeType maxPayload =
        frameSize - AOSHeader::SERIALIZED_SIZE - M_PDUHeader::SERIALIZED_SIZE - AOSTrailer::SERIALIZED_SIZE;

    U8 bufferData[2048];
    const FwSizeType packetSize = maxPayload + 86;
    ASSERT_LE(packetSize, sizeof(bufferData));
    for (FwSizeType i = 0; i < packetSize; ++i) {
        bufferData[i] = static_cast<U8>(i);
    }
    Fw::Buffer buffer(bufferData, packetSize);
    ComCfg::FrameContext context;
    context.set_sendNow(true);

    this->invoke_to_dataIn(0, buffer, context);
    ASSERT_from_dataOut_SIZE(1);

    // The adapter returns the frame but reports FAILURE: forwarded, tail left outstanding
    Fw::Buffer frame = this->fromPortHistory_dataOut->at(0).data;
    ComCfg::FrameContext frameContext = this->fromPortHistory_dataOut->at(0).context;
    this->invoke_to_dataReturnIn(0, frame, frameContext);
    Fw::Success failure = Fw::Success::FAILURE;
    this->invoke_to_comStatusIn(0, failure);
    ASSERT_from_dataOut_SIZE(1);
    ASSERT_from_comStatusOut_SIZE(1);
    ASSERT_from_comStatusOut(0, failure);

    // The recovery SUCCESS is consumed to resume the tail, which completes the second frame
    Fw::Success recovery = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, recovery);
    ASSERT_from_dataOut_SIZE(2);
    ASSERT_from_comStatusOut_SIZE(1);

    // The second frame's status is forwarded, resuming the queue
    this->ackFrame(1);
    ASSERT_from_comStatusOut_SIZE(2);
    Fw::Success expectedStatus = Fw::Success::SUCCESS;
    ASSERT_from_comStatusOut(1, expectedStatus);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void AosFramerTester ::ackFrame(FwSizeType frameIndex) {
    Fw::Buffer frame = this->fromPortHistory_dataOut->at(static_cast<U32>(frameIndex)).data;
    ComCfg::FrameContext context = this->fromPortHistory_dataOut->at(static_cast<U32>(frameIndex)).context;
    this->invoke_to_dataReturnIn(0, frame, context);
    Fw::Success status = Fw::Success::SUCCESS;
    this->invoke_to_comStatusIn(0, status);
}

U8 AosFramerTester::getFrameTfVn(U8* frameData) {
    // Most 2 bits of 1st octet
    return static_cast<U8>((frameData[0] & 0xC0) >> 6U);
}

U16 AosFramerTester::getFrameScId(U8* frameData) {
    U16 scid = 0;

    scid = static_cast<U16>(scid | ((static_cast<U32>(frameData[1]) >> 6U) & 0x0003U));
    scid = static_cast<U16>(scid | ((static_cast<U32>(frameData[0]) << 2U) & 0x00FCU));
    scid = static_cast<U16>(scid | ((static_cast<U32>(frameData[5]) << 4U) & 0x0300U));

    return scid;
}

U8 AosFramerTester::getFrameVcId(U8* frameData) {
    // Least 6 bits of 2nd octet
    return static_cast<U8>(frameData[1U] & 0x3FU);
}

U32 AosFramerTester::getFrameVcCount(U8* frameData) {
    // 3 octets at 3rd octet
    U32 vc_count = 0;

    vc_count |= static_cast<U32>(frameData[2U] << 16U);
    vc_count |= static_cast<U32>(frameData[3U] << 8U);
    vc_count |= static_cast<U32>(frameData[4U] << 0U);

    // VC Frame Count Cycle in use flag
    if (frameData[5U] & 0x40U) {
        // Lowest 4 bits of 6th octet
        vc_count |= static_cast<U32>((frameData[5] & 0x0FU) << 24U);
    }

    return vc_count;
}

U16 AosFramerTester::getFramePacketPointer(U8* frameData) {
    // 2 octets at 7th octet
    U16 offset = 0;

    offset = static_cast<U16>(offset | ((static_cast<U32>(frameData[7]) << 0U) & 0x00FFU));
    offset = static_cast<U16>(offset | ((static_cast<U32>(frameData[6]) << 8U) & 0xFF00U));

    return offset;
}

}  // namespace Ccsds

}  // namespace Svc
