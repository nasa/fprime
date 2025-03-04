#include "Svc/FpySequencer/FpySequencer.hpp"
extern "C" {
#include "Utils/Hash/libcrc/lib_crc.h"
}
namespace Svc {

void FpySequencer::allocateBuffer(NATIVE_INT_TYPE identifier, Fw::MemAllocator& allocator, NATIVE_UINT_TYPE bytes) {
    // if this assertion fails, you aren't allocating enough bytes for the
    // FpySequencer. this is because you must have a buffer big enough to fit the
    // header of a sequence
    FW_ASSERT(bytes >= Fpy::Header::SERIALIZED_SIZE);
    bool recoverable = false;
    this->m_allocatorId = identifier;
    U8* allocatedMemory =
        static_cast<U8*>(allocator.allocate(static_cast<NATIVE_UINT_TYPE>(identifier), bytes, recoverable));
    // if this fails, unable to allocate the memory
    FW_ASSERT(bytes > 0);
    this->m_sequenceBuffer.setExtBuffer(allocatedMemory, bytes);
}

void FpySequencer::deallocateBuffer(Fw::MemAllocator& allocator) {
    allocator.deallocate(static_cast<NATIVE_UINT_TYPE>(this->m_allocatorId), this->m_sequenceBuffer.getBuffAddr());
    this->m_sequenceBuffer.clear();
}

void FpySequencer::updateComputedCRC(const U8* buffer, FwSizeType bufferSize) {
    FW_ASSERT(buffer);
    for (NATIVE_UINT_TYPE index = 0; index < bufferSize; index++) {
        this->m_computedCRC = static_cast<U32>(update_crc_32(this->m_computedCRC, static_cast<char>(buffer[index])));
    }
}

// loads the sequence in memory, and does header/crc/integrity checks.
// return true if sequence is valid
bool FpySequencer::validate() {
    FW_ASSERT(m_sequenceFilePath.length() > 0);

    // crc needs to be initialized with a particular value
    // for the calculation to work
    this->m_computedCRC = CRC_INITIAL_VALUE;

    Os::File sequenceFile;
    Os::File::Status openStatus = sequenceFile.open(m_sequenceFilePath.toChar(), Os::File::OPEN_READ);

    if (openStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileOpenError(m_sequenceFilePath, static_cast<I32>(openStatus));
        return false;
    }

    bool readStatus = readHeader(sequenceFile);
    if (!readStatus) {
        return false;
    }

    // check matching schema version
    if (m_sequenceObj.getheader().getschemaVersion() != Fpy::SCHEMA_VERSION) {
        this->log_WARNING_LO_WrongSchemaVersion(Fpy::SCHEMA_VERSION, m_sequenceObj.header.schemaVersion);
        return false;
    }

    readStatus = readBody(sequenceFile);
    if (!readStatus) {
        return false;
    }

    readStatus = readFooter(sequenceFile);
    if (!readStatus) {
        return false;
    }

    // as we read the 
    this->m_computedCRC = ~this->m_computedCRC;

    return true;
}

bool FpySequencer::readBytes(Os::File& file, FwSizeType readLen) {
    FW_ASSERT(file.isOpen());
    FwSignedSizeType readLen = Fpy::Header::SERIALIZED_SIZE;

    const NATIVE_UINT_TYPE capacity = m_sequenceBuffer.getBuffCapacity();
    FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(readLen), static_cast<FwAssertArgType>(capacity),
              static_cast<FwAssertArgType>(readLen));
    Os::File::Status fileStatus = file.read(m_sequenceBuffer.getBuffAddr(), readLen);

    if (fileStatus != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(m_sequenceFilePath, static_cast<I32>(fileStatus));
        return false;
    }

    if (readLen != Fpy::Header::SERIALIZED_SIZE) {
        this->log_WARNING_HI_EndOfFileError(m_sequenceFilePath);
        return false;
    }

    Fw::SerializeStatus serializeStatus = m_sequenceBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

