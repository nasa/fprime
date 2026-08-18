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
    : FileWorkerComponentBase(compName),
      m_state(FileWorkerState::FW_STATE_IDLE),
      m_abort(false),
      m_chunkSize(BLOCK_SIZE_BYTES) {}

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
    // Validate inputs before processing file
    if (path.length() == 0) {
        this->log_WARNING_HI_InvalidInput(Fw::LogStringArg("readIn"), Fw::LogStringArg("empty path"));
        this->readDoneOut_out(0, FW_STATUS_INVALID_INPUT, 0);
        return;
    }
    if (!buffer.isValid()) {
        this->log_WARNING_HI_InvalidInput(Fw::LogStringArg("readIn"), Fw::LogStringArg("invalid buffer"));
        this->readDoneOut_out(0, FW_STATUS_INVALID_INPUT, 0);
        return;
    }

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
    if (fsStat != Os::FileSystem::OP_OK) {
        // Path is ground-controlled and the file may change between the CRC check and here
        this->log_WARNING_HI_ReadFailedFileSize(fsStat);
        this->readDoneOut_out(0, FW_STATUS_FAILED_FILE_SIZE, 0);
        this->m_state = FW_STATE_IDLE;
        return;
    }

    // Start reading
    FileWorkerStatus workerStat = this->readBufferFromFile(buffer, fileName);

    // Report 0 bytes on a failed or aborted read so readDoneOut does not imply success.
    this->readDoneOut_out(0, workerStat, (workerStat == FW_STATUS_DONE_READ) ? buffer.getSize() : 0);
    this->m_state = FW_STATE_IDLE;
}

void FileWorker ::verifyIn_handler(FwIndexType portNum, const Fw::StringBase& path, U32 crc) {
    // Validate inputs before processing file
    if (path.length() == 0) {
        this->log_WARNING_HI_InvalidInput(Fw::LogStringArg("verifyIn"), Fw::LogStringArg("empty path"));
        this->verifyDoneOut_out(0, FW_STATUS_INVALID_INPUT, 0);
        return;
    }

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

    if (crc != crcCalculated) {
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
    // Validate inputs before processing file
    if (path.length() == 0) {
        this->log_WARNING_HI_InvalidInput(Fw::LogStringArg("writeIn"), Fw::LogStringArg("empty path"));
        this->writeDoneOut_out(0, FW_STATUS_INVALID_INPUT, 0);
        return;
    }
    if (!buffer.isValid()) {
        this->log_WARNING_HI_InvalidInput(Fw::LogStringArg("writeIn"), Fw::LogStringArg("invalid buffer"));
        this->writeDoneOut_out(0, FW_STATUS_INVALID_INPUT, 0);
        return;
    }
    if (offsetBytes > buffer.getSize()) {
        this->log_WARNING_HI_InvalidInput(Fw::LogStringArg("writeIn"), Fw::LogStringArg("invalid offset"));
        this->writeDoneOut_out(0, FW_STATUS_INVALID_INPUT, 0);
        return;
    }

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
    if (length >= FileNameStringSize || length >= sizeof(fileName)) {
        // Path length is ground-controlled, so an oversized path is invalid input, not a coding error.
        this->log_WARNING_HI_InvalidInput(Fw::LogStringArg("writeIn"), Fw::LogStringArg("path too long"));
        this->writeDoneOut_out(0, FW_STATUS_INVALID_INPUT, 0);
        this->m_state = FW_STATE_IDLE;
        return;
    }

    (void)Fw::StringUtils::string_copy(fileName, path.toChar(), sizeof(fileName));
    fileName[sizeof(fileName) - 1] = 0;  // guarantee termination

    // Write
    const bool isWrite = this->writeBufferToFile(buffer, fileName, offsetBytes, append);
    if (isWrite) {
        this->writeBufferHashToFile(buffer, fileName, offsetBytes, append);
    }

    // Report the actual outcome of the write. A failed writeBufferToFile (open
    // failure, permission denied, disk full, partial write) must not be reported
    // to ground as a successful FW_STATUS_DONE_WRITE.
    const FileWorkerStatus writeStatus = isWrite ? FW_STATUS_DONE_WRITE : FW_STATUS_FAILED_TO_WRITE;
    // Report bytes actually written, which excludes the skipped offset. Reporting the full
    // buffer size over-reports by offsetBytes, and reports a whole buffer for a zero-length
    // write. offsetBytes <= buffer.getSize() is checked above, so this cannot underflow.
    const FwSizeType writtenBytes = buffer.getSize() - offsetBytes;
    this->writeDoneOut_out(0, writeStatus, isWrite ? writtenBytes : 0);
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
    const FileWorkerReadStatus readStat = this->readFile(buffer, readSize, file, fileNameStr);

    this->log_ACTIVITY_LO_ReadCompleted(readSize, fileNameStr);
    file.close();

    // Only a completed read is DONE_READ; error, abort, or timeout reports FAILED_TO_READ.
    return (readStat == FileWorkerReadStatus::FW_READ_DONE) ? FileWorkerStatus::FW_STATUS_DONE_READ
                                                            : FileWorkerStatus::FW_STATUS_FAILED_TO_READ;
}

Svc ::FileWorkerReadStatus FileWorker ::readFile(Fw::Buffer& buffer,
                                                 FwSizeType size,
                                                 Os::File& file,
                                                 const Fw::LogStringArg& fileNameStr) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(size > 0);
    FW_ASSERT(fileNameStr != nullptr);

    FwSizeType bytesRead = 0;
    FwSizeType numChunks = 0;
    U64 timeout = 0;

    if (!file.isOpen()) {
        return FileWorkerReadStatus::FW_READ_ERROR;
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
            FW_ASSERT(this->m_chunkSize > 0);
            numChunks = (size / this->m_chunkSize);
            if (size % this->m_chunkSize > 0) {
                numChunks += 1;
            }
            timeout = numChunks * TIMEOUT_MS;
            this->log_WARNING_HI_ReadTimeout(bytesRead, size, fileNameStr, timeout);
            break;

        case FW_READ_UNKNOWN:
            // The read loop ran out of iterations: a read larger than
            // MAX_LOOP_ITERATIONS * BLOCK_SIZE_BYTES cannot complete
            this->log_WARNING_HI_ReadError(bytesRead, size, fileNameStr);
            break;

        default:
            FW_ASSERT(false, static_cast<FwAssertArgType>(readStat));
            break;
    }

    return readStat;
}

