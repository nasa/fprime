// ======================================================================
// \title  FinPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP FIN (Finished) PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void Pdu::FinPdu::initialize(Direction direction,
                              TransmissionMode txmMode,
                              CfdpEntityId sourceEid,
                              CfdpTransactionSeq transactionSeq,
                              CfdpEntityId destEid,
                              ConditionCode conditionCode,
                              FinDeliveryCode deliveryCode,
                              FinFileStatus fileStatus) {
    // Initialize header with T_FIN type
    this->m_header.initialize(T_FIN, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_conditionCode = conditionCode;
    this->m_deliveryCode = deliveryCode;
    this->m_fileStatus = fileStatus;
}

U32 Pdu::FinPdu::bufferSize() const {
    U32 size = this->m_header.bufferSize();

    // Directive code: 1 byte
    // Flags: 1 byte (condition code, delivery code, file status all packed)
    size += sizeof(U8) + sizeof(U8);

    return size;
}

Fw::SerializeStatus Pdu::FinPdu::toBuffer(Fw::Buffer& buffer) const {
    Fw::SerialBuffer serialBuffer(buffer.getData(), buffer.getSize());
    Fw::SerializeStatus status = this->toSerialBuffer(serialBuffer);
    if (status == Fw::FW_SERIALIZE_OK) {
        buffer.setSize(serialBuffer.getSize());
    }
    return status;
}

Fw::SerializeStatus Pdu::FinPdu::fromBuffer(const Fw::Buffer& buffer) {
    // Create SerialBuffer from Buffer
    Fw::SerialBuffer serialBuffer(const_cast<Fw::Buffer&>(buffer).getData(),
                                  const_cast<Fw::Buffer&>(buffer).getSize());
    serialBuffer.fill();

    // Deserialize header first
    Fw::SerializeStatus status = this->m_header.fromSerialBuffer(serialBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Validate this is a directive PDU (not file data)
    if (this->m_header.m_pduType != PDU_TYPE_DIRECTIVE) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Validate directive code
    U8 directiveCode;
    status = serialBuffer.deserializeTo(directiveCode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    if (directiveCode != FILE_DIRECTIVE_FIN) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Now set the type to T_FIN since we've validated it
    this->m_header.m_type = T_FIN;

    // Deserialize the FIN body
    return this->fromSerialBuffer(serialBuffer);
}

Fw::SerializeStatus Pdu::FinPdu::toSerialBuffer(Fw::SerialBuffer& serialBuffer) const {
    FW_ASSERT(this->m_header.m_type == T_FIN);

    // Calculate PDU data length (everything after header)
    U32 dataLength = this->bufferSize() - this->m_header.bufferSize();

    // Update header with data length
    Header headerCopy = this->m_header;
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

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Pdu::FinPdu::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
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

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
