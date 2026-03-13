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
      m_lock{},
      m_abort(false),
      m_chunkSize(0),
      m_append(false) {}

void FileWorker ::configure(U64 chunkSize, bool append) {
    FW_ASSERT(chunkSize > 0);
    this->m_chunkSize = chunkSize;
    this->m_append = append;
}

FileWorker ::~FileWorker() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

I8 FileWorker ::cancelIn_handler(FwIndexType portNum) {
    // Cancel for reading
    this->m_lock.lock();
    this->m_abort = true;
    this->m_lock.unLock();
    return DONE;
}

void FileWorker ::readIn_handler(FwIndexType portNum, const Fw::StringBase& path, Fw::Buffer& buffer) {
    const char* const fileNamePtr = path.toChar();
    U32 crcFromFile;
    U32 crcCalculated;
    U64 fileSize;
    Os::FileSystem::Status fsStat;
    Utils::crc_stat_t crcStat;
    FileWorkerStatus workerStat;

    FW_ASSERT(fileNamePtr != nullptr);

    if (this->m_state != FW_STATE_IDLE) {
        this->log_WARNING_HI_NotInIdle(this->m_state);
        this->readDoneOut_out(0, FW_STATUS_NOT_IDLE, 0);
        return;
    }

    // New read request overrides any leftover abort state
    this->m_lock.lock();
    this->m_abort = false;
    this->m_lock.unLock();

    this->m_state = FW_STATE_READING;

    // Check CRC
    crcStat = Utils::verify_checksum(fileNamePtr, crcFromFile, crcCalculated);

    switch (crcStat) {
        case Utils::PASSED_FILE_CRC_CHECK:
            break;

        case Utils::FAILED_FILE_SIZE:       // Fallthrough
        case Utils::FAILED_FILE_SIZE_CAST:  // Fallthrough
        case Utils::FAILED_FILE_OPEN:       // Fallthrough
        case Utils::FAILED_FILE_READ:       // Fallthrough
        case Utils::FAILED_FILE_CRC_OPEN:   // Fallthrough
        case Utils::FAILED_FILE_CRC_READ:   // Fallthrough
        case Utils::FAILED_FILE_CRC_CHECK:
            this->log_WARNING_HI_CrcFailed(crcStat);
            this->readDoneOut_out(0, FW_STATUS_FAILED_CRC, 0);
            this->m_state = FW_STATE_IDLE;
            return;

        default:
            FW_ASSERT(0);
            break;
    }

    // Get filesize
    fsStat = Os::FileSystem::getFileSize(fileNamePtr, fileSize);
    FW_ASSERT(fsStat == Os::FileSystem::OP_OK, fsStat);  // file size checked with checksum

    // Start reading
    workerStat = this->readBufferFromFile(buffer, fileNamePtr);

    // Signal done and pass U8* buffer with data
    this->readDoneOut_out(0, workerStat, 0);
    this->m_state = FW_STATE_IDLE;
}

void FileWorker ::verifyIn_handler(FwIndexType portNum, const Fw::StringBase& path, U32 crc) {
    const char* const fileNamePtr = path.toChar();
    Utils::crc_stat_t crcStat;
    U32 crcFromFile;
    U32 crcCalculated;
    Os::FileSystem::Status fsStat;
    FwSizeType fileSize;
    FileWorkerStatus workerStat = FW_STATUS_DONE;

    FW_ASSERT(fileNamePtr != nullptr);

    // Get checksum
    crcStat = Utils::verify_checksum(fileNamePtr, crcFromFile, crcCalculated);

    switch (crcStat) {
        case Utils::PASSED_FILE_CRC_CHECK:
            break;

        case Utils::FAILED_FILE_SIZE:       // Fallthrough
        case Utils::FAILED_FILE_SIZE_CAST:  // Fallthrough
        case Utils::FAILED_FILE_OPEN:       // Fallthrough
        case Utils::FAILED_FILE_READ:       // Fallthrough
        case Utils::FAILED_FILE_CRC_OPEN:   // Fallthrough
        case Utils::FAILED_FILE_CRC_READ:   // Fallthrough
        case Utils::FAILED_FILE_CRC_CHECK:
            workerStat = FW_STATUS_FAILED_CRC;
            this->log_WARNING_HI_CrcFailed(crcStat);
            break;

        default:
            FW_ASSERT(0);
            break;
    }

    if (crc != crcFromFile) {
        workerStat = FW_STATUS_FAILED_CRC;
        this->log_WARNING_LO_CrcVerificationError(crc, crcCalculated);
    }

    // Get filesize
    fsStat = Os::FileSystem::getFileSize(fileNamePtr, fileSize);
    if (fsStat != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_ReadFailedFileSize(fsStat);
        workerStat = FW_STATUS_FAILED_FILE_SIZE;
    }

    this->verifyDoneOut_out(0, workerStat, static_cast<U32>(fileSize));
}

