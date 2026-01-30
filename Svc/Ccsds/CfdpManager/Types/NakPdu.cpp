// ======================================================================
// \title  NakPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP NAK (Negative Acknowledge) PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/Pdu.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void Pdu::NakPdu::initialize(Direction direction,
                              Cfdp::Class::T txmMode,
                              CfdpEntityId sourceEid,
                              CfdpTransactionSeq transactionSeq,
                              CfdpEntityId destEid,
                              CfdpFileSize scopeStart,
                              CfdpFileSize scopeEnd) {
    // Initialize header with T_NAK type
    this->m_header.initialize(T_NAK, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_scopeStart = scopeStart;
    this->m_scopeEnd = scopeEnd;
    this->m_numSegments = 0;
}

bool Pdu::NakPdu::addSegment(CfdpFileSize offsetStart, CfdpFileSize offsetEnd) {
    if (this->m_numSegments >= CF_NAK_MAX_SEGMENTS) {
        return false;
    }
    this->m_segments[this->m_numSegments].offsetStart = offsetStart;
    this->m_segments[this->m_numSegments].offsetEnd = offsetEnd;
    this->m_numSegments++;
    return true;
}

void Pdu::NakPdu::clearSegments() {
    this->m_numSegments = 0;
}

U32 Pdu::NakPdu::getBufferSize() const {
    U32 size = this->m_header.getBufferSize();

    // Directive code: 1 byte (FILE_DIRECTIVE_NAK)
    // Scope start: sizeof(CfdpFileSize) bytes
    // Scope end: sizeof(CfdpFileSize) bytes
    // Segment requests: m_numSegments * (2 * sizeof(CfdpFileSize)) bytes
    size += static_cast<U32>(sizeof(U8) + sizeof(CfdpFileSize) + sizeof(CfdpFileSize));
    size += static_cast<U32>(this->m_numSegments * (sizeof(CfdpFileSize) + sizeof(CfdpFileSize)));

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
    U32 dataLength = this->getBufferSize() - this->m_header.getBufferSize();

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

    // Scope start (file offset)
    status = serialBuffer.serializeFrom(this->m_scopeStart);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Scope end (file offset)
    status = serialBuffer.serializeFrom(this->m_scopeEnd);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Serialize segment requests
    for (U8 i = 0; i < this->m_numSegments; i++) {
        // Segment start offset
        status = serialBuffer.serializeFrom(this->m_segments[i].offsetStart);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }

        // Segment end offset
        status = serialBuffer.serializeFrom(this->m_segments[i].offsetEnd);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Pdu::NakPdu::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
    FW_ASSERT(this->m_header.m_type == T_NAK);

    // Directive code already read by fromBuffer()

    // Scope start (file offset)
    Fw::SerializeStatus status = serialBuffer.deserializeTo(this->m_scopeStart);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Scope end (file offset)
    status = serialBuffer.deserializeTo(this->m_scopeEnd);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Calculate number of segment requests from remaining buffer size
    // Each segment is 2 * sizeof(CfdpFileSize) bytes
    Fw::Serializable::SizeType remainingBytes = serialBuffer.getDeserializeSizeLeft();
    U32 segmentSize = sizeof(CfdpFileSize) + sizeof(CfdpFileSize);
    U32 numSegsCalculated = static_cast<U32>(remainingBytes / segmentSize);
    this->m_numSegments = static_cast<U8>(numSegsCalculated);

    // Limit to max segments
    if (this->m_numSegments > CF_NAK_MAX_SEGMENTS) {
        this->m_numSegments = CF_NAK_MAX_SEGMENTS;
    }

    // Deserialize segment requests
    for (U8 i = 0; i < this->m_numSegments; i++) {
        // Segment start offset
        status = serialBuffer.deserializeTo(this->m_segments[i].offsetStart);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }

        // Segment end offset
        status = serialBuffer.deserializeTo(this->m_segments[i].offsetEnd);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
    }

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
