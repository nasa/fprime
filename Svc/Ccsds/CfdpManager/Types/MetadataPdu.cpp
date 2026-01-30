// ======================================================================
// \title  MetadataPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP Metadata PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/Pdu.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/StringUtils.hpp>
#include <config/CfdpCfg.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void Pdu::MetadataPdu::initialize(Direction direction,
                                   Cfdp::Class::T txmMode,
                                   CfdpEntityId sourceEid,
                                   CfdpTransactionSeq transactionSeq,
                                   CfdpEntityId destEid,
                                   CfdpFileSize fileSize,
                                   const Fw::String& sourceFilename,
                                   const Fw::String& destFilename,
                                   ChecksumType checksumType,
                                   U8 closureRequested) {
    this->m_header.initialize(T_METADATA, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_fileSize = fileSize;

    // Enforce CF_FILENAME_MAX_LEN for source filename
    FwSizeType srcLen = sourceFilename.length();
    FW_ASSERT(srcLen <= CF_FILENAME_MAX_LEN, static_cast<FwAssertArgType>(srcLen), CF_FILENAME_MAX_LEN);
    this->m_sourceFilename = sourceFilename;

    // Enforce CF_FILENAME_MAX_LEN for destination filename
    FwSizeType dstLen = destFilename.length();
    FW_ASSERT(dstLen <= CF_FILENAME_MAX_LEN, static_cast<FwAssertArgType>(dstLen), CF_FILENAME_MAX_LEN);
    this->m_destFilename = destFilename;

    this->m_checksumType = checksumType;
    this->m_closureRequested = closureRequested;
}

U32 Pdu::MetadataPdu::getBufferSize() const {
    U32 size = this->m_header.getBufferSize();

    // Directive code: 1 byte
    // Segmentation control byte (includes closure requested and checksum type): 1 byte
    // File size: variable
    size += sizeof(U8) + sizeof(U8) + sizeof(CfdpFileSize);

    // Source filename LV: length(1) + value(n)
    size += 1 + static_cast<U32>(this->m_sourceFilename.length());

    // Dest filename LV: length(1) + value(n)
    size += 1 + static_cast<U32>(this->m_destFilename.length());

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
    U32 dataLength = this->getBufferSize() - this->m_header.getBufferSize();

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

    // File size (CfdpFileSize)
    status = serialBuffer.serializeFrom(this->m_fileSize);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Source filename LV
    U8 sourceFilenameLength = static_cast<U8>(this->m_sourceFilename.length());
    status = serialBuffer.serializeFrom(sourceFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = serialBuffer.pushBytes(
        reinterpret_cast<const U8*>(this->m_sourceFilename.toChar()),
        sourceFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Destination filename LV
    U8 destFilenameLength = static_cast<U8>(this->m_destFilename.length());
    status = serialBuffer.serializeFrom(destFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    status = serialBuffer.pushBytes(
        reinterpret_cast<const U8*>(this->m_destFilename.toChar()),
        destFilenameLength);
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
    U8 sourceFilenameLength;
    status = serialBuffer.deserializeTo(sourceFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Validate filename length against CF_FILENAME_MAX_LEN
    if (sourceFilenameLength > CF_FILENAME_MAX_LEN) {
        // CFE_EVS_SendEvent(CF_PDU_INVALID_SRC_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: metadata PDU rejected due to invalid source filename length of 0x%02x", sourceFilenameLength);
        // ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }

    // Validate filename is not empty
    if (sourceFilenameLength == 0) {
        // CFE_EVS_SendEvent(CF_PDU_INVALID_SRC_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: metadata PDU rejected due to empty source filename");
        // ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }

    // Read filename into temporary buffer
    char sourceFilenameBuffer[CF_FILENAME_MAX_LEN + 1];
    status = serialBuffer.popBytes(reinterpret_cast<U8*>(sourceFilenameBuffer), sourceFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    // Null-terminate before assigning to Fw::String
    sourceFilenameBuffer[sourceFilenameLength] = '\0';
    this->m_sourceFilename = sourceFilenameBuffer;

    // Destination filename LV
    U8 destFilenameLength;
    status = serialBuffer.deserializeTo(destFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Validate filename length against CF_FILENAME_MAX_LEN
    if (destFilenameLength > CF_FILENAME_MAX_LEN) {
        // CFE_EVS_SendEvent(CF_PDU_INVALID_DST_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: metadata PDU rejected due to invalid dest filename length of 0x%02x", destFilenameLength);
        // ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }

    // Validate filename is not empty
    if (destFilenameLength == 0) {
        // CFE_EVS_SendEvent(CF_PDU_INVALID_DST_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
        //                   "CF: metadata PDU rejected due to empty dest filename");
        // ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.recv.error;
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }

    // Read filename into temporary buffer
    char destFilenameBuffer[CF_FILENAME_MAX_LEN + 1];
    status = serialBuffer.popBytes(reinterpret_cast<U8*>(destFilenameBuffer), destFilenameLength);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }
    // Null-terminate before assigning to Fw::String
    destFilenameBuffer[destFilenameLength] = '\0';
    this->m_destFilename = destFilenameBuffer;

    return Fw::FW_SERIALIZE_OK;
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
