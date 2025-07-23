#include "Svc/FpySequencer/FppConstantsAc.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"
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

void FpySequencer::updateCrc(U32& crc, const U8* buffer, FwSizeType bufferSize) {
    FW_ASSERT(buffer);
    for (FwSizeType index = 0; index < bufferSize; index++) {
        crc = static_cast<U32>(update_crc_32(crc, static_cast<char>(buffer[index])));
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

    Fw::Success readStatus =
        this->readBytes(sequenceFile, Fpy::Header::SERIALIZED_SIZE, FpySequencer_FileReadStage::HEADER);

    if (readStatus != Fw::Success::SUCCESS) {
        return Fw::Success::FAILURE;
    }

    readStatus = this->readHeader();

    if (readStatus != Fw::Success::SUCCESS) {
        return Fw::Success::FAILURE;
    }

    readStatus =
        readBytes(sequenceFile, this->m_sequenceObj.get_header().get_bodySize(), FpySequencer_FileReadStage::BODY);

    if (readStatus != Fw::Success::SUCCESS) {
        return Fw::Success::FAILURE;
    }

    readStatus = this->readBody();

    if (readStatus != Fw::Success::SUCCESS) {
        return Fw::Success::FAILURE;
    }

    // read footer bytes but don't include in CRC
    readStatus = this->readBytes(sequenceFile, Fpy::Footer::SERIALIZED_SIZE, FpySequencer_FileReadStage::FOOTER, false);

    if (readStatus != Fw::Success::SUCCESS) {
        return Fw::Success::FAILURE;
    }

    readStatus = this->readFooter();

    if (readStatus != Fw::Success::SUCCESS) {
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

Fw::Success FpySequencer::readHeader() {
    // deser header
    Fw::SerializeStatus deserStatus = this->m_sequenceBuffer.deserialize(this->m_sequenceObj.get_header());
    if (deserStatus != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(
            FpySequencer_FileReadStage::HEADER, this->m_sequenceFilePath, static_cast<I32>(deserStatus),
            this->m_sequenceBuffer.getBuffLeft(), this->m_sequenceBuffer.getBuffLength());
        return Fw::Success::FAILURE;
    }

    // check matching schema version
    if (this->m_sequenceObj.get_header().get_schemaVersion() != Fpy::SCHEMA_VERSION) {
        this->log_WARNING_HI_WrongSchemaVersion(Fpy::SCHEMA_VERSION,
                                                this->m_sequenceObj.get_header().get_schemaVersion());
        return Fw::Success::FAILURE;
    }

    if (this->m_sequenceObj.get_header().get_argumentCount() > Fpy::MAX_SEQUENCE_ARG_COUNT) {
        this->log_WARNING_HI_TooManySequenceArgs(m_sequenceObj.get_header().get_argumentCount(),
                                                 Fpy::MAX_SEQUENCE_ARG_COUNT);
        return Fw::Success::FAILURE;
    }

    if (this->m_sequenceObj.get_header().get_statementCount() > Fpy::MAX_SEQUENCE_STATEMENT_COUNT) {
        this->log_WARNING_HI_TooManySequenceStatements(this->m_sequenceObj.get_header().get_statementCount(),
                                                       Fpy::MAX_SEQUENCE_STATEMENT_COUNT);
        return Fw::Success::FAILURE;
    }
    return Fw::Success::SUCCESS;
}

Fw::Success FpySequencer::readBody() {
    Fw::SerializeStatus deserStatus;
    // deser body:
    // deser arg mappings
    for (U8 argMappingIdx = 0; argMappingIdx < this->m_sequenceObj.get_header().get_argumentCount(); argMappingIdx++) {
        // serializable register index of arg $argMappingIdx
        // TODO should probably check that this serReg is inside range
        deserStatus = this->m_sequenceBuffer.deserialize(this->m_sequenceObj.get_args()[argMappingIdx]);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(
                FpySequencer_FileReadStage::BODY, this->m_sequenceFilePath, static_cast<I32>(deserStatus),
                this->m_sequenceBuffer.getBuffLeft(), this->m_sequenceBuffer.getBuffLength());
            return Fw::Success::FAILURE;
        }
    }

    // deser statements
    for (U16 statementIdx = 0; statementIdx < this->m_sequenceObj.get_header().get_statementCount(); statementIdx++) {
        // deser statement
        deserStatus = this->m_sequenceBuffer.deserialize(this->m_sequenceObj.get_statements()[statementIdx]);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(
                FpySequencer_FileReadStage::BODY, this->m_sequenceFilePath, static_cast<I32>(deserStatus),
                this->m_sequenceBuffer.getBuffLeft(), this->m_sequenceBuffer.getBuffLength());
            return Fw::Success::FAILURE;
        }
    }
    return Fw::Success::SUCCESS;
}

Fw::Success FpySequencer::readFooter() {
    Fw::SerializeStatus deserStatus = this->m_sequenceBuffer.deserialize(this->m_sequenceObj.get_footer());
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(
            FpySequencer_FileReadStage::FOOTER, this->m_sequenceFilePath, static_cast<I32>(deserStatus),
            this->m_sequenceBuffer.getBuffLeft(), this->m_sequenceBuffer.getBuffLength());
        return Fw::Success::FAILURE;
    }

    // need this for some reason to "finalize" the crc TODO get an explanation on this
    this->m_computedCRC = ~this->m_computedCRC;

    if (this->m_computedCRC != this->m_sequenceObj.get_footer().get_crc()) {
        this->log_WARNING_HI_WrongCRC(this->m_sequenceObj.get_footer().get_crc(), this->m_computedCRC);
        return Fw::Success::FAILURE;
    }

    return Fw::Success::SUCCESS;
}

