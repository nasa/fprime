// ======================================================================
// \title  Pdu.cpp
// \author campuzan
// \brief  cpp file for CFDP PDU union
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/Pdu.hpp>
#include <Fw/Types/Assert.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

Fw::SerializeStatus Pdu::fromBuffer(const Fw::Buffer& buffer) {
    // Create SerialBuffer from Buffer
    Fw::SerialBuffer serialBuffer(const_cast<Fw::Buffer&>(buffer).getData(),
                                  const_cast<Fw::Buffer&>(buffer).getSize());
    serialBuffer.fill();

    // Deserialize header first to determine PDU type
    Fw::SerializeStatus status = this->m_header.fromSerialBuffer(serialBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // For directive PDUs, m_type will be T_NONE after header deserialization
    // We need to peek at the directive code to determine the specific PDU type
    if (this->m_header.m_type == T_NONE) {
        // Peek at the directive code byte
        U8 directiveCode;
        status = serialBuffer.deserializeTo(directiveCode);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }

        // Map directive code to PDU type
        switch (static_cast<FileDirective>(directiveCode)) {
            case FILE_DIRECTIVE_METADATA:
                this->m_header.m_type = T_METADATA;
                break;
            case FILE_DIRECTIVE_END_OF_FILE:
                this->m_header.m_type = T_EOF;
                break;
            case FILE_DIRECTIVE_FIN:
                this->m_header.m_type = T_FIN;
                break;
            case FILE_DIRECTIVE_ACK:
                this->m_header.m_type = T_ACK;
                break;
            case FILE_DIRECTIVE_NAK:
                this->m_header.m_type = T_NAK;
                break;
            default:
                // Unknown directive code
                return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
        }
    }

    // Based on header type, deserialize the specific PDU
    switch (this->m_header.m_type) {
        case T_METADATA:
            return this->m_metadataPdu.fromBuffer(buffer);

        case T_FILE_DATA:
            return this->m_fileDataPdu.fromBuffer(buffer);

        case T_EOF:
            return this->m_eofPdu.fromBuffer(buffer);

        case T_FIN:
            return this->m_finPdu.fromBuffer(buffer);

        case T_ACK:
            return this->m_ackPdu.fromBuffer(buffer);

        case T_NAK:
            return this->m_nakPdu.fromBuffer(buffer);

        default:
            // Unknown PDU type
            // Don't assert on unknown data from the ground
            return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }
}

Fw::SerializeStatus Pdu::toBuffer(Fw::Buffer& buffer) const {
    // Based on header type, serialize the specific PDU
    switch (this->m_header.m_type) {
        case T_METADATA:
            return this->m_metadataPdu.toBuffer(buffer);

        case T_FILE_DATA:
            return this->m_fileDataPdu.toBuffer(buffer);

        case T_EOF:
            return this->m_eofPdu.toBuffer(buffer);

        case T_FIN:
            return this->m_finPdu.toBuffer(buffer);

        case T_ACK:
            return this->m_ackPdu.toBuffer(buffer);

        case T_NAK:
            return this->m_nakPdu.toBuffer(buffer);

        default:
            // Unknown PDU type
            // This is on the send side, so we should know what we are sending
            FW_ASSERT(false, this->m_header.m_type);
            return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }
}

const Pdu::Header& Pdu::asHeader() const {
    return this->m_header;
}

FileDirective Pdu::getDirectiveCode() const {
    switch (this->m_header.m_type) {
        case T_METADATA:
            return this->m_metadataPdu.getDirectiveCode();
        case T_EOF:
            return this->m_eofPdu.getDirectiveCode();
        case T_FIN:
            return this->m_finPdu.getDirectiveCode();
        case T_ACK:
            return this->m_ackPdu.getDirectiveCode();
        case T_NAK:
            return this->m_nakPdu.getDirectiveCode();
        case T_FILE_DATA:
            // File data PDU - not a directive
            return FILE_DIRECTIVE_INVALID_MAX;
        default:
            // Unknown PDU type
            FW_ASSERT(false, this->m_header.m_type);
            return FILE_DIRECTIVE_INVALID_MAX;
    }
}

const Pdu::MetadataPdu& Pdu::asMetadataPdu() const {
    FW_ASSERT(this->m_header.m_type == T_METADATA);
    return this->m_metadataPdu;
}

const Pdu::FileDataPdu& Pdu::asFileDataPdu() const {
    FW_ASSERT(this->m_header.m_type == T_FILE_DATA);
    return this->m_fileDataPdu;
}

const Pdu::EofPdu& Pdu::asEofPdu() const {
    FW_ASSERT(this->m_header.m_type == T_EOF);
    return this->m_eofPdu;
}

const Pdu::FinPdu& Pdu::asFinPdu() const {
    FW_ASSERT(this->m_header.m_type == T_FIN);
    return this->m_finPdu;
}

const Pdu::AckPdu& Pdu::asAckPdu() const {
    FW_ASSERT(this->m_header.m_type == T_ACK);
    return this->m_ackPdu;
}

const Pdu::NakPdu& Pdu::asNakPdu() const {
    FW_ASSERT(this->m_header.m_type == T_NAK);
    return this->m_nakPdu;
}

Pdu::MetadataPdu& Pdu::asMetadataPdu() {
    this->m_header.m_type = T_METADATA;
    return this->m_metadataPdu;
}

Pdu::FileDataPdu& Pdu::asFileDataPdu() {
    this->m_header.m_type = T_FILE_DATA;
    return this->m_fileDataPdu;
}

Pdu::EofPdu& Pdu::asEofPdu() {
    this->m_header.m_type = T_EOF;
    return this->m_eofPdu;
}

Pdu::FinPdu& Pdu::asFinPdu() {
    this->m_header.m_type = T_FIN;
    return this->m_finPdu;
}

Pdu::AckPdu& Pdu::asAckPdu() {
    this->m_header.m_type = T_ACK;
    return this->m_ackPdu;
}

Pdu::NakPdu& Pdu::asNakPdu() {
    this->m_header.m_type = T_NAK;
    return this->m_nakPdu;
}

U32 Pdu::getBufferSize() const {
    // Based on header type, get size from the specific PDU
    switch (this->m_header.m_type) {
        case T_METADATA:
            return this->m_metadataPdu.getBufferSize();

        case T_FILE_DATA:
            return this->m_fileDataPdu.getBufferSize();

        case T_EOF:
            return this->m_eofPdu.getBufferSize();

        case T_FIN:
            return this->m_finPdu.getBufferSize();

        case T_ACK:
            return this->m_ackPdu.getBufferSize();

        case T_NAK:
            return this->m_nakPdu.getBufferSize();

        default:
            // Unknown PDU type, return header size only
            return this->m_header.getBufferSize();
    }
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
