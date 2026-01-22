// ======================================================================
// \title  NakPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP NAK (Negative Acknowledge) PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void Pdu::NakPdu::initialize(Direction direction,
                              TransmissionMode txmMode,
                              CfdpEntityId sourceEid,
                              CfdpTransactionSeq transactionSeq,
                              CfdpEntityId destEid,
                              U32 scopeStart,
                              U32 scopeEnd) {
    // Initialize header with T_NAK type
    this->m_header.initialize(T_NAK, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_scopeStart = scopeStart;
    this->m_scopeEnd = scopeEnd;
}

U32 Pdu::NakPdu::bufferSize() const {
    U32 size = this->m_header.bufferSize();

    // Directive code: 1 byte (FILE_DIRECTIVE_NAK)
    // Scope start: 4 bytes
    // Scope end: 4 bytes
    // Note: This implementation uses simplified NAK with only scope, no segment requests
    size += sizeof(U8) + sizeof(U32) + sizeof(U32);

    return size;
}

Fw::SerializeStatus Pdu::NakPdu::toBuffer(Fw::Buffer& buffer) const {
    Fw::SerialBuffer serialBuffer(buffer.getData(), buffer.getSize());
    Fw::SerializeStatus status = this->toSerialBuffer(serialBuffer);
    if (status == Fw::FW_SERIALIZE_OK) {
        buffer.setSize(serialBuffer.getSize());
    }
    return status;
}

Fw::SerializeStatus Pdu::NakPdu::fromBuffer(const Fw::Buffer& buffer) {
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
    if (directiveCode != FILE_DIRECTIVE_NAK) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Now set the type to T_NAK since we've validated it
    this->m_header.m_type = T_NAK;

    // Deserialize the NAK body
    return this->fromSerialBuffer(serialBuffer);
}

Fw::SerializeStatus Pdu::NakPdu::toSerialBuffer(Fw::SerialBuffer& serialBuffer) const {
    FW_ASSERT(this->m_header.m_type == T_NAK);

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

    // Directive code (NAK = 8)
    U8 directiveCodeByte = static_cast<U8>(FILE_DIRECTIVE_NAK);
    status = serialBuffer.serializeFrom(directiveCodeByte);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Scope start (32-bit offset)
    status = serialBuffer.serializeFrom(this->m_scopeStart);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Scope end (32-bit offset)
    status = serialBuffer.serializeFrom(this->m_scopeEnd);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Note: This implementation uses simplified NAK with only scope
    // No segment request pairs are included
    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Pdu::NakPdu::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
    FW_ASSERT(this->m_header.m_type == T_NAK);

    // Directive code already read by fromBuffer()

    // Scope start (32-bit offset)
    Fw::SerializeStatus status = serialBuffer.deserializeTo(this->m_scopeStart);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Scope end (32-bit offset)
    status = serialBuffer.deserializeTo(this->m_scopeEnd);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Note: This implementation uses simplified NAK with only scope
    // No segment request pairs are parsed
    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
