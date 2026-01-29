// ======================================================================
// \title  AckPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP ACK (Acknowledge) PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/Pdu.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void Pdu::AckPdu::initialize(Direction direction,
                              Cfdp::Class::T txmMode,
                              CfdpEntityId sourceEid,
                              CfdpTransactionSeq transactionSeq,
                              CfdpEntityId destEid,
                              FileDirective directiveCode,
                              U8 directiveSubtypeCode,
                              ConditionCode conditionCode,
                              AckTxnStatus transactionStatus) {
    // Initialize header with T_ACK type
    this->m_header.initialize(T_ACK, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_directiveCode = directiveCode;
    this->m_directiveSubtypeCode = directiveSubtypeCode;
    this->m_conditionCode = conditionCode;
    this->m_transactionStatus = transactionStatus;
}

U32 Pdu::AckPdu::bufferSize() const {
    U32 size = this->m_header.bufferSize();

    // Directive code: 1 byte (FILE_DIRECTIVE_ACK)
    // Directive and subtype code (bit-packed): 1 byte
    // Condition code and transaction status (bit-packed): 1 byte
    size += sizeof(U8) + sizeof(U8) + sizeof(U8);

    return size;
}

Fw::SerializeStatus Pdu::AckPdu::toBuffer(Fw::Buffer& buffer) const {
    Fw::SerialBuffer serialBuffer(buffer.getData(), buffer.getSize());
    Fw::SerializeStatus status = this->toSerialBuffer(serialBuffer);
    if (status == Fw::FW_SERIALIZE_OK) {
        buffer.setSize(serialBuffer.getSize());
    }
    return status;
}

Fw::SerializeStatus Pdu::AckPdu::fromBuffer(const Fw::Buffer& buffer) {
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
    if (directiveCode != FILE_DIRECTIVE_ACK) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Now set the type to T_ACK since we've validated it
    this->m_header.m_type = T_ACK;

    // Deserialize the ACK body
    return this->fromSerialBuffer(serialBuffer);
}

Fw::SerializeStatus Pdu::AckPdu::toSerialBuffer(Fw::SerialBuffer& serialBuffer) const {
    FW_ASSERT(this->m_header.m_type == T_ACK);

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

    // Directive code (ACK = 6)
    U8 directiveCodeByte = static_cast<U8>(FILE_DIRECTIVE_ACK);
    status = serialBuffer.serializeFrom(directiveCodeByte);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Directive and subtype code (bit-packed into 1 byte)
    // Bits 7-4: Directive code being acknowledged (4 bits)
    // Bits 3-0: Directive subtype code (4 bits)
    U8 directiveAndSubtype = 0;
    directiveAndSubtype |= (static_cast<U8>(this->m_directiveCode) & 0x0F) << 4;  // Bits 7-4
    directiveAndSubtype |= (this->m_directiveSubtypeCode & 0x0F);                  // Bits 3-0

    status = serialBuffer.serializeFrom(directiveAndSubtype);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Condition code and transaction status (bit-packed into 1 byte)
    // Bits 7-4: Condition code (4 bits)
    // Bits 3-2: Spare (0)
    // Bits 1-0: Transaction status (2 bits)
    U8 ccAndStatus = 0;
    ccAndStatus |= (static_cast<U8>(this->m_conditionCode) & 0x0F) << 4;      // Bits 7-4
    ccAndStatus |= (static_cast<U8>(this->m_transactionStatus) & 0x03);       // Bits 1-0

    status = serialBuffer.serializeFrom(ccAndStatus);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Pdu::AckPdu::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
    FW_ASSERT(this->m_header.m_type == T_ACK);

    // Directive code already read by fromBuffer()

    // Directive and subtype code (packed byte)
    U8 directiveAndSubtype;
    Fw::SerializeStatus status = serialBuffer.deserializeTo(directiveAndSubtype);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Extract fields from directive and subtype byte:
    // Bits 7-4: Directive code being acknowledged
    // Bits 3-0: Directive subtype code
    U8 directiveCodeVal = (directiveAndSubtype >> 4) & 0x0F;
    U8 subtypeCodeVal = directiveAndSubtype & 0x0F;

    this->m_directiveCode = static_cast<FileDirective>(directiveCodeVal);
    this->m_directiveSubtypeCode = subtypeCodeVal;

    // Condition code and transaction status (packed byte)
    U8 ccAndStatus;
    status = serialBuffer.deserializeTo(ccAndStatus);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Extract fields from condition code and transaction status byte:
    // Bits 7-4: Condition code
    // Bits 3-2: Spare
    // Bits 1-0: Transaction status
    U8 conditionCodeVal = (ccAndStatus >> 4) & 0x0F;
    U8 transactionStatusVal = ccAndStatus & 0x03;

    this->m_conditionCode = static_cast<ConditionCode>(conditionCodeVal);
    this->m_transactionStatus = static_cast<AckTxnStatus>(transactionStatusVal);

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
