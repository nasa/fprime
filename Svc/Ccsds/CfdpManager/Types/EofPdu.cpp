// ======================================================================
// \title  EofPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP EOF PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/EofPdu.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void EofPdu::initialize(Direction direction,
                              Cfdp::Class::T txmMode,
                              CfdpEntityId sourceEid,
                              CfdpTransactionSeq transactionSeq,
                              CfdpEntityId destEid,
                              ConditionCode conditionCode,
                              U32 checksum,
                              CfdpFileSize fileSize) {
    // Initialize header with T_EOF type
    this->m_header.initialize(T_EOF, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_conditionCode = conditionCode;
    this->m_checksum = checksum;
    this->m_fileSize = fileSize;

    // Clear TLV list
    this->m_tlvList.clear();
}

U32 EofPdu::getBufferSize() const {
    U32 size = this->m_header.getBufferSize();

    // Directive code: 1 byte
    // Condition code: 1 byte
    // Checksum: 4 bytes (U32)
    // File size: sizeof(CfdpFileSize) bytes
    size += sizeof(U8) + sizeof(U8) + sizeof(U32) + sizeof(CfdpFileSize);

    // Add TLV size
    size += this->m_tlvList.getEncodedSize();

    return size;
}

Fw::SerializeStatus EofPdu::serializeTo(Fw::SerialBufferBase& buffer,
                                         Fw::Endianness mode) const {
    return this->toSerialBuffer(buffer);
}

Fw::SerializeStatus EofPdu::deserializeFrom(Fw::SerialBufferBase& buffer,
                                             Fw::Endianness mode) {
    // Deserialize header first
    Fw::SerializeStatus status = this->m_header.fromSerialBuffer(buffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Validate this is a directive PDU (not file data)
    if (this->m_header.m_pduType != PDU_TYPE_DIRECTIVE) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Validate directive code
    U8 directiveCode;
    status = buffer.deserializeTo(directiveCode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    if (directiveCode != FILE_DIRECTIVE_END_OF_FILE) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Now set the type to T_EOF since we've validated it
    this->m_header.m_type = T_EOF;

    // Deserialize the EOF body
    return this->fromSerialBuffer(buffer);
}

Fw::SerializeStatus EofPdu::toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const {
    FW_ASSERT(this->m_header.m_type == T_EOF);

    // Calculate PDU data length (everything after header)
    U32 dataLength = this->getBufferSize() - this->m_header.getBufferSize();

    // Update header with data length
    PduHeader headerCopy = this->m_header;
    headerCopy.setPduDataLength(static_cast<U16>(dataLength));

    // Serialize header
    Fw::SerializeStatus status = headerCopy.toSerialBuffer(serialBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Directive code
    U8 directiveCode = static_cast<U8>(FILE_DIRECTIVE_END_OF_FILE);
    status = serialBuffer.serializeFrom(directiveCode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Condition code
    U8 conditionCode = static_cast<U8>(this->m_conditionCode);
    status = serialBuffer.serializeFrom(conditionCode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Checksum (U32)
    status = serialBuffer.serializeFrom(this->m_checksum);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // File size (CfdpFileSize)
    status = serialBuffer.serializeFrom(this->m_fileSize);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Serialize TLVs (if any)
    status = this->m_tlvList.toSerialBuffer(serialBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus EofPdu::fromSerialBuffer(Fw::SerialBufferBase& serialBuffer) {
    FW_ASSERT(this->m_header.m_type == T_EOF);

    // Directive code already read by union wrapper

    // Condition code
    U8 conditionCodeVal;
    Fw::SerializeStatus status = serialBuffer.deserializeTo(conditionCodeVal);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    this->m_conditionCode = static_cast<ConditionCode>(conditionCodeVal);

    // Checksum
    status = serialBuffer.deserializeTo(this->m_checksum);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // File size
    status = serialBuffer.deserializeTo(this->m_fileSize);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Deserialize TLVs (consumes rest of buffer)
    status = this->m_tlvList.fromSerialBuffer(serialBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
