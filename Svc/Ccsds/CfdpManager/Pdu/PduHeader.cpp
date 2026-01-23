// ======================================================================
// \title  PduHeader.cpp
// \author campuzan
// \brief  cpp file for CFDP PDU Header
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void Pdu::Header::initialize(Type type,
                              Direction direction,
                              Class txmMode,
                              CfdpEntityId sourceEid,
                              CfdpTransactionSeq transactionSeq,
                              CfdpEntityId destEid) {
    this->m_type = type;
    this->m_version = 1;  // CFDP version is always 1
    this->m_pduType = (type == T_FILE_DATA) ? PDU_TYPE_FILE_DATA : PDU_TYPE_DIRECTIVE;
    this->m_direction = direction;
    this->m_txmMode = txmMode;
    this->m_crcFlag = CRC_NOT_PRESENT;  // CRC not currently supported
    this->m_largeFileFlag = LARGE_FILE_32_BIT;  // 32-bit file sizes
    this->m_segmentationControl = 0;
    this->m_segmentMetadataFlag = 0;
    this->m_pduDataLength = 0;  // To be set later
    this->m_sourceEid = sourceEid;
    this->m_transactionSeq = transactionSeq;
    this->m_destEid = destEid;
}

// Helper function to calculate minimum bytes needed to encode a value
static U8 getValueEncodedSize(U64 value) {
    U8  minSize;
    U64 limit = 0x100;

    for (minSize = 1; minSize < 8 && value >= limit; ++minSize) {
        limit <<= 8;
    }

    return minSize;
}

// Helper function to encode an integer in variable-length format
static Fw::SerializeStatus encodeIntegerInSize(Fw::SerialBuffer& serialBuffer, U64 value, U8 encodeSize) {
    // Encode from MSB to LSB (big-endian)
    for (U8 i = 0; i < encodeSize; ++i) {
        U8 shift = static_cast<U8>((encodeSize - 1 - i) * 8);
        U8 byte = static_cast<U8>((value >> shift) & 0xFF);
        Fw::SerializeStatus status = serialBuffer.serializeFrom(byte);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
    }
    return Fw::FW_SERIALIZE_OK;
}

// Helper function to decode an integer from variable-length format
static U64 decodeIntegerInSize(Fw::SerialBuffer& serialBuffer, U8 decodeSize, Fw::SerializeStatus& status) {
    U64 value = 0;

    // Decode from MSB to LSB (big-endian)
    for (U8 i = 0; i < decodeSize; ++i) {
        U8 byte;
        status = serialBuffer.deserializeTo(byte);
        if (status != Fw::FW_SERIALIZE_OK) {
            return 0;
        }
        value = (value << 8) | byte;
    }

    return value;
}

U32 Pdu::Header::bufferSize() const {
    // Fixed portion: flags(1) + length(2) + eidTsnLengths(1) = 4 bytes
    U32 size = 4;

    // Variable-size entity IDs and transaction sequence number based on actual values
    U8 eidSize = getValueEncodedSize(this->m_sourceEid > this->m_destEid ?
                                      this->m_sourceEid : this->m_destEid);
    U8 tsnSize = getValueEncodedSize(this->m_transactionSeq);

    size += eidSize;      // source EID
    size += tsnSize;      // transaction sequence number
    size += eidSize;      // destination EID

    return size;
}