void FileWorker ::writeIn_handler(FwIndexType portNum,
                                  const Fw::StringBase& path,
                                  Fw::Buffer& buffer,
                                  U64 offsetBytes) {
    this->m_lock.lock();
    this->m_abort = true;
    this->m_lock.unLock();

    char fileName[Svc::MAX_STRING_BYTES];
    U64 length;

    FW_ASSERT(buffer.getData() != nullptr);

    // Make sure we are in IDLE state before proceeding
    if (this->m_state != FW_STATE_IDLE) {
        this->log_WARNING_HI_NotInIdle(this->m_state);
        this->writeDoneOut_out(0, FW_STATUS_NOT_IDLE, 0);
        return;
    }

    this->m_state = FW_STATE_WRITING;

    // New write request overrides any leftover abort state
    this->m_lock.lock();
    this->m_abort = false;
    this->m_lock.unLock();

    // Save filename
    // NB: may count null terminator due to FPRIME/fprime-sw#57, but should still be less than MAX_STRING_BYTES in any
    // case
    length = static_cast<U32>(strnlen(path.toChar(), Svc::MAX_STRING_BYTES));
    FW_ASSERT(length < Svc::MAX_STRING_BYTES && length < sizeof(fileName), static_cast<FwAssertArgType>(length));

    (void)strncpy(fileName, path.toChar(), sizeof(fileName));
    fileName[sizeof(fileName) - 1] = 0;  // guarantee termination

    // Write
    if (this->writeBufferToFile(buffer, fileName, offsetBytes)) {
        // Method will emit warning, it doesn't matter now whether hash file succeeded or not
        (void)this->writeBufferHashToFile(buffer, fileName, offsetBytes);
    }

    this->writeDoneOut_out(0, FW_STATUS_DONE_WRITE, static_cast<U32>(buffer.getSize()));
    this->m_state = FW_STATE_IDLE;
    return;
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

Svc ::FileWorkerStatus FileWorker ::readBufferFromFile(Fw::Buffer& buffer, const char* const fileName) {
    Fw::LogStringArg fileNameStr(fileName);
    Os::File::Status fileStat;
    Os::File f;
    U64 readSize;

    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(fileName != nullptr);

    // Open file
    fileStat = f.open(fileName, Os::File::OPEN_READ);
    if (fileStat != Os::File::OP_OK) {
        this->log_WARNING_HI_OpenFileError(fileNameStr, fileStat);
        return FW_STATUS_FAILED_TO_OPEN;
    }

    // Get buffer data and size
    readSize = static_cast<U32>(buffer.getSize());

    // Read file
    this->log_ACTIVITY_LO_ReadBegin(readSize, fileNameStr);
    this->readFile(buffer, readSize, f, fileNameStr);

    this->log_ACTIVITY_LO_ReadCompleted(readSize, fileNameStr);
    f.close();

    return FW_STATUS_DONE_READ;
}

void FileWorker ::readFile(Fw::Buffer& buffer, U64 length, Os::File& file, Fw::LogStringArg fileNameStr) {
    U64 bytesRead = 0;
    U64 numChunks = 0;
    U64 timeout = 0;
    FileWorkerReadStatus readStat;

    FW_ASSERT(buffer.getData() != nullptr);

    if (!file.isOpen()) {
        return;
    }

    readStat = this->readFileBytes(buffer, length, file, bytesRead);

    switch (readStat) {
        case FW_READ_ERROR:
            // Some read error
            this->log_WARNING_HI_ReadError(bytesRead, length, fileNameStr);
            break;

        case FW_READ_DONE:
            break;

        case FW_READ_ABORT:
            // Abort command was sent
            this->log_WARNING_LO_ReadAborted(bytesRead, length, fileNameStr);
            break;

        case FW_READ_TIMEOUT:
            // Determine true timeout
            static_assert(BLOCK_SIZE_BYTES > 0, "Divide by 0 error");
            numChunks = (length / BLOCK_SIZE_BYTES);
            if (length % BLOCK_SIZE_BYTES > 0) {
                numChunks += 1;
            }
            timeout = static_cast<U64>(numChunks) * TIMEOUT_MS;
            this->log_WARNING_HI_ReadTimeout(bytesRead, length, fileNameStr, timeout);
            break;

        default:
            FW_ASSERT(0);  // Should not get here
            break;
    }

    return;
}

Svc ::FileWorkerReadStatus FileWorker ::readFileBytes(Fw::Buffer& buffer, U64 length, Os::File& file, U64& bytesRead) {
    FW_ASSERT(buffer.getData() != nullptr);

    U64 numChunks;
    U64 timeout;
    Fw::Time start;
    FwSizeType readAmt;
    FwSizeType readAmtActual;
    Os::File::Status ret;
    Fw::Time now;
    Fw::Time diff;
    U64 elapsed;

    // Determine true timeout
    static_assert(BLOCK_SIZE_BYTES > 0, "Divide by 0 error");
    numChunks = (length / BLOCK_SIZE_BYTES);
    if (length % BLOCK_SIZE_BYTES > 0) {
        numChunks += 1;
    }
    timeout = static_cast<U64>(numChunks) * TIMEOUT_MS;

    // Read loop
    bytesRead = 0;
    start = this->getTime();

    for (U32 counter = 0; counter < MAX_LOOP_ITERATIONS; counter++) {
        readAmt = FW_MIN(length - bytesRead, BLOCK_SIZE_BYTES);
        readAmtActual = readAmt;
        ret = file.read(buffer.getData() + bytesRead, readAmtActual);

        if (Os::File::OP_OK != ret || readAmt != readAmtActual) {
            return FW_READ_ERROR;
        }

        this->m_lock.lock();
        bool currAbort = this->m_abort;
        this->m_lock.unLock();

        if (currAbort) {
            // Abort command was sent
            return FW_READ_ABORT;
        }

        if (timeout > 0) {
            // Only check timeout if > 0
            now = this->getTime();
            diff = Fw::Time::sub(now, start);
            elapsed = diff.getSeconds() * static_cast<U64>(1000000) + diff.getUSeconds();
            if (elapsed >= timeout) {
                return FW_READ_TIMEOUT;
            }
        }

        bytesRead += static_cast<U32>(readAmt);
        if (bytesRead >= length) {
            // Finished, break out
            return FW_READ_DONE;
        }
    }

    return FW_READ_UNKNOWN;
}

bool FileWorker ::getHash(const char* const hashFileName,
                          Utils::Hash& hash,
                          Utils::HashBuffer& hashBuffer,
                          const U8* const data,
                          const U64 size) {
    FW_ASSERT(hashFileName);
    FW_ASSERT(data);

    // Open file
    Os::File file;
    Os::File::Status stat = file.open(hashFileName, Os::File::OPEN_READ);

    // Read value if it exists
    if (stat == Os::File::OP_OK) {
        HASH_HANDLE_TYPE hashValue;
        FwSizeType hashSize = sizeof(hashValue);
        U8* hashValuePtr = reinterpret_cast<U8*>(&hashValue);
        FW_ASSERT(hashValuePtr);

        Os::File::Status readStat = file.read(hashValuePtr, hashSize);
        if (readStat != Os::File::OP_OK) {
            Fw::LogStringArg s(hashFileName);
            this->log_WARNING_HI_WriteValidationReadError(s, readStat);
            return false;
        }
        Utils::HashBuffer tmp(hashValuePtr, hashSize);
        hash.setHashValue(tmp);
        hash.update(data, size);
        hash.final(hashBuffer);

    } else if (stat == Os::File::DOESNT_EXIST) {
        hash.hash(data, size, hashBuffer);

    } else {
        Fw::LogStringArg s(hashFileName);
        this->log_WARNING_HI_WriteValidationOpenError(s, stat);
        return false;
    }

    return true;
}

bool FileWorker ::writeBufferToFile(Fw::Buffer& buffer, const char* fileName, U64 offset) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(fileName != nullptr);
    Fw::LogStringArg logStringArg(fileName);

    // Open file
    Os::File file;
    Os::File::Status stat = Os::File::OP_OK;

    if (!this->m_append) {
        stat = file.open(fileName, Os::File::Mode::OPEN_WRITE);
    } else {
        stat = file.open(fileName, Os::File::Mode::OPEN_APPEND);
    }

    if (stat != Os::File::OP_OK) {
        this->log_WARNING_HI_OpenFileError(logStringArg, stat);
        return false;
    }

    // Get buffer data and size
    U64 size = static_cast<U64>(buffer.getSize());
    U8* const data = reinterpret_cast<U8*>(buffer.getData());
    FW_ASSERT(data != nullptr);

    // Apply offset
    FW_ASSERT(offset <= size);
    size -= offset;
    U8* const dataFromOffset = reinterpret_cast<U8*>(data + offset);
    FW_ASSERT(dataFromOffset != nullptr);

    // Write file
    this->log_ACTIVITY_LO_WriteBegin(size, logStringArg);
    U32 writtenSize = this->writeToFile(dataFromOffset, size, file, fileName);

    // Check written size
    if (writtenSize != size) {
        return false;
    }

    this->log_ACTIVITY_LO_WriteCompleted(size, logStringArg);
    return true;
}

