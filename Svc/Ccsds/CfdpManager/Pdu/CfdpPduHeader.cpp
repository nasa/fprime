// ======================================================================
// \title  CfdpPduHeader.cpp
// \author campuzan
// \brief  cpp file for CFDP PDU Header
//
// \copyright
// Copyright 2025, California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Pdu/CfdpPduClasses.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>

namespace Svc {
namespace Ccsds {

void CfdpPdu::Header::initialize(Type type,
                                 U8 direction,
                                 U8 txmMode,
                                 CfdpEntityId sourceEid,
                                 CfdpTransactionSeq transactionSeq,
                                 CfdpEntityId destEid) {
    this->m_type = type;
    this->m_version = 1;  // CFDP version is always 1
    this->m_pduType = (type == T_FILE_DATA) ? 1 : 0;
    this->m_direction = direction;
    this->m_txmMode = txmMode;
    this->m_crcFlag = 0;  // CRC not currently supported
    this->m_largeFileFlag = 0;  // 32-bit file sizes
    this->m_segmentationControl = 0;
    this->m_segmentMetadataFlag = 0;
    this->m_pduDataLength = 0;  // To be set later
    this->m_sourceEid = sourceEid;
    this->m_transactionSeq = transactionSeq;
    this->m_destEid = destEid;
}

U32 CfdpPdu::Header::bufferSize() const {
    // Fixed portion: flags(1) + length(2) + eidTsnLengths(1) = 4 bytes
    U32 size = 4;

    // Fixed-size entity IDs and transaction sequence number based on type definitions
    U8 eidSize = sizeof(CfdpEntityId);
    U8 tsnSize = sizeof(CfdpTransactionSeq);

    size += eidSize;      // source EID
    size += tsnSize;      // transaction sequence number
    size += eidSize;      // destination EID

    return size;
}

Fw::SerializeStatus CfdpPdu::Header::toSerialBuffer(Fw::SerialBuffer& serialBuffer) const {
    Fw::SerializeStatus status;

    // Fixed-size entity IDs and transaction sequence number based on type definitions
    U8 eidSize = sizeof(CfdpEntityId);
    U8 tsnSize = sizeof(CfdpTransactionSeq);

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

    // Fixed-width fields (size determined by typedef)
    status = serialBuffer.serializeFrom(this->m_sourceEid);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = serialBuffer.serializeFrom(this->m_transactionSeq);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = serialBuffer.serializeFrom(this->m_destEid);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CfdpPdu::Header::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
    Fw::SerializeStatus status;

    // Byte 0: flags
    U8 flags;
    status = serialBuffer.deserializeTo(flags);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    this->m_version = (flags >> 5) & 0x07;
    this->m_pduType = (flags >> 4) & 0x01;
    this->m_direction = (flags >> 3) & 0x01;
    this->m_txmMode = (flags >> 2) & 0x01;
    this->m_crcFlag = (flags >> 1) & 0x01;
    this->m_largeFileFlag = flags & 0x01;

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

    // Validate that the encoded sizes match the typedef sizes
    FW_ASSERT(eidSize == sizeof(CfdpEntityId), static_cast<FwAssertArgType>(eidSize));
    FW_ASSERT(tsnSize == sizeof(CfdpTransactionSeq), static_cast<FwAssertArgType>(tsnSize));

    // Fixed-width fields (size determined by typedef)
    status = serialBuffer.deserializeTo(this->m_sourceEid);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = serialBuffer.deserializeTo(this->m_transactionSeq);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = serialBuffer.deserializeTo(this->m_destEid);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Don't set m_type yet - it will be determined by the directive code for directive PDUs
    // or set to T_FILE_DATA for file data PDUs
    if (this->m_pduType == 1) {
        this->m_type = T_FILE_DATA;
    } else {
        // For directive PDUs, type will be set when directive code is read
        this->m_type = T_NONE;
    }

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Ccsds
}  // namespace Svc