    updateComputedCRC(m_sequenceBuffer.getBuffAddr(), readLen);

}
//! loads from disk and formats the header into memory representation
bool FpySequencer::readHeader(Os::File& file) {
    // Major version
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.header.majorVersion);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    // Minor version
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.header.minorVersion);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    // Patch version
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.header.patchVersion);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    // Schema version
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.header.schemaVersion);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    // Argument count
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.header.argumentCount);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    // Statement count
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.header.statementCount);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    // File size
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.header.bodySize);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    this->log_DIAGNOSTIC_ReadHeaderSuccess(m_sequenceFilePath);
    return true;
}

//! loads from disk and formats the body into memory representation
bool FpySequencer::readBody(Os::File& file) {
    FW_ASSERT(file.isOpen());
    FwSignedSizeType readLen = m_sequenceObj.header.bodySize;

    const NATIVE_UINT_TYPE capacity = m_sequenceBuffer.getBuffCapacity();
    FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(readLen), static_cast<FwAssertArgType>(capacity),
              static_cast<FwAssertArgType>(readLen));
    Os::File::Status fileStatus = file.read(m_sequenceBuffer.getBuffAddr(), readLen);

    if (fileStatus != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(m_sequenceFilePath, static_cast<I32>(fileStatus));
        return false;
    }

    if (readLen != Fpy::Header::SERIALIZED_SIZE) {
        this->log_WARNING_HI_EndOfFileError(m_sequenceFilePath);
        return false;
    }

    Fw::SerializeStatus serializeStatus = m_sequenceBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

    U8 remainingArgMappings = m_sequenceObj.header.argumentCount;
    while (remainingArgMappings > 0) {
        // local variable index of arg $remainingArgMappings - 1
        serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.argArray[remainingArgMappings - 1]);
        if (serializeStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                          m_sequenceBuffer.getBuffLeft(),
                                                          m_sequenceBuffer.getBuffLength());
            return false;
        }
        remainingArgMappings--;
    }
    U16 remainingStatements = m_sequenceObj.header.statementCount;
    while (remainingStatements > 0) {
        // statement at index $remainingStatements - 1
        Fpy::Statement& statement = m_sequenceObj.statementArray[remainingStatements - 1];
        // opcode
        serializeStatus = m_sequenceBuffer.deserialize(statement.opcode);
        if (serializeStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                          m_sequenceBuffer.getBuffLeft(),
                                                          m_sequenceBuffer.getBuffLength());
            return false;
        }

        // arg buf
        serializeStatus = m_sequenceBuffer.deserialize(statement.args);
        if (serializeStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                          m_sequenceBuffer.getBuffLeft(),
                                                          m_sequenceBuffer.getBuffLength());
            return false;
        }
        remainingStatements--;
    }

    this->log_DIAGNOSTIC_ReadBodySuccess(m_sequenceFilePath);
    return true;
}

//! loads from disk and formats the footer into memory representation
bool FpySequencer::readFooter(Os::File& file) {
    FW_ASSERT(file.isOpen());
    FwSignedSizeType readLen = Fpy::Footer::SERIALIZED_SIZE;

    const NATIVE_UINT_TYPE capacity = m_sequenceBuffer.getBuffCapacity();
    FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(readLen), static_cast<FwAssertArgType>(capacity),
              static_cast<FwAssertArgType>(readLen));
    Os::File::Status fileStatus = file.read(m_sequenceBuffer.getBuffAddr(), readLen);

    if (fileStatus != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(m_sequenceFilePath, static_cast<I32>(fileStatus));
        return false;
    }

    if (readLen != Fpy::Header::SERIALIZED_SIZE) {
        this->log_WARNING_HI_EndOfFileError(m_sequenceFilePath);
        return false;
    }

    Fw::SerializeStatus serializeStatus = m_sequenceBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

    // crc
    serializeStatus = m_sequenceBuffer.deserialize(m_sequenceObj.footer.crc);
    if (serializeStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(serializeStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    this->log_DIAGNOSTIC_ReadFooterSuccess(m_sequenceFilePath);
    return true;
}

}  // namespace Svc