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
    // Condition code: 1 byte
    // Delivery code (1 bit) + File status (2 bits) + spare (5 bits) packed in 1 byte
    size += sizeof(U8) + sizeof(U8) + sizeof(U8);

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

    // Condition code
    U8 conditionCode = static_cast<U8>(this->m_conditionCode);
    status = serialBuffer.serializeFrom(conditionCode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Delivery code and file status packed into 1 byte
    // Bit 7: delivery code (0=complete, 1=incomplete)
    // Bits 6-5: file status (00=discarded, 01=discarded-filestore, 10=retained, 11=unreported)
    // Bits 4-0: spare (set to 0)
    U8 deliveryAndStatus = 0;
    deliveryAndStatus |= (static_cast<U8>(this->m_deliveryCode) & 0x01) << 7;
    deliveryAndStatus |= (static_cast<U8>(this->m_fileStatus) & 0x03) << 5;

    status = serialBuffer.serializeFrom(deliveryAndStatus);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Pdu::FinPdu::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
    FW_ASSERT(this->m_header.m_type == T_FIN);

    // Directive code already read by union wrapper

    // Condition code
    U8 conditionCodeVal;
    Fw::SerializeStatus status = serialBuffer.deserializeTo(conditionCodeVal);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    this->m_conditionCode = static_cast<ConditionCode>(conditionCodeVal);

    // Delivery code and file status (packed byte)
    U8 deliveryAndStatus;
    status = serialBuffer.deserializeTo(deliveryAndStatus);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Extract delivery code from bit 7
    U8 deliveryCodeVal = (deliveryAndStatus >> 7) & 0x01;
    this->m_deliveryCode = static_cast<FinDeliveryCode>(deliveryCodeVal);

    // Extract file status from bits 6-5
    U8 fileStatusVal = (deliveryAndStatus >> 5) & 0x03;
    this->m_fileStatus = static_cast<FinFileStatus>(fileStatusVal);

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