bool FileWorker ::writeBufferHashToFile(Fw::Buffer& buffer, const char* fileName, U64 offset) {
    FW_ASSERT(buffer.getData() != nullptr);
    FW_ASSERT(fileName != nullptr);

    // Construct hash filename
    const char* ext = Utils::Hash::getFileExtensionString();
    FW_ASSERT(ext != nullptr);
    char hashFileName[Svc::MAX_STRING_BYTES];
    U32 bytesCopied = static_cast<U32>(snprintf(hashFileName, sizeof(hashFileName), "%s%s", fileName, ext));
    FW_ASSERT(bytesCopied < sizeof(hashFileName));

    // Compute hash
    Utils::HashBuffer hashBuffer;
    U64 size = static_cast<U32>(buffer.getSize());
    U8* const data = reinterpret_cast<U8*>(buffer.getData());
    FW_ASSERT(data != nullptr);

    // Apply offset
    FW_ASSERT(offset <= size);
    size -= offset;
    U8* const dataFromOffset = reinterpret_cast<U8*>(data + offset);
    FW_ASSERT(dataFromOffset != nullptr);

    Utils::Hash hash;

    if (!this->m_append) {
        hash.hash(dataFromOffset, size, hashBuffer);

    } else {
        if (!this->getHash(hashFileName, hash, hashBuffer, dataFromOffset, size)) {
            return false;
        }
    }

    // Open file
    Os::File file;
    Os::File::Status stat = file.open(hashFileName, Os::File::Mode::OPEN_WRITE);
    if (stat != Os::File::OP_OK) {
        Fw::LogStringArg logStringArg(hashFileName);
        this->log_WARNING_HI_OpenFileError(logStringArg, stat);
        return false;
    }

    // Write hash
    U32 writtenSize =
        this->writeToFile(hashBuffer.getBuffAddr(), static_cast<U32>(hashBuffer.getSize()), file, hashFileName);

    // Check written size
    U32 hashSize = static_cast<U32>(hashBuffer.getSize());
    if (writtenSize != hashSize) {
        Fw::LogStringArg logStringArg(hashFileName);
        this->log_WARNING_LO_WriteValidationError(logStringArg, writtenSize, hashSize);
        return false;
    }
    return true;
}

