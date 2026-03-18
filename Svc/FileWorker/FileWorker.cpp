// ======================================================================
// \title  FileWorker.cpp
// \author racheljt
// \brief  cpp file for FileWorker component implementation class
// ======================================================================

#include "Svc/FileWorker/FileWorker.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FileWorker ::FileWorker(const char* const compName)
    : FileWorkerComponentBase(compName), m_state(FileWorkerState::FW_STATE_IDLE), m_abort(false), m_chunkSize(0) {}

void FileWorker ::configure(U64 chunkSize) {
    FW_ASSERT(chunkSize > 0);
    this->m_chunkSize = chunkSize;
}

FileWorker ::~FileWorker() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void FileWorker ::cancelIn_handler(FwIndexType portNum) {
    this->m_abort.store(true, std::memory_order_relaxed);
}

void FileWorker ::readIn_handler(FwIndexType portNum, const Fw::StringBase& path, Fw::Buffer& buffer) {
    FW_ASSERT(path != nullptr);
    FW_ASSERT(path.length() > 0);
    FW_ASSERT(buffer.getData() != nullptr);

    const char* const fileName = path.toChar();
    FwSizeType fileSize = 0;

    if (this->m_state != FW_STATE_IDLE) {
        this->log_WARNING_HI_NotInIdle(this->m_state);
        this->readDoneOut_out(0, FW_STATUS_NOT_IDLE, 0);
        return;
    }

    // New read request overrides any leftover abort state
    this->m_abort.store(false, std::memory_order_relaxed);

    this->m_state = FW_STATE_READING;

    // Check CRC
    U32 crcFromFile = 0;
    U32 crcCalculated = 0;
    Utils::crc_stat_t crcStat = Utils::verify_checksum(fileName, crcFromFile, crcCalculated);
    if (crcStat != Utils::PASSED_FILE_CRC_CHECK) {
        this->log_WARNING_HI_CrcFailed(crcStat);
        this->readDoneOut_out(0, FW_STATUS_FAILED_CRC, 0);
        this->m_state = FW_STATE_IDLE;
        return;
    }

    // Get filesize
    Os::FileSystem::Status fsStat = Os::FileSystem::getFileSize(fileName, fileSize);
    FW_ASSERT(fsStat == Os::FileSystem::OP_OK, fsStat);  // file size checked with checksum

    // Start reading
    FileWorkerStatus workerStat = this->readBufferFromFile(buffer, fileName);

    // Signal done and pass U8* buffer with data
    this->readDoneOut_out(0, workerStat, fileSize);
    this->m_state = FW_STATE_IDLE;
}

void FileWorker ::verifyIn_handler(FwIndexType portNum, const Fw::StringBase& path, U32 crc) {
    FW_ASSERT(path != nullptr);
    FW_ASSERT(path.length() > 0);

    const char* const fileName = path.toChar();
    FwSizeType fileSize = 0;
    FileWorkerStatus workerStat = FW_STATUS_DONE;

    U32 crcFromFile = 0;
    U32 crcCalculated = 0;
    Utils::crc_stat_t crcStat = Utils::verify_checksum(fileName, crcFromFile, crcCalculated);

    if (crcStat != Utils::PASSED_FILE_CRC_CHECK) {
        this->log_WARNING_HI_CrcFailed(crcStat);
        workerStat = FW_STATUS_FAILED_CRC;
    }

    if (crc != crcFromFile) {
        workerStat = FW_STATUS_FAILED_CRC;
        this->log_WARNING_LO_CrcVerificationError(crc, crcCalculated);
    }

    // Get filesize
    Os::FileSystem::Status fsStat = Os::FileSystem::getFileSize(fileName, fileSize);
    if (fsStat != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_ReadFailedFileSize(fsStat);
        workerStat = FW_STATUS_FAILED_FILE_SIZE;
    }

    this->verifyDoneOut_out(0, workerStat, fileSize);
}

void FileWorker ::writeIn_handler(FwIndexType portNum,
                                  const Fw::StringBase& path,
                                  Fw::Buffer& buffer,
                                  FwSizeType offsetBytes,
                                  bool append) {
    FW_ASSERT(path != nullptr);
    FW_ASSERT(path.length() > 0);
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(offsetBytes <= buffer.getSize());

    char fileName[FileNameStringSize];

    // Make sure we are in IDLE state before proceeding
    if (this->m_state != FW_STATE_IDLE) {
        this->log_WARNING_HI_NotInIdle(this->m_state);
        this->writeDoneOut_out(0, FW_STATUS_NOT_IDLE, 0);
        return;
    }

    this->m_state = FW_STATE_WRITING;

    // New write request overrides any leftover abort state
    this->m_abort.store(false, std::memory_order_relaxed);

    // Save file name
    // NB: may count null terminator due to FPRIME/fprime-sw#57, but should still be less than FileNameStringSize in any
    // case
    FwSizeType length = Fw::StringUtils::string_length(path.toChar(), FileNameStringSize);
    FW_ASSERT(length < FileNameStringSize && length < sizeof(fileName));

    (void)Fw::StringUtils::string_copy(fileName, path.toChar(), sizeof(fileName));
    fileName[sizeof(fileName) - 1] = 0;  // guarantee termination

    // Write
    bool isWrite = this->writeBufferToFile(buffer, fileName, offsetBytes, append);
    if (isWrite) {
        this->writeBufferHashToFile(buffer, fileName, offsetBytes, append);
    }

    this->writeDoneOut_out(0, FW_STATUS_DONE_WRITE, buffer.getSize());
    this->m_state = FW_STATE_IDLE;
    return;
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

Svc ::FileWorkerStatus FileWorker ::readBufferFromFile(Fw::Buffer& buffer, const char* const fileName) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(fileName != nullptr);

    Fw::LogStringArg fileNameStr(fileName);
    Os::File file;

    // Open file
    Os::File::Status fileStat = file.open(fileName, Os::File::OPEN_READ);
    if (fileStat != Os::File::OP_OK) {
        this->log_WARNING_HI_OpenFileError(fileNameStr, fileStat);
        return FW_STATUS_FAILED_TO_OPEN;
    }

    // Get buffer data and size
    FwSizeType readSize = buffer.getSize();

    // Read file
    this->log_ACTIVITY_LO_ReadBegin(readSize, fileNameStr);
    this->readFile(buffer, readSize, file, fileNameStr);

    this->log_ACTIVITY_LO_ReadCompleted(readSize, fileNameStr);
    file.close();

    return FileWorkerStatus::FW_STATUS_DONE_READ;
}

void FileWorker ::readFile(Fw::Buffer& buffer, FwSizeType size, Os::File& file, const Fw::LogStringArg& fileNameStr) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(size > 0);
    FW_ASSERT(fileNameStr != nullptr);

    FwSizeType bytesRead = 0;
    FwSizeType numChunks = 0;
    U64 timeout = 0;

    if (!file.isOpen()) {
        return;
    }

    FileWorkerReadStatus readStat = this->readFileBytes(buffer, size, file, bytesRead);

    switch (readStat) {
        case FW_READ_ERROR:
            // Some read error
            this->log_WARNING_HI_ReadError(bytesRead, size, fileNameStr);
            break;

        case FW_READ_DONE:
            break;

        case FW_READ_ABORT:
            // Abort command was sent
            this->log_WARNING_LO_ReadAborted(bytesRead, size, fileNameStr);
            break;

        case FW_READ_TIMEOUT:
            // Determine true timeout
            static_assert(BLOCK_SIZE_BYTES > 0, "Divide by 0 error");
            numChunks = (size / BLOCK_SIZE_BYTES);
            if (size % BLOCK_SIZE_BYTES > 0) {
                numChunks += 1;
            }
            timeout = numChunks * TIMEOUT_MS;
            this->log_WARNING_HI_ReadTimeout(bytesRead, size, fileNameStr, timeout);
            break;

        default:
            FW_ASSERT(0);  // Should not get here
            break;
    }

    return;
}