// reads some bytes from the open file into the m_sequenceBuffer.
// return success if successful
Fw::Success FpySequencer::readBytes(Os::File& file,
                                    FwSizeType expectedReadLen,
                                    const FpySequencer_FileReadStage& readStage,
                                    bool updateCrc) {
    FW_ASSERT(file.isOpen());
    // this has to be declared a var because file.read must take a ref
    FwSizeType actualReadLen = expectedReadLen;

    const FwSizeType capacity = this->m_sequenceBuffer.getBuffCapacity();

    // if this fails, then you need to give the sequencer more buffer memory. pass in a bigger number
    // to fpySeq.allocateBuffer(). This is usually done in topology setup CPP
    if (expectedReadLen > capacity) {
        this->log_WARNING_HI_InsufficientBufferSpace(static_cast<U64>(capacity), this->m_sequenceFilePath);
        return Fw::Success::FAILURE;
    }

    Os::File::Status fileStatus = file.read(this->m_sequenceBuffer.getBuffAddr(), actualReadLen);

    if (fileStatus != Os::File::OP_OK) {
        this->log_WARNING_HI_FileReadError(readStage, this->m_sequenceFilePath, static_cast<I32>(fileStatus));
        return Fw::Success::FAILURE;
    }

    if (actualReadLen < expectedReadLen) {
        this->log_WARNING_HI_EndOfFileError(readStage, this->m_sequenceFilePath);
        return Fw::Success::FAILURE;
    }

    // should probably fail if we read in MORE bytes than we ask for
    FW_ASSERT(expectedReadLen == actualReadLen, static_cast<FwAssertArgType>(expectedReadLen),
              static_cast<FwAssertArgType>(actualReadLen));

    Fw::SerializeStatus serializeStatus =
        this->m_sequenceBuffer.setBuffLen(static_cast<Fw::Serializable::SizeType>(expectedReadLen));
    FW_ASSERT(serializeStatus == Fw::FW_SERIALIZE_OK, serializeStatus);

    if (updateCrc) {
        FpySequencer::updateCrc(this->m_computedCRC, this->m_sequenceBuffer.getBuffAddr(), expectedReadLen);
    }

    return Fw::Success::SUCCESS;
}

}  // namespace Svc
