// On-wire regression vectors independent of createEppPacket and sizeEppPacket.
#include "AosDeframerTester.hpp"

namespace Svc {
namespace Ccsds {

void AosDeframerTester::testEppConformantAdjacentLengths() {
    this->configureDefault();
    this->clearHistory();
    // EPI=7; lengths 5, 7 and 11 include their 2-, 4- and 8-byte headers.
    // A final SPP packet checks the boundary between the two packet protocols.
    U8 payload[] = {0xFD, 5, 0xA1, 0xA2, 0xA3, 0xFE, 0,    0,    7,    0xB1, 0xB2, 0xB3, 0xFF, 0,    0,    0,
                    0,    0, 0,    11,   0xC1, 0xC2, 0xC3, 0x01, 0x11, 0xC0, 0,    0,    2,    0xD1, 0xD2, 0xD3};
    const FwSizeType sizes[] = {5, 7, 11, 9};
    this->m_expectedPacketBytes = payload;
    this->m_expectedPacketSize = sizeof payload;
    Fw::Buffer frame = this->assembleFrameBuffer(payload, sizeof payload, 0);
    ComCfg::FrameContext context;
    this->invoke_to_dataIn(0, frame, context);

    ASSERT_EQ(this->fromPortHistory_dataOut->size(), 4U);
    for (U32 i = 0; i < 4; ++i) {
        ASSERT_EQ(this->fromPortHistory_dataOut->at(i).data.getSize(), sizes[i]);
        ASSERT_EQ(this->fromPortHistory_dataOut->at(i).context.get_pvn(),
                  i < 3 ? ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL : ComCfg::Pvn::SPACE_PACKET_PROTOCOL);
    }
    ASSERT_EQ(this->m_checkedPacketBytes, sizeof payload);
    ASSERT_from_dataReturnOut_SIZE(1);
    ASSERT_EVENTS_SIZE(0);
    this->assertDataOutVcId(0);
}

void AosDeframerTester::testEppConformantLengthBoundaries() {
    // Largest one-octet length and first length requiring two octets.
    for (FwSizeType packetSize = 255; packetSize <= 256; ++packetSize) {
        SCOPED_TRACE(::testing::Message() << "packetSize=" << packetSize);
        this->configureDefault();
        this->clearHistory();
        U8 packet[256];
        ::memset(packet, 0xA5, sizeof packet);
        if (packetSize == 255) {
            packet[0] = 0xFD;
            packet[1] = 0xFF;
        } else {
            packet[0] = 0xFE;
            packet[1] = 0;
            packet[2] = 1;
            packet[3] = 0;
        }
        this->m_expectedPacketBytes = packet;
        this->m_expectedPacketSize = packetSize;
        this->m_checkedPacketBytes = 0;
        ComCfg::FrameContext context;
        Fw::Buffer first = this->assembleFrameBuffer(packet, TEST_DATA_ZONE_SIZE, 0, ComCfg::SpacecraftId, 0, 0);
        this->invoke_to_dataIn(0, first, context);
        ASSERT_EQ(this->fromPortHistory_dataOut->size(), 0U);
        Fw::Buffer second = this->assembleFrameBuffer(packet + TEST_DATA_ZONE_SIZE, packetSize - TEST_DATA_ZONE_SIZE,
                                                      M_PDUSubfields::FHP_NO_PACKET_START, ComCfg::SpacecraftId, 0, 1);
        this->invoke_to_dataIn(0, second, context);
        ASSERT_EQ(this->fromPortHistory_dataOut->size(), 1U);
        ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), packetSize);
        ASSERT_EQ(this->m_checkedPacketBytes, packetSize);
        ASSERT_from_dataReturnOut_SIZE(2);
        ASSERT_EVENTS_SIZE(0);
    }
}

void AosDeframerTester::testEppConformantHeaderSplits() {
    const U8 headerSizes[] = {2, 4, 8};
    const U8 firstBytes[] = {0xFD, 0xFE, 0xFF};
    for (U32 variant = 0; variant < 3; ++variant) {
        const FwSizeType headerSize = headerSizes[variant];
        const FwSizeType packetSize = headerSize + 3;
        for (FwSizeType split = 1; split < headerSize; ++split) {
            SCOPED_TRACE(::testing::Message() << "headerSize=" << headerSize << " split=" << split);
            this->configureDefault();
            this->clearHistory();
            // Each possible header split, followed immediately by another packet.
            U8 packets[16] = {};
            packets[0] = firstBytes[variant];
            packets[headerSize - 1] = static_cast<U8>(packetSize);
            packets[headerSize] = 0xA1;
            packets[headerSize + 1] = 0xA2;
            packets[headerSize + 2] = 0xA3;
            const U8 next[] = {0xFD, 5, 0xB1, 0xB2, 0xB3};
            ::memcpy(packets + packetSize, next, sizeof next);
            this->m_expectedPacketBytes = packets;
            this->m_expectedPacketSize = packetSize + sizeof next;
            this->m_checkedPacketBytes = 0;

            U8 payload[TEST_DATA_ZONE_SIZE] = {};
            const FwSizeType fhp = TEST_DATA_ZONE_SIZE - split;
            ::memcpy(payload + fhp, packets, split);
            ComCfg::FrameContext context;
            Fw::Buffer first =
                this->assembleFrameBuffer(payload, sizeof payload, static_cast<U16>(fhp), ComCfg::SpacecraftId, 0, 0);
            this->invoke_to_dataIn(0, first, context);
            ASSERT_EQ(this->fromPortHistory_dataOut->size(), 0U);
            Fw::Buffer second =
                this->assembleFrameBuffer(packets + split, packetSize - split + sizeof next,
                                          static_cast<U16>(packetSize - split), ComCfg::SpacecraftId, 0, 1);
            this->invoke_to_dataIn(0, second, context);
            ASSERT_EQ(this->fromPortHistory_dataOut->size(), 2U);
            ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), packetSize);
            ASSERT_EQ(this->fromPortHistory_dataOut->at(1).data.getSize(), sizeof next);
            ASSERT_EQ(this->m_checkedPacketBytes, packetSize + sizeof next);
            ASSERT_from_dataReturnOut_SIZE(2);
            ASSERT_EVENTS_SIZE(0);
        }
    }
}

