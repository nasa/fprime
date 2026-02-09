// ======================================================================
// \title  FinPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP FIN (Finished) PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/FinPdu.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void FinPdu::initialize(PduDirection direction,
                              Cfdp::Class::T txmMode,
                              EntityId sourceEid,
                              TransactionSeq transactionSeq,
                              EntityId destEid,
                              ConditionCode conditionCode,
                              FinDeliveryCode deliveryCode,
                              FinFileStatus fileStatus) {
    // Initialize header with T_FIN type
    this->m_header.initialize(T_FIN, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_conditionCode = conditionCode;
    this->m_deliveryCode = deliveryCode;
    this->m_fileStatus = fileStatus;

    // Clear TLV list
    this->m_tlvList.clear();
}

U32 FinPdu::getBufferSize() const {
    U32 size = this->m_header.getBufferSize();

    // Directive code: 1 byte
    // Flags: 1 byte (condition code, delivery code, file status all packed)
    size += sizeof(U8) + sizeof(U8);

    // Add TLV size
    size += this->m_tlvList.getEncodedSize();

    return size;
}

Fw::SerializeStatus FinPdu::serializeTo(Fw::SerialBufferBase& buffer,
                                         Fw::Endianness mode) const {
    return this->toSerialBuffer(buffer);
}

Fw::SerializeStatus FinPdu::deserializeFrom(Fw::SerialBufferBase& buffer,
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
    if (directiveCode != FILE_DIRECTIVE_FIN) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Now set the type to T_FIN since we've validated it
    this->m_header.m_type = T_FIN;

    // Deserialize the FIN body
    return this->fromSerialBuffer(buffer);
}

Fw::SerializeStatus FinPdu::toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const {
    FW_ASSERT(this->m_header.m_type == T_FIN);

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

    // Directive code (FIN = 5)
    U8 directiveCode = static_cast<U8>(FILE_DIRECTIVE_FIN);
    status = serialBuffer.serializeFrom(directiveCode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Flags byte: condition code, delivery code, and file status packed together
    // Bits 7-4: Condition code (4 bits)
    // Bit 3: Spare (0)
    // Bit 2: Delivery code (1 bit)
    // Bits 1-0: File status (2 bits)
    U8 flags = 0;
    flags |= (static_cast<U8>(this->m_conditionCode) & 0x0F) << 4;  // Bits 7-4
    flags |= (static_cast<U8>(this->m_deliveryCode) & 0x01) << 2;   // Bit 2
    flags |= (static_cast<U8>(this->m_fileStatus) & 0x03);          // Bits 1-0

    status = serialBuffer.serializeFrom(flags);
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

Fw::SerializeStatus FinPdu::fromSerialBuffer(Fw::SerialBufferBase& serialBuffer) {
    FW_ASSERT(this->m_header.m_type == T_FIN);

    // Directive code already read by fromBuffer()

    // Flags byte contains: condition code, delivery code, and file status
    U8 flags;
    Fw::SerializeStatus status = serialBuffer.deserializeTo(flags);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Extract fields from flags byte:
    // Bits 7-4: Condition code (4 bits)
    // Bit 3: Spare
    // Bit 2: Delivery code (1 bit)
    // Bits 1-0: File status (2 bits)
    U8 conditionCodeVal = (flags >> 4) & 0x0F;
    U8 deliveryCodeVal = (flags >> 2) & 0x01;
    U8 fileStatusVal = flags & 0x03;

    this->m_conditionCode = static_cast<ConditionCode>(conditionCodeVal);
    this->m_deliveryCode = static_cast<FinDeliveryCode>(deliveryCodeVal);
    this->m_fileStatus = static_cast<FinFileStatus>(fileStatusVal);

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