Fw::SerializeStatus Pdu::Header::toSerialBuffer(Fw::SerialBuffer& serialBuffer) const {
    Fw::SerializeStatus status;

    // Variable-size entity IDs and transaction sequence number based on actual values
    U8 eidSize = getValueEncodedSize(this->m_sourceEid > this->m_destEid ?
                                      this->m_sourceEid : this->m_destEid);
    U8 tsnSize = getValueEncodedSize(this->m_transactionSeq);

    // Byte 0: flags
    // bits 7-5: version (001b = 1)
    // bit 4: pdu_type (0=directive, 1=file data)
    // bit 3: direction (0=toward receiver, 1=toward sender)
    // bit 2: txm_mode (0=ack, 1=unack)
    // bit 1: crc_flag (0=not present, 1=present)
    // bit 0: large_file_flag (0=32-bit, 1=64-bit)
    U8 flags = 0;
    flags |= (this->m_version & 0x07) << 5;
    flags |= (this->m_pduType & 0x01) << 4;
    flags |= (this->m_direction & 0x01) << 3;
    flags |= (this->m_txmMode & 0x01) << 2;
    flags |= (this->m_crcFlag & 0x01) << 1;
    flags |= (this->m_largeFileFlag & 0x01);

    status = serialBuffer.serializeFrom(flags);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Bytes 1-2: PDU data length (big-endian)
    status = serialBuffer.serializeFrom(static_cast<U16>(this->m_pduDataLength));
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Byte 3: eidTsnLengths
    // bit 7: segmentation_control
    // bits 6-4: eid_length - 1 (3 bits)
    // bit 3: segment_metadata_flag
    // bits 2-0: tsn_length - 1 (3 bits)
    U8 eidTsnLengths = 0;
    eidTsnLengths |= static_cast<U8>((this->m_segmentationControl & 0x01) << 7);
    eidTsnLengths |= static_cast<U8>(((eidSize - 1) & 0x07) << 4);
    eidTsnLengths |= static_cast<U8>((this->m_segmentMetadataFlag & 0x01) << 3);
    eidTsnLengths |= static_cast<U8>((tsnSize - 1) & 0x07);

    status = serialBuffer.serializeFrom(eidTsnLengths);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Variable-width fields (size based on actual values)
    status = encodeIntegerInSize(serialBuffer, this->m_sourceEid, eidSize);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = encodeIntegerInSize(serialBuffer, this->m_transactionSeq, tsnSize);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = encodeIntegerInSize(serialBuffer, this->m_destEid, eidSize);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Pdu::Header::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
    Fw::SerializeStatus status;

    // Byte 0: flags
    U8 flags;
    status = serialBuffer.deserializeTo(flags);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    this->m_version = (flags >> 5) & 0x07;
    this->m_pduType = static_cast<PduType>((flags >> 4) & 0x01);
    this->m_direction = static_cast<Direction>((flags >> 3) & 0x01);
    this->m_txmMode = static_cast<Class>((flags >> 2) & 0x01);
    this->m_crcFlag = static_cast<CrcFlag>((flags >> 1) & 0x01);
    this->m_largeFileFlag = static_cast<LargeFileFlag>(flags & 0x01);

    // Bytes 1-2: PDU data length
    status = serialBuffer.deserializeTo(this->m_pduDataLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Byte 3: eidTsnLengths
    U8 eidTsnLengths;
    status = serialBuffer.deserializeTo(eidTsnLengths);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    this->m_segmentationControl = (eidTsnLengths >> 7) & 0x01;
    U8 eidSize = ((eidTsnLengths >> 4) & 0x07) + 1;
    this->m_segmentMetadataFlag = (eidTsnLengths >> 3) & 0x01;
    U8 tsnSize = (eidTsnLengths & 0x07) + 1;

    // Validate that the sizes are within bounds (1-8 bytes)
    FW_ASSERT(eidSize >= 1 && eidSize <= 8, static_cast<FwAssertArgType>(eidSize));
    FW_ASSERT(tsnSize >= 1 && tsnSize <= 8, static_cast<FwAssertArgType>(tsnSize));

    // Variable-width fields (size determined by encoded length)
    this->m_sourceEid = static_cast<CfdpEntityId>(decodeIntegerInSize(serialBuffer, eidSize, status));
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    this->m_transactionSeq = static_cast<CfdpTransactionSeq>(decodeIntegerInSize(serialBuffer, tsnSize, status));
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    this->m_destEid = static_cast<CfdpEntityId>(decodeIntegerInSize(serialBuffer, eidSize, status));
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Don't set m_type yet - it will be determined by the directive code for directive PDUs
    // or set to T_FILE_DATA for file data PDUs
    if (this->m_pduType == PDU_TYPE_FILE_DATA) {
        this->m_type = T_FILE_DATA;
    } else {
        // For directive PDUs, type will be set when directive code is read
        this->m_type = T_NONE;
    }

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
