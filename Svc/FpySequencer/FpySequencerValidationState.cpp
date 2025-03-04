#include "Svc/FpySequencer/FppConstantsAc.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
#include "Svc/FpySequencer/StatementTypeEnumAc.hpp"
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

    // read header bytes
    bool readStatus = readBytes(sequenceFile, Fpy::Header::SERIALIZED_SIZE);
    if (!readStatus) {
        return false;
    }

    // deser header
    Fw::SerializeStatus deserStatus = m_sequenceBuffer.deserialize(m_sequenceObj.getheader());
    if (deserStatus != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }

    // check matching schema version
    if (m_sequenceObj.getheader().getschemaVersion() != Fpy::SCHEMA_VERSION) {
        this->log_WARNING_LO_WrongSchemaVersion(Fpy::SCHEMA_VERSION, m_sequenceObj.getheader().getschemaVersion());
        return false;
    }

    // read body bytes
    readStatus = readBytes(sequenceFile, m_sequenceObj.getheader().getbodySize());
    if (!readStatus) {
        return false;
    }

    // deser body:
    // deser arg mappings
    U8 argMappingIdx = 0;
    while (argMappingIdx < m_sequenceObj.getheader().getargumentCount()) {
        // local variable index of arg $argMappingIdx
        deserStatus = m_sequenceBuffer.deserialize(m_sequenceObj.getargs()[argMappingIdx]);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                          m_sequenceBuffer.getBuffLeft(),
                                                          m_sequenceBuffer.getBuffLength());
            return false;
        }
        argMappingIdx++;
    }

    // deser statements
    U16 statementIdx = 0;
    while (statementIdx < m_sequenceObj.getheader().getstatementCount()) {
        Fpy::Statement& statement = m_sequenceObj.getstatements()[statementIdx];

        // statement type (directive or cmd)
        Fpy::StatementType type;
        deserStatus = m_sequenceBuffer.deserialize(type);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                          m_sequenceBuffer.getBuffLeft(),
                                                          m_sequenceBuffer.getBuffLength());
            return false;
        }
        statement.settype(type);

        // statement opcode
        FwOpcodeType opCode;
        deserStatus = m_sequenceBuffer.deserialize(opCode);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                          m_sequenceBuffer.getBuffLeft(),
                                                          m_sequenceBuffer.getBuffLength());
            return false;
        }
        statement.setopCode(opCode);

        // arg buf size
        FwSizeStoreType argBufSize;
        deserStatus = m_sequenceBuffer.deserialize(argBufSize);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                          m_sequenceBuffer.getBuffLeft(),
                                                          m_sequenceBuffer.getBuffLength());
            return false;
        }
        statement.setargBufSize(argBufSize);

        // need this for now... can remove when deserialize signature is updated
        NATIVE_UINT_TYPE argBufSizeUint = static_cast<NATIVE_UINT_TYPE>(argBufSize);
        // deser directly into arg buf
        deserStatus = m_sequenceBuffer.deserialize(statement.getargBuf(), argBufSizeUint, true);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                          m_sequenceBuffer.getBuffLeft(),
                                                          m_sequenceBuffer.getBuffLength());
            return false;
        }

        statementIdx++;
    }

    // read footer bytes (but don't include in CRC)
    readStatus = readBytes(sequenceFile, Fpy::Footer::SERIALIZED_SIZE, false);
    if (!readStatus) {
        return false;
    }

    U32 crc;
    deserStatus = m_sequenceBuffer.deserialize(crc);
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                      m_sequenceBuffer.getBuffLeft(), m_sequenceBuffer.getBuffLength());
        return false;
    }
    m_sequenceObj.getfooter().setcrc(crc);

    // need this for some reason to "finalize" the crc TODO get an explanation on this
    this->m_computedCRC = ~this->m_computedCRC;

    if (this->m_computedCRC != m_sequenceObj.getfooter().getcrc()) {
        this->log_WARNING_LO_WrongCRC(m_sequenceObj.getfooter().getcrc(), m_computedCRC);
        return false;
    }

    return true;
}

// reads some bytes from the open file into the m_sequenceBuffer.
// return true if successful
bool FpySequencer::readBytes(Os::File& file, FwSizeType readLen, bool updateCRC) {
    FW_ASSERT(file.isOpen());
    FwSignedSizeType actualReadLen = static_cast<FwSignedSizeType>(readLen);

    const NATIVE_UINT_TYPE capacity = m_sequenceBuffer.getBuffCapacity();
    FW_ASSERT(capacity >= static_cast<NATIVE_UINT_TYPE>(actualReadLen), static_cast<FwAssertArgType>(capacity),
              static_cast<FwAssertArgType>(actualReadLen));
    Os::File::Status fileStatus = file.read(m_sequenceBuffer.getBuffAddr(), actualReadLen);

    if (fileStatus != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(m_sequenceFilePath, static_cast<I32>(fileStatus));
        return false;
    }

    if (static_cast<FwSignedSizeType>(readLen) != actualReadLen) {
        this->log_WARNING_HI_EndOfFileError(m_sequenceFilePath);
        return false;
    }

    Fw::SerializeStatus serializeStatus = m_sequenceBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

    if (updateCRC) {
        updateComputedCRC(m_sequenceBuffer.getBuffAddr(), readLen);
    }

    return true;
}

}  // namespace Svc