Svc ::FileWorkerReadStatus FileWorker ::readFileBytes(Fw::Buffer& buffer,
                                                      FwSizeType size,
                                                      Os::File& file,
                                                      FwSizeType& bytesRead) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(size > 0);

    // Determine true timeout
    FW_ASSERT(this->m_chunkSize > 0);
    FwSizeType numChunks = (size / this->m_chunkSize);
    if (size % this->m_chunkSize > 0) {
        numChunks += 1;
    }
    U64 timeout = numChunks * TIMEOUT_MS;

    // Read loop
    bytesRead = 0;
    Fw::Time start = this->getTime();

    for (FwSizeType i = 0; i < numChunks; i++) {
        FwSizeType readAmt = FW_MIN(size - bytesRead, this->m_chunkSize);
        FwSizeType readAmtActual = readAmt;
        Os::File::Status ret = file.read(buffer.getData() + bytesRead, readAmtActual);

        if (Os::File::OP_OK != ret || readAmt != readAmtActual) {
            // Count the bytes actually transferred so ReadError telemetry reports
            // the true amount. A short read stays an error on purpose: FileWorker
            // reads a fixed, caller-specified size and must not silently accept a
            // file shorter than expected (e.g. truncated mid-read).
            bytesRead += readAmtActual;
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

    // Get buffer data and size, then apply offset
    FwSizeType size = buffer.getSize();
    U8* const data = reinterpret_cast<U8*>(buffer.getData());
    FW_ASSERT(data != nullptr);
    FW_ASSERT(offset <= size);
    size -= offset;

    // A zero-length write (offset == buffer size, a valid "nothing left to write" boundary
    // permitted by writeIn_handler's offset check) is a successful no-op. Return before
    // opening the file: this avoids reaching FW_ASSERT(size > 0) in writeToFile(), and avoids
    // creating an empty file for a request that writes nothing, since both OPEN_WRITE and
    // OPEN_APPEND pass O_CREAT. An existing file's contents are not at risk either way here:
    // OPEN_WRITE overwrites in place and does not truncate; only OPEN_CREATE sets O_TRUNC.
    if (size == 0) {
        this->log_ACTIVITY_LO_WriteCompleted(size, logStringArg);
        return true;
    }

    U8* const dataFromOffset = reinterpret_cast<U8*>(data + offset);
    FW_ASSERT(dataFromOffset != nullptr);

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

    // A zero-length write changed no file contents, so the hash must not change either. Skip
    // generation entirely: on the append path this would otherwise trip FW_ASSERT(size > 0) in
    // getHash(), and on the non-append path it would silently overwrite a valid hash file with
    // the hash of zero bytes.
    if (size == 0) {
        return;
    }

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
    FW_ASSERT(this->m_chunkSize > 0);
    FwSizeType numChunks = (size / this->m_chunkSize);
    if (size % this->m_chunkSize > 0) {
        numChunks += 1;
    }
    U64 timeout = numChunks * TIMEOUT_MS;

    // Write loop: legal short writes make progress but consume an iteration, so
    // allow extra iterations beyond the chunk count before giving up
    const FwSizeType maxIterations = numChunks + MAX_LOOP_ITERATIONS;
    FwSizeType bytesWritten = 0;
    Fw::Time start = this->getTime();
    for (FwSizeType i = 0; (i < maxIterations) && (bytesWritten < size); i++) {
        FwSizeType writeAmt = FW_MIN(size - bytesWritten, this->m_chunkSize);
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
    }

    return bytesWritten;
}

}  // namespace Svc
