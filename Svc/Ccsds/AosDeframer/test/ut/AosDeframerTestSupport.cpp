// ======================================================================
// \title  AosDeframerTestSupport.cpp
// \author Codex
// \brief  helper function definitions for AosDeframer unit tests
// ======================================================================

#include "AosDeframerTester.hpp"
#include "Svc/Ccsds/Types/AOSHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/AOSTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Types/M_PDUHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"

namespace Svc {

namespace Ccsds {

Fw::Buffer AosDeframerTester::from_allocate_handler(FwIndexType portNum, FwSizeType size) {
    (void)portNum;
    if (m_failNextAlloc) {
        m_failNextAlloc = false;
        return Fw::Buffer();
    }
    if (size <= ALLOC_BUF_SIZE) {
        return Fw::Buffer(this->m_allocBuf, size);
    }
    return Fw::Buffer();
}

void AosDeframerTester::configureDefault() {
    this->component.configure(TEST_FRAME_SIZE, true, ComCfg::SpacecraftId, 0,
                              PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK);
}

Fw::Buffer AosDeframerTester::assembleFrameBuffer(U8* payload,
                                                  FwSizeType payloadLength,
                                                  U16 fhp,
                                                  U16 scid,
                                                  U8 vcid,
                                                  U32 vcCount,
                                                  U8 tfvn,
                                                  bool includeFecf) {
    const U32 frameSize = includeFecf ? TEST_FRAME_SIZE : (TEST_FRAME_SIZE - AOSTrailer::SERIALIZED_SIZE);
    ::memset(this->m_frameData, 0, sizeof(this->m_frameData));

    // Build AOS Primary Header (6 bytes)
    // Byte 0-1: globalVcId (2b TFVN | 8b SCID LSB | 6b VCID)

    const U16 globalVcId = static_cast<U16>(
        static_cast<U16>((tfvn & 0x3) << AOSHeaderSubfields::frameVersionOffset) |
        static_cast<U16>((scid & 0xFF) << AOSHeaderSubfields::spacecraftIdLsbOffset) | static_cast<U16>(vcid & 0x3F));
    this->m_frameData[0] = static_cast<U8>(globalVcId >> 8);
    this->m_frameData[1] = static_cast<U8>(globalVcId & 0xFF);

    // Byte 2-4: VC Frame Count (24 bits)
    this->m_frameData[2] = static_cast<U8>((vcCount >> 16) & 0xFF);
    this->m_frameData[3] = static_cast<U8>((vcCount >> 8) & 0xFF);
    this->m_frameData[4] = static_cast<U8>(vcCount & 0xFF);

    // Byte 5: Signaling field (replay | cycle use | SCID MSB | VC cycle)

    const U8 signaling =
        static_cast<U8>(static_cast<U8>(1 << AOSHeaderSubfields::cycleCountFlagOffset) |  // Cycle count in use
                        static_cast<U8>(((scid >> 8) & 0x3) << AOSHeaderSubfields::spacecraftIdMsbOffset) |
                        static_cast<U8>((vcCount >> 24) & 0x0F));  // Cycl
    this->m_frameData[5] = signaling;

    // Byte 6-7: M_PDU Header (First Header Pointer)
    this->m_frameData[6] = static_cast<U8>(fhp >> 8);
    this->m_frameData[7] = static_cast<U8>(fhp & 0xFF);

    // Copy payload to data zone
    const FwSizeType dataZoneStart = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE;
    const FwSizeType dataZoneEnd = frameSize - (includeFecf ? AOSTrailer::SERIALIZED_SIZE : 0);
    FwSizeType maxPayload = dataZoneEnd - dataZoneStart;
    FwSizeType copyLen = FW_MIN(payloadLength, maxPayload);
    ::memcpy(this->m_frameData + dataZoneStart, payload, copyLen);

    // Fill remaining data zone with an EPP fill packet (protocolId=0, lengthOfLength=0)
    // This prevents interpretation of zeros as valid SPP packets
    FwSizeType fillStart = dataZoneStart + copyLen;
    if (fillStart < dataZoneEnd) {
        this->createEppPacket(this->m_frameData + fillStart, EppProtocolId::Idle, EppLengthOfLength::Zero, 0);
    }

    // Add FECF if enabled
    if (includeFecf) {
        U16 crc = Ccsds::Utils::CRC16::compute(this->m_frameData, frameSize - AOSTrailer::SERIALIZED_SIZE);
        this->m_frameData[frameSize - 2] = static_cast<U8>(crc >> 8);
        this->m_frameData[frameSize - 1] = static_cast<U8>(crc & 0xFF);
    }

    return Fw::Buffer(this->m_frameData, frameSize);
}

FwSizeType AosDeframerTester::createSppPacket(U8* buffer, U16 apid, U16 dataLength, U16 seqCount) {
    const U16 packetIdentification =
        static_cast<U16>((ComCfg::Pvn::SPACE_PACKET_PROTOCOL << SpacePacketSubfields::PvnOffset) |
                         (apid & SpacePacketSubfields::ApidMask));
    const U16 packetSequenceControl = static_cast<U16>((0x3 << SpacePacketSubfields::SeqFlagsOffset) |
                                                       (seqCount & SpacePacketSubfields::SeqCountMask));

    const U16 packetDataLength = static_cast<U16>(dataLength - 1);

    SpacePacketHeader header(packetIdentification, packetSequenceControl, packetDataLength);
    Fw::ExternalSerializeBuffer serializer(buffer, SpacePacketHeader::SERIALIZED_SIZE);
    FW_ASSERT(header.serializeTo(serializer) == Fw::FW_SERIALIZE_OK);

    for (U16 i = 0; i < dataLength; i++) {
        buffer[SpacePacketHeader::SERIALIZED_SIZE + i] = static_cast<U8>(i & 0xFF);
    }

    return SpacePacketHeader::SERIALIZED_SIZE + dataLength;
}

FwSizeType AosDeframerTester::createEppPacket(U8* buffer,
                                              U8 protocolId,
                                              EppLengthOfLength lengthOfLength,
                                              const FwSizeType dataLength) {
    // EPP Packet per CCSDS 133.1-B-3 Section 4.1.2 / 4.1.3.2
    // Byte 0: 3b PVN=7 | 3b protocolId | 2b lengthOfLength
    // protocolId=0 (EppProtocolId::Idle) produces an EPP idle/fill packet

    buffer[0] = static_cast<U8>(
        static_cast<U8>(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL << EPPSubfields::packetVersionOffset) |
        static_cast<U8>((protocolId & EPPSubfields::protocolIdMask) << EPPSubfields::protocolIdOffset) |
        static_cast<U8>(lengthOfLength & EPPSubfields::lengthOfLengthMask));

    if (lengthOfLength == EppLengthOfLength::Zero) {
        return 1;
    }

    // Numerical meaning (not wire version) of length of length
    U8 lol = lengthOfLength;

    FwSizeType offset = 1;

    // Extension byte for lengthOfLength >= 2 (per CCSDS 133.1-B-3 Section 4.1.2.1.1)
    if (lengthOfLength >= EppLengthOfLength::Two) {
        buffer[offset++] = 0x00;
    }

    // Two CCSDS reserved bytes for lengthOfLength == 4
    if (lengthOfLength == EppLengthOfLength::Four) {
        buffer[offset++] = 0x00;
        buffer[offset++] = 0x00;

        // 3 on the wire, but means 4
        lol = 4;
    }

    // Length field (big-endian)
    for (U8 i = 0; i < lol; i++) {
        buffer[offset++] = static_cast<U8>(dataLength >> (8 * (lol - i - 1)) & 0xFF);
    }

    // Fill data
    for (FwSizeType i = 0; i < dataLength; i++) {
        buffer[offset++] = 0x55;
    }

    return offset;
}

}  // namespace Ccsds

}  // namespace Svc
