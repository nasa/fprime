// ======================================================================
// \title  FileDataPdu.cpp
// \author campuzan
// \brief  cpp file for CFDP File Data PDU
// ======================================================================

#include <Svc/Ccsds/CfdpManager/Types/FileDataPdu.hpp>
#include <Fw/Types/Assert.hpp>
#include <config/CfdpCfg.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

void FileDataPdu::initialize(Direction direction,
                                   Cfdp::Class::T txmMode,
                                   CfdpEntityId sourceEid,
                                   CfdpTransactionSeq transactionSeq,
                                   CfdpEntityId destEid,
                                   CfdpFileSize offset,
                                   U16 dataSize,
                                   const U8* data) {
    // Initialize header with T_FILE_DATA type
    this->m_header.initialize(T_FILE_DATA, direction, txmMode, sourceEid, transactionSeq, destEid);

    this->m_offset = offset;
    this->m_dataSize = dataSize;
    this->m_data = data;
}

U32 FileDataPdu::getBufferSize() const {
    U32 size = this->m_header.getBufferSize();

    // Offset field size depends on large file flag
    if (this->m_header.m_largeFileFlag == LARGE_FILE_64_BIT) {
        size += sizeof(U64);  // 8-byte offset
    } else {
        size += sizeof(U32);  // 4-byte offset
    }

    size += this->m_dataSize;  // actual data
    return size;
}

U32 FileDataPdu::getMaxFileDataSize() {
    U32 size = this->m_header.getBufferSize();

    // Offset field size depends on large file flag
    if (this->m_header.m_largeFileFlag == LARGE_FILE_64_BIT) {
        size += sizeof(U64);  // 8-byte offset
    } else {
        size += sizeof(U32);  // 4-byte offset
    }

    return CFDP_MAX_PDU_SIZE - size;
}

Fw::SerializeStatus FileDataPdu::toBuffer(Fw::Buffer& buffer) const {
    Fw::SerialBuffer serialBuffer(buffer.getData(), buffer.getSize());
    Fw::SerializeStatus status = this->toSerialBuffer(serialBuffer);
    if (status == Fw::FW_SERIALIZE_OK) {
        buffer.setSize(serialBuffer.getSize());
    }
    return status;
}

Fw::SerializeStatus FileDataPdu::fromBuffer(const Fw::Buffer& buffer) {
    // Create SerialBuffer from Buffer
    Fw::SerialBuffer serialBuffer(const_cast<Fw::Buffer&>(buffer).getData(),
                                  const_cast<Fw::Buffer&>(buffer).getSize());
    serialBuffer.fill();

    // Deserialize header first
    Fw::SerializeStatus status = this->m_header.fromSerialBuffer(serialBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Validate this is a file data PDU
    if (this->m_header.m_pduType != PDU_TYPE_FILE_DATA) {
        return Fw::FW_DESERIALIZE_TYPE_MISMATCH;
    }

    // Set the type to T_FILE_DATA since we've validated it
    this->m_header.m_type = T_FILE_DATA;

    // Deserialize the file data body
    return this->fromSerialBuffer(serialBuffer);
}

Fw::SerializeStatus FileDataPdu::toSerialBuffer(Fw::SerialBuffer& serialBuffer) const {
    FW_ASSERT(this->m_header.m_type == T_FILE_DATA);

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

    // Serialize offset - size depends on large file flag
    if (this->m_header.m_largeFileFlag == LARGE_FILE_64_BIT) {
        // Serialize as 8 bytes (64-bit)
        U64 offset64 = this->m_offset;
        status = serialBuffer.serializeFrom(offset64);
    } else {
        // Serialize as 4 bytes (32-bit)
        U32 offset32 = static_cast<U32>(this->m_offset);
        status = serialBuffer.serializeFrom(offset32);
    }
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Serialize file data (only if there is data to serialize)
    if (this->m_dataSize > 0) {
        status = serialBuffer.pushBytes(this->m_data, this->m_dataSize);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus FileDataPdu::fromSerialBuffer(Fw::SerialBuffer& serialBuffer) {
    FW_ASSERT(this->m_header.m_type == T_FILE_DATA);

    // Deserialize offset - size depends on large file flag
    Fw::SerializeStatus status;
    U8 offsetSize;
    status = serialBuffer.deserializeTo(this->m_offset);
    offsetSize = sizeof(this->m_offset);

    // Calculate remaining data size based on header's PDU data length
    U16 pduDataLength = this->m_header.getPduDataLength();
    this->m_dataSize = static_cast<U16>(pduDataLength - offsetSize);  // minus offset size

    // Point to the data in the buffer (zero-copy)
    this->m_data = serialBuffer.getBuffAddrLeft();

    // Validate we have enough bytes
    if (serialBuffer.getDeserializeSizeLeft() < this->m_dataSize) {
        return Fw::FW_DESERIALIZE_SIZE_MISMATCH;
    }

    // Advance the buffer pointer
    U8 tempBuf[1];  // Dummy buffer for validation
    for (U16 i = 0; i < this->m_dataSize; ++i) {
        status = serialBuffer.popBytes(tempBuf, 1);
        if (status != Fw::FW_SERIALIZE_OK) {
            return status;
        }
    }

    return Fw::FW_SERIALIZE_OK;
}

Fw::SerializeStatus FileDataPdu::serializeTo(Fw::SerialBufferBase& buffer,
                                               Fw::Endianness mode) const {
    // Cast to SerialBuffer and delegate to toSerialBuffer
    Fw::SerialBuffer* serialBuffer = dynamic_cast<Fw::SerialBuffer*>(&buffer);
    if (serialBuffer == nullptr) {
        return Fw::FW_SERIALIZE_FORMAT_ERROR;
    }
    return this->toSerialBuffer(*serialBuffer);
}

Fw::SerializeStatus FileDataPdu::deserializeFrom(Fw::SerialBufferBase& buffer,
                                                   Fw::Endianness mode) {
    // Cast to SerialBuffer and delegate to fromSerialBuffer
    Fw::SerialBuffer* serialBuffer = dynamic_cast<Fw::SerialBuffer*>(&buffer);
    if (serialBuffer == nullptr) {
        return Fw::FW_DESERIALIZE_FORMAT_ERROR;
    }

    // Deserialize header first
    Fw::SerializeStatus status = this->m_header.fromSerialBuffer(*serialBuffer);
    if (status != Fw::FW_SERIALIZE_OK) {
        return status;
    }

    // Deserialize the file data body
    return this->fromSerialBuffer(*serialBuffer);
}

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc
