#include "Svc/FpySequencer/FppConstantsAc.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
#include "Svc/FpySequencer/StatementTypeEnumAc.hpp"
extern "C" {
#include "Utils/Hash/libcrc/lib_crc.h"
}
namespace Svc {

void FpySequencer::allocateBuffer(FwEnumStoreType identifier, Fw::MemAllocator& allocator, FwSizeType bytes) {
    // if this assertion fails, you aren't allocating enough bytes for the
    // FpySequencer. this is because you must have a buffer big enough to fit the
    // header of a sequence
    FW_ASSERT(bytes >= Fpy::Header::SERIALIZED_SIZE, static_cast<FwAssertArgType>(bytes));
    FwSizeType originalBytes = bytes;
    bool recoverable = false;
    this->m_allocatorId = identifier;
    U8* allocatedMemory = static_cast<U8*>(allocator.allocate(identifier, bytes, recoverable));
    // if this fails, unable to allocate the requested amount of money
    FW_ASSERT(bytes >= originalBytes, static_cast<FwAssertArgType>(bytes));
    this->m_sequenceBuffer.setExtBuffer(allocatedMemory, bytes);
}

void FpySequencer::deallocateBuffer(Fw::MemAllocator& allocator) {
    allocator.deallocate(this->m_allocatorId, this->m_sequenceBuffer.getBuffAddr());
    this->m_sequenceBuffer.clear();
}

void FpySequencer::updateComputedCRC(const U8* buffer, FwSizeType bufferSize) {
    FW_ASSERT(buffer);
    for (FwSizeType index = 0; index < bufferSize; index++) {
        this->m_computedCRC = static_cast<U32>(update_crc_32(this->m_computedCRC, static_cast<char>(buffer[index])));
    }
}

// loads the sequence in memory, and does header/crc/integrity checks.
// return true if sequence is valid
Fw::Success FpySequencer::validate() {
    FW_ASSERT(this->m_sequenceFilePath.length() > 0);

    // crc needs to be initialized with a particular value
    // for the calculation to work
    this->m_computedCRC = CRC_INITIAL_VALUE;

    Os::File sequenceFile;
    Os::File::Status openStatus = sequenceFile.open(this->m_sequenceFilePath.toChar(), Os::File::OPEN_READ);

    if (openStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileOpenError(this->m_sequenceFilePath, static_cast<I32>(openStatus));
        return Fw::Success::FAILURE;
    }

    // read header bytes
    bool readStatus = this->readBytes(sequenceFile, Fpy::Header::SERIALIZED_SIZE);
    if (!readStatus) {
        return Fw::Success::FAILURE;
    }

    // deser header
    Fw::SerializeStatus deserStatus = this->m_sequenceBuffer.deserialize(this->m_sequenceObj.getheader());
    if (deserStatus != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(this->m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                      this->m_sequenceBuffer.getBuffLeft(), this->m_sequenceBuffer.getBuffLength());
        return Fw::Success::FAILURE;
    }

    // check matching schema version
    if (this->m_sequenceObj.getheader().getschemaVersion() != Fpy::SCHEMA_VERSION) {
        this->log_WARNING_HI_WrongSchemaVersion(Fpy::SCHEMA_VERSION, this->m_sequenceObj.getheader().getschemaVersion());
        return Fw::Success::FAILURE;
    }

    if (this->m_sequenceObj.getheader().getargumentCount() > Fpy::MAX_SEQUENCE_ARG_COUNT) {
        this->log_WARNING_HI_TooManySequenceArgs(m_sequenceObj.getheader().getargumentCount(), Fpy::MAX_SEQUENCE_ARG_COUNT);
        return Fw::Success::FAILURE;
    }

    if (this->m_sequenceObj.getheader().getstatementCount() > Fpy::MAX_SEQUENCE_STATEMENT_COUNT) {
        this->log_WARNING_HI_TooManySequenceStatements(this->m_sequenceObj.getheader().getstatementCount(), Fpy::MAX_SEQUENCE_STATEMENT_COUNT);
        return Fw::Success::FAILURE;
    }

    // read body bytes
    readStatus = readBytes(sequenceFile, this->m_sequenceObj.getheader().getbodySize());
    if (!readStatus) {
        return Fw::Success::FAILURE;
    }

    // deser body:
    // deser arg mappings
    for (U8 argMappingIdx = 0; argMappingIdx < this->m_sequenceObj.getheader().getargumentCount(); argMappingIdx++) {
        // local variable index of arg $argMappingIdx
        deserStatus = this->m_sequenceBuffer.deserialize(this->m_sequenceObj.getargs()[argMappingIdx]);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(this->m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                          this->m_sequenceBuffer.getBuffLeft(),
                                                          this->m_sequenceBuffer.getBuffLength());
            return Fw::Success::FAILURE;
        }
    }

