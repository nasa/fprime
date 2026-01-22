// ======================================================================
// \title  MetadataPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP Metadata PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Pdu/Pdu.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <config/CfdpCfg.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void Pdu::MetadataPdu::initialize(Direction direction,
                                   TransmissionMode txmMode,
                                   CfdpEntityId sourceEid,
                                   CfdpTransactionSeq transactionSeq,
                                   CfdpEntityId destEid,
                                   CfdpFileSize fileSize,
                                   const char* sourceFilename,
                                   const char* destFilename,
                                   ChecksumType checksumType,
                                   U8 closureRequested) {
    this->m_header.initialize(T_METADATA, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_fileSize = fileSize;
    this->m_sourceFilename = sourceFilename;

    // Enforce CF_FILENAME_MAX_LEN for source filename
    FwSizeType srcLen = Fw::StringUtils::string_length(sourceFilename, CF_FILENAME_MAX_LEN);
    FW_ASSERT(srcLen <= CF_FILENAME_MAX_LEN, static_cast<FwAssertArgType>(srcLen));
    this->m_sourceFilenameLength = static_cast<U8>(srcLen);

    this->m_destFilename = destFilename;

    // Enforce CF_FILENAME_MAX_LEN for destination filename
    FwSizeType dstLen = Fw::StringUtils::string_length(destFilename, CF_FILENAME_MAX_LEN);
    FW_ASSERT(dstLen <= CF_FILENAME_MAX_LEN, static_cast<FwAssertArgType>(dstLen));
    this->m_destFilenameLength = static_cast<U8>(dstLen);

    this->m_checksumType = checksumType;
    this->m_closureRequested = closureRequested;
}

U32 Pdu::MetadataPdu::bufferSize() const {
    U32 size = this->m_header.bufferSize();

    // Directive code: 1 byte
    // Segmentation control byte (includes closure requested and checksum type): 1 byte
    // File size: variable
    size += sizeof(U8) + sizeof(U8) + sizeof(CfdpFileSize);

    // Source filename LV: length(1) + value(n)
    size += 1 + this->m_sourceFilenameLength;

    // Dest filename LV: length(1) + value(n)
    size += 1 + this->m_destFilenameLength;

    return size;
}

Fw::SerializeStatus Pdu::MetadataPdu::toBuffer(Fw::Buffer& buffer) const {
    Fw::SerialBuffer serialBuffer(buffer.getData(), buffer.getSize());
    Fw::SerializeStatus status = this->toSerialBuffer(serialBuffer);
    if (status == Fw::FW_SERIALIZE_OK) {
        buffer.setSize(serialBuffer.getSize());
    }
    return status;
}

Fw::SerializeStatus Pdu::MetadataPdu::fromBuffer(const Fw::Buffer& buffer) {
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
    if (directiveCode != FILE_DIRECTIVE_METADATA) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Now set the type to T_METADATA since we've validated it
    this->m_header.m_type = T_METADATA;

    // Deserialize the metadata body
    return this->fromSerialBuffer(serialBuffer);
}

Fw::SerializeStatus Pdu::MetadataPdu::toSerialBuffer(Fw::SerialBuffer& serialBuffer) const {
    FW_ASSERT(this->m_header.m_type == T_METADATA);

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

    // Directive code (METADATA = 7)
    U8 directiveCode = static_cast<U8>(FILE_DIRECTIVE_METADATA);
    status = serialBuffer.serializeFrom(directiveCode);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Segmentation control byte
    // bit 7: closure_requested
    // bits 6-4: reserved (000b)
    // bits 3-0: checksum_type
    U8 segmentationControl = 0;
    segmentationControl |= (this->m_closureRequested & 0x01) << 7;
    segmentationControl |= (static_cast<U8>(this->m_checksumType) & 0x0F);

    status = serialBuffer.serializeFrom(segmentationControl);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // File size (32-bit, big-endian)
    status = serialBuffer.serializeFrom(this->m_fileSize);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Source filename LV
    status = serialBuffer.serializeFrom(this->m_sourceFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = serialBuffer.pushBytes(
        reinterpret_cast<const U8*>(this->m_sourceFilename),
        this->m_sourceFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Destination filename LV
    status = serialBuffer.serializeFrom(this->m_destFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = serialBuffer.pushBytes(
        reinterpret_cast<const U8*>(this->m_destFilename),
        this->m_destFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Pdu::MetadataPdu::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
    FW_ASSERT(this->m_header.m_type == T_METADATA);

    // Directive code already read by union wrapper

    // Segmentation control byte
    U8 segmentationControl;
    Fw::SerializeStatus status = serialBuffer.deserializeTo(segmentationControl);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    this->m_closureRequested = (segmentationControl >> 7) & 0x01;
    U8 checksumTypeVal = segmentationControl & 0x0F;
    this->m_checksumType = static_cast<ChecksumType>(checksumTypeVal);

    // File size
    status = serialBuffer.deserializeTo(this->m_fileSize);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Source filename LV
    status = serialBuffer.deserializeTo(this->m_sourceFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Validate filename length against CF_FILENAME_MAX_LEN
    FW_ASSERT(this->m_sourceFilenameLength <= CF_FILENAME_MAX_LEN,
              static_cast<FwAssertArgType>(this->m_sourceFilenameLength));

    // Point to the filename data in the buffer (zero-copy)
    const U8* addrLeft = serialBuffer.getBuffAddrLeft();
    U8 bytes[CF_FILENAME_MAX_LEN];  // Temporary buffer for validation
    status = serialBuffer.popBytes(bytes, this->m_sourceFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    this->m_sourceFilename = reinterpret_cast<const char*>(addrLeft);

    // Destination filename LV
    status = serialBuffer.deserializeTo(this->m_destFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Validate filename length against CF_FILENAME_MAX_LEN
    FW_ASSERT(this->m_destFilenameLength <= CF_FILENAME_MAX_LEN,
              static_cast<FwAssertArgType>(this->m_destFilenameLength));

    addrLeft = serialBuffer.getBuffAddrLeft();
    status = serialBuffer.popBytes(bytes, this->m_destFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    this->m_destFilename = reinterpret_cast<const char*>(addrLeft);

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