Svc ::FileWorkerReadStatus FileWorker ::readFileBytes(Fw::Buffer& buffer,
                                                      FwSizeType size,
                                                      Os::File& file,
                                                      FwSizeType& bytesRead) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(size > 0);

    // Determine true timeout
    static_assert(BLOCK_SIZE_BYTES > 0, "Divide by 0 error");
    FwSizeType numChunks = (size / BLOCK_SIZE_BYTES);
    if (size % BLOCK_SIZE_BYTES > 0) {
        numChunks += 1;
    }
    U64 timeout = numChunks * TIMEOUT_MS;

    // Read loop
    bytesRead = 0;
    Fw::Time start = this->getTime();

    for (U32 i = 0; i < MAX_LOOP_ITERATIONS; i++) {
        FwSizeType readAmt = FW_MIN(size - bytesRead, BLOCK_SIZE_BYTES);
        FwSizeType readAmtActual = readAmt;
        Os::File::Status ret = file.read(buffer.getData() + bytesRead, readAmtActual);

        if (Os::File::OP_OK != ret || readAmt != readAmtActual) {
            return FileWorkerReadStatus::FW_READ_ERROR;
        }

        bool currAbort = this->m_abort.load(std::memory_order_relaxed);
        if (currAbort) {
            // Abort command was sent
            return FileWorkerReadStatus::FW_READ_ABORT;
        }

        if (timeout > 0) {
            // Only check timeout if > 0
            Fw::Time now = this->getTime();
            Fw::Time diff = Fw::Time::sub(now, start);
            U64 elapsed = (diff.getSeconds() * 1000000) + diff.getUSeconds();
            if (elapsed >= timeout) {
                return FileWorkerReadStatus::FW_READ_TIMEOUT;
            }
        }

        bytesRead += readAmt;
        if (bytesRead >= size) {
            // Finished, break out
            return FileWorkerReadStatus::FW_READ_DONE;
        }
    }

    return FileWorkerReadStatus::FW_READ_UNKNOWN;
}

bool FileWorker ::getHash(const char* const hashFileName,
                          Utils::Hash& hash,
                          Utils::HashBuffer& hashBuffer,
                          const U8* const data,
                          const FwSizeType size) {
    FW_ASSERT(hashFileName != nullptr);
    FW_ASSERT(data != nullptr);
    FW_ASSERT(size > 0);

    // Open file
    Os::File file;
    Os::File::Status stat = file.open(hashFileName, Os::File::OPEN_READ);

    // Read value if it exists
    if (stat == Os::File::OP_OK) {
        HASH_HANDLE_TYPE hashValue;
        FwSizeType hashSize = sizeof(hashValue);
        U8* hashValuePtr = reinterpret_cast<U8*>(&hashValue);
        FW_ASSERT(hashValuePtr != nullptr);

        Os::File::Status readStat = file.read(hashValuePtr, hashSize);
        if (readStat != Os::File::OP_OK) {
            Fw::LogStringArg s(hashFileName);
            this->log_WARNING_HI_WriteValidationReadError(s, readStat);
            return false;
        }
        Utils::HashBuffer tmp(hashValuePtr, hashSize);
        hash.setHashValue(tmp);
        hash.update(data, size);
        hash.finalize(hashBuffer);

    } else if (stat == Os::File::DOESNT_EXIST) {
        hash.hash(data, size, hashBuffer);

    } else {
        Fw::LogStringArg s(hashFileName);
        this->log_WARNING_HI_WriteValidationOpenError(s, stat);
        return false;
    }

    return true;
}

bool FileWorker ::writeBufferToFile(Fw::Buffer& buffer, const char* fileName, FwSizeType offset, bool append) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(fileName != nullptr);

    Fw::LogStringArg logStringArg(fileName);
    Os::File file;
    Os::File::Status stat = Os::File::OP_OK;

    // Open file
    if (!append) {
        stat = file.open(fileName, Os::File::Mode::OPEN_WRITE);
    } else {
        stat = file.open(fileName, Os::File::Mode::OPEN_APPEND);
    }

    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_OpenFileError(logStringArg, stat);
        return false;
    }

    // Get buffer data and size
    FwSizeType size = buffer.getSize();
    U8* const data = reinterpret_cast<U8*>(buffer.getData());
    FW_ASSERT(data != nullptr);

    // Apply offset
    FW_ASSERT(offset <= size);
    size -= offset;
    U8* const dataFromOffset = reinterpret_cast<U8*>(data + offset);
    FW_ASSERT(dataFromOffset != nullptr);

    // Write file
    this->log_ACTIVITY_LO_WriteBegin(size, logStringArg);
    FwSizeType writtenSize = this->writeToFile(dataFromOffset, size, file, fileName);

    // Check written size
    if (writtenSize != size) {
        return false;
    }

    this->log_ACTIVITY_LO_WriteCompleted(size, logStringArg);
    return true;
}

void FileWorker ::writeBufferHashToFile(Fw::Buffer& buffer, const char* fileName, FwSizeType offset, bool append) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(fileName != nullptr);

    // Construct hash file name
    const char* ext = Utils::Hash::getFileExtensionString();
    FW_ASSERT(ext != nullptr);
    char hashFileName[FileNameStringSize];
    Fw::FormatStatus status = Fw::stringFormat(hashFileName, sizeof(hashFileName), "%s%s", fileName, ext);
    FW_ASSERT(status == Fw::FormatStatus::SUCCESS);

    // Compute hash
    Utils::HashBuffer hashBuffer;
    FwSizeType size = buffer.getSize();
    U8* const data = reinterpret_cast<U8*>(buffer.getData());
    FW_ASSERT(data != nullptr);

    // Apply offset
    FW_ASSERT(offset <= size);
    size -= offset;  // checked by assert
    U8* const dataFromOffset = reinterpret_cast<U8*>(data + offset);
    FW_ASSERT(dataFromOffset != nullptr);

    Utils::Hash hash;
    if (!append) {
        hash.hash(dataFromOffset, size, hashBuffer);

    } else {
        bool isHash = this->getHash(hashFileName, hash, hashBuffer, dataFromOffset, size);
        if (!isHash) {
            return;
        }
    }

    // Open file
    Os::File file;
    Os::File::Status stat = file.open(hashFileName, Os::File::Mode::OPEN_WRITE);
    if (stat != Os::File::OP_OK) {
        Fw::LogStringArg logStringArg(hashFileName);
        this->log_WARNING_HI_OpenFileError(logStringArg, stat);
        return;
    }

    // Write hash
    FwSizeType writtenSize = this->writeToFile(hashBuffer.getBuffAddr(), hashBuffer.getSize(), file, hashFileName);

    // Check written size
    FwSizeType hashSize = hashBuffer.getSize();
    if (writtenSize != hashSize) {
        Fw::LogStringArg logStringArg(hashFileName);
        this->log_WARNING_LO_WriteValidationError(logStringArg, writtenSize, hashSize);
        return;
    }

    return;
}

FwSizeType FileWorker ::writeToFile(const U8* data, FwSizeType size, Os::File& file, const char* fileName) {
    FW_ASSERT(data != nullptr);
    FW_ASSERT(size > 0);
    FW_ASSERT(file.isOpen());
    FW_ASSERT(fileName != nullptr);

    // Determine true timeout
    static_assert(BLOCK_SIZE_BYTES > 0, "Divide by 0 error");
    FwSizeType numChunks = (size / BLOCK_SIZE_BYTES);
    if (size % BLOCK_SIZE_BYTES > 0) {
        numChunks += 1;
    }
    U64 timeout = numChunks * TIMEOUT_MS;

    // Write loop
    FwSizeType bytesWritten = 0;
    Fw::Time start = this->getTime();
    for (U32 i = 0; i < MAX_LOOP_ITERATIONS; i++) {
        FwSizeType writeAmt = FW_MIN(size - bytesWritten, BLOCK_SIZE_BYTES);
        Os::File::Status ret = file.write(data + bytesWritten, writeAmt);

        if (Os::File::OP_OK != ret || writeAmt == 0) {
            Fw::LogStringArg logStringArg(fileName);
            this->log_WARNING_HI_WriteFileError(bytesWritten, size, logStringArg, ret);
            break;
        }

        bool currAbort = this->m_abort.load(std::memory_order_relaxed);
        if (currAbort) {
            // Abort command was sent
            Fw::LogStringArg logStringArg(fileName);
            this->log_WARNING_LO_WriteAborted(bytesWritten, size, logStringArg);
            break;
        }

        if (timeout > 0) {
            // Only check timeout if > 0
            Fw::Time now = this->getTime();
            Fw::Time diff = Fw::Time::sub(now, start);
            U64 elapsed = (diff.getSeconds() * 1000000) + diff.getUSeconds();

            if (elapsed >= timeout) {
                Fw::LogStringArg logStringArg(fileName);
                this->log_WARNING_HI_WriteTimeout(bytesWritten, size, logStringArg, timeout);
                break;
            }
        }

        bytesWritten += writeAmt;
        if (bytesWritten >= size) {
            // Finished, break out
            break;
        }
    }

    return bytesWritten;
}

}  // namespace Svc
