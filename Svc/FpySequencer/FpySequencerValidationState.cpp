#include <Utils/Hash/Hash.hpp>
#include "Svc/FpySequencer/FppConstantsAc.hpp"
#include "Svc/FpySequencer/FpySequencer.hpp"

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

// loads the sequence in memory, and does header/crc/integrity checks.
// return SUCCESS if sequence is valid, FAILURE otherwise
Fw::Success FpySequencer::validate() {
    FW_ASSERT(this->m_sequenceFilePath.length() > 0);

    // crc needs to be initialized with a particular value
    // for the calculation to work
    this->m_computedCRC.init();

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

    // make sure we're at EOF. The size() and position() OS calls can fail
    // on filesystem errors or if the file was concurrently modified by
    // another FileManager command (e.g. RemoveFile or AppendFile) between
    // the file open and this point. Treat the failure as a validation
    // failure rather than aborting the FSW process.
    FwSizeType sequenceFileSize;
    Os::File::Status sizeStatus = sequenceFile.size(sequenceFileSize);
    if (sizeStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileApiError(this->m_sequenceFilePath, static_cast<I32>(sizeStatus));
        return Fw::Success::FAILURE;
    }

    FwSizeType sequenceFilePosition;
    Os::File::Status positionStatus = sequenceFile.position(sequenceFilePosition);
    if (positionStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileApiError(this->m_sequenceFilePath, static_cast<I32>(positionStatus));
        return Fw::Success::FAILURE;
    }

    if (sequenceFileSize != sequenceFilePosition) {
        this->log_WARNING_HI_ExtraBytesInSequence(static_cast<FwSizeType>(sequenceFileSize - sequenceFilePosition));
        return Fw::Success::FAILURE;
    }

    Fpy::StackSizeType availableSpace = Fpy::MAX_STACK_SIZE - this->m_runtime.stack.size;

    if (this->m_sequenceArgs.get_size() > availableSpace) {
        return Fw::Success::FAILURE;
    }

    return Fw::Success::SUCCESS;
}

// reads and validates the header from the m_sequenceBuffer
// return SUCCESS if sequence is valid, FAILURE otherwise
Fw::Success FpySequencer::readHeader() {
    // deser header
    Fw::SerializeStatus deserStatus = this->m_sequenceBuffer.deserializeTo(this->m_sequenceObj.get_header());
    if (deserStatus != Fw::SerializeStatus::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(
            FpySequencer_FileReadStage::HEADER, this->m_sequenceFilePath, static_cast<I32>(deserStatus),
            this->m_sequenceBuffer.getDeserializeSizeLeft(), this->m_sequenceBuffer.getSize());
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
        this->log_WARNING_HI_TooManySequenceDirectives(this->m_sequenceObj.get_header().get_statementCount(),
                                                       Fpy::MAX_SEQUENCE_STATEMENT_COUNT);
        return Fw::Success::FAILURE;
    }
    return Fw::Success::SUCCESS;
}

// reads and validates the body from the m_sequenceBuffer
// return SUCCESS if sequence is valid, FAILURE otherwise
Fw::Success FpySequencer::readBody() {
    Fw::SerializeStatus deserStatus;

    const U8 argumentCount = this->m_sequenceObj.get_header().get_argumentCount();
    this->m_totalExpectedArgSize = 0;

    // deser arguments
    // Read and deserialize each arg_spec incrementally since they're variable-length
    for (U8 i = 0; i < argumentCount; i++) {
        Fpy::ArgSpec& argSpec = this->m_sequenceObj.get_args()[i];
        deserStatus = this->m_sequenceBuffer.deserializeTo(argSpec);
        if (deserStatus != Fw::SerializeStatus::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(
                FpySequencer_FileReadStage::BODY, this->m_sequenceFilePath, static_cast<I32>(deserStatus),
                this->m_sequenceBuffer.getDeserializeSizeLeft(), this->m_sequenceBuffer.getSize());
            return Fw::Success::FAILURE;
        }

        m_totalExpectedArgSize += argSpec.get_argSize();
    }

    // Check for overflow
    if (m_totalExpectedArgSize > Fpy::MAX_STACK_SIZE) {
        this->log_WARNING_HI_ArgTotalSizeExceedsStackLimit(m_totalExpectedArgSize);
        return Fw::Success::FAILURE;
    }

    // Validate total argument size
    if (this->m_totalExpectedArgSize != this->m_sequenceArgs.get_size()) {
        this->log_WARNING_HI_ArgSizeMismatch(this->m_totalExpectedArgSize, this->m_sequenceArgs.get_size(),
                                             this->m_sequenceFilePath);
        return Fw::Success::FAILURE;
    }

    // deser statements
    for (U16 statementIdx = 0; statementIdx < this->m_sequenceObj.get_header().get_statementCount(); statementIdx++) {
        // deser statement
        deserStatus = this->m_sequenceBuffer.deserializeTo(this->m_sequenceObj.get_statements()[statementIdx]);
        if (deserStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_FileReadDeserializeError(
                FpySequencer_FileReadStage::BODY, this->m_sequenceFilePath, static_cast<I32>(deserStatus),
                this->m_sequenceBuffer.getDeserializeSizeLeft(), this->m_sequenceBuffer.getSize());
            return Fw::Success::FAILURE;
        }
    }
    return Fw::Success::SUCCESS;
}

// reads and validates the footer from the m_sequenceBuffer
// return SUCCESS if sequence is valid, FAILURE otherwise
Fw::Success FpySequencer::readFooter() {
    Fw::SerializeStatus deserStatus = this->m_sequenceBuffer.deserializeTo(this->m_sequenceObj.get_footer());
    if (deserStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_FileReadDeserializeError(
            FpySequencer_FileReadStage::FOOTER, this->m_sequenceFilePath, static_cast<I32>(deserStatus),
            this->m_sequenceBuffer.getDeserializeSizeLeft(), this->m_sequenceBuffer.getSize());
        return Fw::Success::FAILURE;
    }

    // need this for some reason to "finalize" the crc TODO get an explanation on this
    U32 computedCRC = 0;
    this->m_computedCRC.finalize(computedCRC);

    if (computedCRC != this->m_sequenceObj.get_footer().get_crc()) {
        this->log_WARNING_HI_WrongCRC(this->m_sequenceObj.get_footer().get_crc(), computedCRC);
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

    const FwSizeType capacity = this->m_sequenceBuffer.getCapacity();

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
        this->m_computedCRC.update(this->m_sequenceBuffer.getBuffAddr(), expectedReadLen);
    }

    return Fw::Success::SUCCESS;
}

}  // namespace Svc