U32 FileWorker ::writeToFile(const U8* data, U64 length, Os::File& file, const char* fileName) {
    FW_ASSERT(data != nullptr);
    FW_ASSERT(file.isOpen());

    // Determine true timeout
    static_assert(BLOCK_SIZE_BYTES > 0, "Divide by 0 error");
    U64 numChunks = (length / BLOCK_SIZE_BYTES);
    if (length % BLOCK_SIZE_BYTES > 0) {
        numChunks += 1;
    }
    U64 timeout = static_cast<U64>(numChunks) * TIMEOUT_MS;

    // Write loop
    U32 bytesWritten = 0;
    Fw::Time start = this->getTime();
    for (U32 counter = 0; counter < MAX_LOOP_ITERATIONS; counter++) {
        FwSizeType writeAmt = FW_MIN(length - bytesWritten, BLOCK_SIZE_BYTES);
        Os::File::Status ret = file.write(data + bytesWritten, writeAmt);

        if (Os::File::OP_OK != ret || writeAmt == 0) {
            Fw::LogStringArg logStringArg(fileName);
            this->log_WARNING_HI_WriteFileError(bytesWritten, length, logStringArg, ret);
            break;
        }

        this->m_lock.lock();
        bool currAbort = this->m_abort;
        this->m_lock.unLock();

        if (currAbort) {
            // Abort command was sent
            Fw::LogStringArg logStringArg(fileName);
            this->log_WARNING_LO_WriteAborted(bytesWritten, length, logStringArg);
            break;
        }

        if (timeout > 0) {
            // Only check timeout if > 0
            Fw::Time now = this->getTime();
            Fw::Time diff = Fw::Time::sub(now, start);
            U64 elapsed = diff.getSeconds() * static_cast<U64>(1000000) + diff.getUSeconds();

            if (elapsed >= timeout) {
                Fw::LogStringArg logStringArg(fileName);
                this->log_WARNING_HI_WriteTimeout(bytesWritten, length, logStringArg, timeout);
                break;
            }
        }

        bytesWritten += static_cast<U32>(writeAmt);
        if (bytesWritten >= length) {
            // Finished, break out
            break;
        }
    }

    return bytesWritten;
}

}  // namespace Svc