void AosDeframerTester::testEppInvalidDeclaredLengths() {
    const U8 headerSizes[] = {1, 2, 4, 8};
    const U8 firstBytes[] = {0xFC, 0xFD, 0xFE, 0xFF};
    for (U32 variant = 0; variant < 4; ++variant) {
        const FwSizeType headerSize = headerSizes[variant];
        // Header-only non-idle packets are invalid per section 4.1.3.1.5.
        const FwSizeType maxLength = variant == 0 ? 0 : headerSize;
        for (FwSizeType declared = 0; declared <= maxLength; ++declared) {
            SCOPED_TRACE(::testing::Message() << "headerSize=" << headerSize << " declared=" << declared);
            this->configureDefault();
            this->clearHistory();
            this->m_allocationCalls = 0;
            U8 payload[8] = {};
            payload[0] = firstBytes[variant];
            if (variant != 0) {
                payload[headerSize - 1] = static_cast<U8>(declared);
            }
            ComCfg::FrameContext context;
            Fw::Buffer frame = this->assembleFrameBuffer(payload, headerSize, 0, ComCfg::SpacecraftId, 0, 0);
            this->invoke_to_dataIn(0, frame, context);
            ASSERT_EQ(this->fromPortHistory_dataOut->size(), 0U);
            ASSERT_EQ(this->m_allocationCalls, 0U);
            ASSERT_from_dataReturnOut_SIZE(1);

            // An invalid length must not prevent recovery at the next known boundary.
            U8 valid[] = {0xFD, 5, 0x81, 0x82, 0x83};
            frame = this->assembleFrameBuffer(valid, sizeof valid, 0, ComCfg::SpacecraftId, 0, 1);
            this->invoke_to_dataIn(0, frame, context);
            ASSERT_EQ(this->fromPortHistory_dataOut->size(), 1U);
            ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), sizeof valid);
            ASSERT_EQ(::memcmp(this->fromPortHistory_dataOut->at(0).data.getData(), valid, sizeof valid), 0);
            ASSERT_from_dataReturnOut_SIZE(2);
        }
    }
}

void AosDeframerTester::testEppHelperEncodesTotalLength() {
    const EppLengthOfLength variants[] = {EppLengthOfLength::One, EppLengthOfLength::Two, EppLengthOfLength::Four};
    const U8 headerSizes[] = {2, 4, 8};
    const U8 firstBytes[] = {0xFD, 0xFE, 0xFF};
    for (U32 variant = 0; variant < 3; ++variant) {
        SCOPED_TRACE(::testing::Message() << "variant=" << variant);
        U8 packet[13] = {};
        const FwSizeType size = this->createEppPacket(packet, EppProtocolId::MissionSpecific, variants[variant], 5);
        ASSERT_EQ(size, static_cast<FwSizeType>(headerSizes[variant] + 5));
        ASSERT_EQ(packet[0], firstBytes[variant]);
        ASSERT_EQ(packet[headerSizes[variant] - 1], size);
        for (FwSizeType i = 1; i + 1 < headerSizes[variant]; ++i) {
            ASSERT_EQ(packet[i], 0U);
        }
    }
}

void AosDeframerTester::testEppConformantAllocationFailure() {
    this->configureDefault();
    this->clearHistory();
    U8 packets[] = {0xFD, 5, 0xA1, 0xA2, 0xA3, 0xFE, 0, 0, 7, 0xB1, 0xB2, 0xB3};
    this->m_expectedPacketBytes = packets + 5;
    this->m_expectedPacketSize = 7;
    this->m_failNextAlloc = true;
    Fw::Buffer frame = this->assembleFrameBuffer(packets, sizeof packets, 0);
    ComCfg::FrameContext context;
    this->invoke_to_dataIn(0, frame, context);
    ASSERT_EQ(this->fromPortHistory_dataOut->size(), 1U);
    ASSERT_EQ(this->fromPortHistory_dataOut->at(0).data.getSize(), 7U);
    ASSERT_EQ(this->m_checkedPacketBytes, 7U);
    ASSERT_EVENTS_SpanningPacketAllocFailed_SIZE(1);
    ASSERT_EVENTS_SpanningPacketAllocFailed(0, 0, ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL, 5);
    ASSERT_from_dataReturnOut_SIZE(1);
}

}  // namespace Ccsds
}  // namespace Svc
