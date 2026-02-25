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
    if (size <= ALLOC_BUF_SIZE) {
        return Fw::Buffer(this->m_allocBuf, size);
    }
    return Fw::Buffer();
}

void AosDeframerTester::from_deallocate_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    (void)portNum;
    (void)fwBuffer;
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
    U16 globalVcId = static_cast<U16>((tfvn & 0x3) << AOSHeaderSubfields::frameVersionOffset);
    globalVcId |= static_cast<U16>((scid & 0xFF) << AOSHeaderSubfields::spacecraftIdLsbOffset);
    globalVcId |= static_cast<U16>(vcid & 0x3F);
    this->m_frameData[0] = static_cast<U8>(globalVcId >> 8);
    this->m_frameData[1] = static_cast<U8>(globalVcId & 0xFF);

    // Byte 2-4: VC Frame Count (24 bits)
    this->m_frameData[2] = static_cast<U8>((vcCount >> 16) & 0xFF);
    this->m_frameData[3] = static_cast<U8>((vcCount >> 8) & 0xFF);
    this->m_frameData[4] = static_cast<U8>(vcCount & 0xFF);

    // Byte 5: Signaling field (replay | cycle use | SCID MSB | VC cycle)
    U8 signaling = 0;
    signaling |= static_cast<U8>(1 << AOSHeaderSubfields::cycleCountFlagOffset);  // Cycle count in use
    signaling |= static_cast<U8>(((scid >> 8) & 0x3) << AOSHeaderSubfields::spacecraftIdMsbOffset);
    signaling |= static_cast<U8>((vcCount >> 24) & 0x0F);  // Cycle count
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

    // Fill remaining data zone with EPP fill packet (type=1, lengthOfLength=0)
    // This prevents interpretation of zeros as valid SPP packets
    FwSizeType fillStart = dataZoneStart + copyLen;
    if (fillStart < dataZoneEnd) {
        // EPP fill packet header: PVN=7, type=1, lengthOfLength=0
        // This is 0xF0 = (7<<5) | (1<<4) | 0
        this->m_frameData[fillStart] = 0xF0;
        // Fill rest with idle pattern
        for (FwSizeType i = fillStart + 1; i < dataZoneEnd; i++) {
            this->m_frameData[i] = 0x55;  // Idle fill pattern
        }
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
    // SPP Header (6 bytes)
    // Byte 0-1: Packet Identification (3b PVN=0 | 1b type | 1b sec hdr | 11b APID)
    U16 pktId = apid & 0x07FF;  // APID in lower 11 bits, PVN=0 in upper bits
    buffer[0] = static_cast<U8>(pktId >> 8);
    buffer[1] = static_cast<U8>(pktId & 0xFF);

    // Byte 2-3: Packet Sequence Control (2b flags | 14b seq count)
    U16 seqCtrl = static_cast<U16>((0x3 << 14) | (seqCount & 0x3FFF));  // Unsegmented
    buffer[2] = static_cast<U8>(seqCtrl >> 8);
    buffer[3] = static_cast<U8>(seqCtrl & 0xFF);

    // Byte 4-5: Packet Data Length (actual length - 1)
    U16 lengthField = static_cast<U16>(dataLength > 0 ? dataLength - 1 : 0);
    buffer[4] = static_cast<U8>(lengthField >> 8);
    buffer[5] = static_cast<U8>(lengthField & 0xFF);

    // Fill data with pattern
    for (U16 i = 0; i < dataLength; i++) {
        buffer[6 + i] = static_cast<U8>(i & 0xFF);
    }

    return SpacePacketHeader::SERIALIZED_SIZE + dataLength;
}

FwSizeType AosDeframerTester::createEppPacket(U8* buffer, U8 protocolId, U16 dataLength) {
    // EPP Header
    // Byte 0: 3b PVN=7 | 1b type=0 | 4b protocol ID
    buffer[0] = static_cast<U8>((7 << 5) | (protocolId & 0x0F));

    // Byte 1-2: Length field (for standard protocol IDs)
    buffer[1] = static_cast<U8>(dataLength >> 8);
    buffer[2] = static_cast<U8>(dataLength & 0xFF);

    // Fill data with pattern
    for (U16 i = 0; i < dataLength; i++) {
        buffer[3 + i] = static_cast<U8>((i + 0x80) & 0xFF);
    }

    return 3 + dataLength;
}

FwSizeType AosDeframerTester::createEppIdlePacket(U8* buffer, U8 lengthOfLength, FwSizeType packetLength) {
    // EPP Idle Header
    // Byte 0: 3b PVN=7 | 1b type=1 | 4b length of length
    buffer[0] = static_cast<U8>((7 << 5) | (1 << 4) | (lengthOfLength & 0x0F));

    if (lengthOfLength == 0) {
        // Fill packet - just header byte
        return 1;
    }

    // Write length field
    FwSizeType offset = 1;
    for (U8 i = 0; i < lengthOfLength; i++) {
        U8 shift = static_cast<U8>((lengthOfLength - 1 - i) * 8);
        buffer[offset++] = static_cast<U8>((packetLength >> shift) & 0xFF);
    }

    // Fill with idle pattern
    for (FwSizeType i = 0; i < packetLength; i++) {
        buffer[offset++] = 0x55;
    }

    return 1 + lengthOfLength + packetLength;
}

U8 AosDeframerTester::getFrameTfvn(U8* frameData) {
    return static_cast<U8>((frameData[0] >> 6) & 0x03);
}

U16 AosDeframerTester::getFrameScId(U8* frameData) {
    U16 scidLsb = static_cast<U16>(((frameData[0] & 0x3F) << 2) | (frameData[1] >> 6));
    U16 scidMsb = static_cast<U16>((frameData[5] >> 4) & 0x03);
    return static_cast<U16>(scidLsb | (scidMsb << 8));
}

U8 AosDeframerTester::getFrameVcId(U8* frameData) {
    return static_cast<U8>(frameData[1] & 0x3F);
}

U32 AosDeframerTester::getFrameVcCount(U8* frameData) {
    U32 count = 0;
    count |= static_cast<U32>(frameData[2]) << 16;
    count |= static_cast<U32>(frameData[3]) << 8;
    count |= static_cast<U32>(frameData[4]);

    if (frameData[5] & 0x40) {  // Cycle count in use
        count |= static_cast<U32>(frameData[5] & 0x0F) << 24;
    }

    return count;
}

U16 AosDeframerTester::getFrameFhp(U8* frameData) {
    return static_cast<U16>((frameData[6] << 8) | frameData[7]);
}

}  // namespace Ccsds

}  // namespace Svc