    // deser statements
    for (U16 statementIdx = 0; statementIdx < this->m_sequenceObj.getheader().getstatementCount(); statementIdx++) {
        // deser statement
        deserStatus = this->m_sequenceBuffer.deserialize(this->m_sequenceObj.getstatements()[statementIdx]);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(this->m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                          this->m_sequenceBuffer.getBuffLeft(),
                                                          this->m_sequenceBuffer.getBuffLength());
            return Fw::Success::FAILURE;
        }
    }

    // read footer bytes (but don't include in CRC)
    readStatus = this->readBytes(sequenceFile, Fpy::Footer::SERIALIZED_SIZE, false);
    if (!readStatus) {
        return Fw::Success::FAILURE;
    }

    deserStatus = this->m_sequenceBuffer.deserialize(this->m_sequenceObj.getfooter());
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(this->m_sequenceFilePath, static_cast<I32>(deserStatus),
                                                      this->m_sequenceBuffer.getBuffLeft(), this->m_sequenceBuffer.getBuffLength());
        return Fw::Success::FAILURE;
    }

    // need this for some reason to "finalize" the crc TODO get an explanation on this
    this->m_computedCRC = ~this->m_computedCRC;

    if (this->m_computedCRC != this->m_sequenceObj.getfooter().getcrc()) {
        this->log_WARNING_HI_WrongCRC(this->m_sequenceObj.getfooter().getcrc(), this->m_computedCRC);
        return Fw::Success::FAILURE;
    }

    // make sure we're at EOF
    FwSizeType sequenceFileSize;
    FW_ASSERT(sequenceFile.size(sequenceFileSize) == Os::File::Status::OP_OK);

    FwSizeType sequenceFilePosition;
    FW_ASSERT(sequenceFile.position(sequenceFilePosition) == Os::File::Status::OP_OK);

    if (sequenceFileSize != sequenceFilePosition) {
        this->log_WARNING_HI_ExtraBytesInSequence(static_cast<U32>(sequenceFileSize - sequenceFilePosition));
        return Fw::Success::FAILURE;
    }

    return Fw::Success::SUCCESS;
}

// reads some bytes from the open file into the m_sequenceBuffer.
// return true if successful
Fw::Success FpySequencer::readBytes(Os::File& file, FwSizeType readLen, bool updateCRC) {
    FW_ASSERT(file.isOpen());
    // this has to be declared a var because file.read must take a ref
    FwSizeType expectedReadLen = readLen;

    const FwSizeType capacity = this->m_sequenceBuffer.getBuffCapacity();

    // if this fails, then you need to give the sequencer more buffer memory. pass in a bigger number
    // to fpySeq.allocateBuffer(). This is usually done in topology setup CPP
    if (expectedReadLen > capacity) {
        this->log_WARNING_HI_InsufficientBufferSpace(static_cast<U64>(capacity), this->m_sequenceFilePath);
        return Fw::Success::FAILURE;
    }

    Os::File::Status fileStatus = file.read(this->m_sequenceBuffer.getBuffAddr(), expectedReadLen);

    if (fileStatus != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(this->m_sequenceFilePath, static_cast<I32>(fileStatus));
        return Fw::Success::FAILURE;
    }

    if (readLen < expectedReadLen) {
        this->log_WARNING_HI_EndOfFileError(this->m_sequenceFilePath);
        return Fw::Success::FAILURE;
    }

    // should probably fail if we read in MORE bytes than we ask for
    FW_ASSERT(readLen == expectedReadLen, static_cast<FwAssertArgType>(readLen), static_cast<FwAssertArgType>(expectedReadLen));

    Fw::SerializeStatus serializeStatus = this->m_sequenceBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(readLen));
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

    if (updateCRC) {
        this->updateComputedCRC(this->m_sequenceBuffer.getBuffAddr(), readLen);
    }

    return Fw::Success::SUCCESS;
}

}  // namespace Svc
