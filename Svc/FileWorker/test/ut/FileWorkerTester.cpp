// ======================================================================
// \title  FileWorkerTester.cpp
// \author racheljt
// \brief  cpp file for FileWorker component test harness implementation class
// ======================================================================

#include "FileWorkerTester.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

FileWorkerTester ::FileWorkerTester()
    : FileWorkerGTestBase("FileWorkerTester", FileWorkerTester::MAX_HISTORY_SIZE), component("FileWorker") {
    this->initComponents();
    this->connectPorts();

    this->component.configure(100);
}

FileWorkerTester ::~FileWorkerTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void FileWorkerTester ::testReadFile() {
    FwSizeType maxDataSize = 4096;
    FwSizeType dataSize = 1024;
    U8 data[maxDataSize];
    Fw::Buffer buf(data, dataSize);
    const char* fnameChar = "nominalread.bin";
    Fw::String fname = fnameChar;

    // Nominal read
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);
    this->invoke_to_readIn(0, fname, buf);
    this->component.doDispatch();
    ASSERT_EVENTS_NotInIdle_SIZE(0);
    ASSERT_EVENTS_CrcFailed_SIZE(0);
    ASSERT_EVENTS_ReadFailedFileSize_SIZE(0);
    ASSERT_from_readDoneOut_SIZE(1);
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);

    // Verify read contents against original file
    Os::File file;
    Os::File::Status stat;
    U8 dataBuf[dataSize];
    stat = file.open(fnameChar, Os::File::OPEN_READ);
    ASSERT_EQ(stat, Os::File::OP_OK);

    for (FwSizeType i = 0; i < dataSize; i += sizeof(buf)) {
        FwSizeType amt = FW_MIN(sizeof(buf), dataSize - i);
        stat = file.read(dataBuf, amt);
        ASSERT_EQ(stat, Os::File::OP_OK);
        I32 cmp = memcmp(data + i, dataBuf, amt);
        ASSERT_EQ(cmp, 0) << "at i=" << i << "\n";
    }

    // Attempt to read when not in IDLE
    this->clearHistory();
    this->component.m_state = FileWorkerState::FW_STATE_WRITING;
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_WRITING);
    this->invoke_to_readIn(0, fname, buf);
    this->component.doDispatch();
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_WRITING);
    ASSERT_from_readDoneOut_SIZE(1);
    ASSERT_EVENTS_NotInIdle(0, FileWorkerState::FW_STATE_WRITING);
    ASSERT_EVENTS_NotInIdle_SIZE(1);

    // CRC failed
    this->clearHistory();
    this->component.m_state = FileWorkerState::FW_STATE_IDLE;
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);
    fname = "nominaldne.bin";
    this->invoke_to_readIn(0, fname, buf);
    this->component.doDispatch();
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);
    ASSERT_from_readDoneOut_SIZE(1);
    ASSERT_EVENTS_CrcFailed_SIZE(1);
}

void FileWorkerTester ::testCancel() {
    Os::File f;
    Os::File::Status fsStat;
    const char* fnameChar = "testfile.txt";
    Fw::String fname = fnameChar;
    FwSizeType readBytes = 0;
    FwSizeType writeBytes = 0;
    FwSizeType maxSize = 20 << 20;
    FwSizeType size = 1024 * 100;
    U8 data[size];
    Fw::Buffer buf(data, size);
    FileWorkerReadStatus rStat;

    fsStat = f.open(fnameChar, Os::File::OPEN_WRITE);
    ASSERT_EQ(fsStat, Os::File::OP_OK);
    fsStat = f.write(data, size);
    ASSERT_EQ(fsStat, Os::File::OP_OK);
    f.close();

    fsStat = f.open(fnameChar, Os::File::OPEN_READ);
    ASSERT_EQ(fsStat, Os::File::OP_OK);
    ASSERT_EQ(false, this->component.m_abort.load());
    this->invoke_to_cancelIn(0);
    ASSERT_EQ(true, this->component.m_abort.load());
    rStat = this->component.readFileBytes(buf, maxSize, f, readBytes);
    ASSERT_EQ(FileWorkerReadStatus::FW_READ_ABORT, rStat);
    this->component.m_abort.store(false);
    f.close();

    fsStat = f.open(fnameChar, Os::File::OPEN_WRITE);
    ASSERT_EQ(fsStat, Os::File::OP_OK);
    ASSERT_EQ(false, this->component.m_abort.load());
    this->invoke_to_cancelIn(0);
    ASSERT_EQ(true, this->component.m_abort.load());
    writeBytes = this->component.writeToFile(data, maxSize, f, fnameChar);
    ASSERT_EQ(0, writeBytes);
    ASSERT_EVENTS_WriteAborted_SIZE(1);
    this->component.m_abort.store(false);
    f.close();
}

void FileWorkerTester ::testFileError() {
    Os::File f;
    const char* fnameChar = "testfile.txt";
    Fw::String fname = fnameChar;
    FwSizeType size = 1024 * 100;
    U8 data[size];
    Fw::Buffer buf(data, size);

    this->component.readFile(buf, size, f, fname);
}

void FileWorkerTester ::testVerify() {
    Fw::String fname;
    U32 exp;

    fname = "nominalread.bin";
    exp = 0x0;
    this->invoke_to_verifyIn(0, fname, exp);
    this->component.doDispatch();
    ASSERT_EVENTS_CrcFailed_SIZE(0);
    ASSERT_EVENTS_CrcVerificationError_SIZE(1);
    ASSERT_from_verifyDoneOut_SIZE(1);
    ASSERT_from_verifyDoneOut(0, FileWorkerStatus::FW_STATUS_FAILED_CRC, 8247);

    this->clearHistory();
    fname = "nominalread.bin";
    exp = 0xBC0905F4;
    this->invoke_to_verifyIn(0, fname, exp);
    this->component.doDispatch();
    ASSERT_EVENTS_CrcFailed_SIZE(0);
    ASSERT_EVENTS_CrcVerificationError_SIZE(0);
    ASSERT_from_verifyDoneOut_SIZE(1);
    ASSERT_from_verifyDoneOut(0, FileWorkerStatus::FW_STATUS_DONE, 8247);

    this->clearHistory();
    fname = "nominaldne.bin";
    exp = 0xBC0905F4;
    this->invoke_to_verifyIn(0, fname, exp);
    this->component.doDispatch();
    ASSERT_EVENTS_CrcFailed_SIZE(1);
    ASSERT_EVENTS_CrcVerificationError_SIZE(1);
    ASSERT_from_verifyDoneOut_SIZE(1);
    ASSERT_from_verifyDoneOut(0, FileWorkerStatus::FW_STATUS_FAILED_FILE_SIZE, 0);
}

void FileWorkerTester ::testTransfer() {
    Fw::String fname;
    FwSizeType offsetBytes = 0;
    U8 data[1024];
    Fw::Buffer buf(data, sizeof(data));

    // Attempt to write when not in IDLE
    fname = "nominalwrite.bin";
    this->component.m_state = FileWorkerState::FW_STATE_WRITING;
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_WRITING);
    this->invoke_to_writeIn(0, fname, buf, offsetBytes, true);
    this->component.doDispatch();
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_WRITING);
    ASSERT_from_writeDoneOut_SIZE(1);
    ASSERT_from_writeDoneOut(0, FileWorkerState::FW_STATE_IDLE, 0);
    ASSERT_EVENTS_NotInIdle_SIZE(1);
    ASSERT_EVENTS_NotInIdle(0, FileWorkerState::FW_STATE_WRITING);
}

void FileWorkerTester ::testWriting() {
    FwSizeType maxSize = 4096;
    FwSizeType dataSize = 1024;
    U8 data[maxSize];
    U8* dataPtr = data;
    FwSizeType amt = 0;

    // Make data
    for (FwSizeType i = 0; i < dataSize && i < maxSize; i++) {
        data[i] = static_cast<U8>(i % 256);
    }
    for (FwSizeType i = dataSize; i < maxSize; i += dataSize) {
        amt = FW_MIN(dataSize, maxSize - i);
        (void)memcpy(dataPtr + i, data, amt);
    }

    // Make buffer
    Fw::Buffer buffer(data, dataSize);
    const char* fnameChar = "testwrite.txt";
    Fw::String fname = fnameChar;
    FwSizeType offsetBytes = 0;

    // Write
    this->invoke_to_writeIn(0, fname, buffer, offsetBytes, true);
    this->component.doDispatch();

    // Verify file contents
    Os::File file;
    Os::File::Status stat;
    U8 buf[maxSize];

    stat = file.open(fnameChar, Os::File::OPEN_READ);
    ASSERT_EQ(stat, Os::File::OP_OK);

    for (FwSizeType i = 0; i < dataSize; i += sizeof(buf)) {
        amt = FW_MIN(sizeof(buf), dataSize - i);
        stat = file.read(buf, amt);
        ASSERT_EQ(stat, Os::File::OP_OK);
        I32 cmp = memcmp(data + i, buf, amt);
        ASSERT_EQ(cmp, 0) << "at i=" << i << "\n";
    }

    // Verify validation file
    ASSERT_EVENTS_WriteValidationError_SIZE(0);
    std::string hashFileNameStr = std::string(fnameChar) + std::string(Utils::Hash::getFileExtensionString());
    const char* hashFileName = hashFileNameStr.c_str();
    Os::ValidateFile::Status validateStat = Os::ValidateFile::validate(fnameChar, hashFileName);
    ASSERT_EQ(validateStat, Os::ValidateFile::VALIDATION_OK);

    // Verify evr
    ASSERT_EVENTS_WriteBegin_SIZE(1);
    ASSERT_EVENTS_WriteCompleted_SIZE(1);
    ASSERT_EVENTS_WriteAborted_SIZE(0);
    ASSERT_EVENTS_OpenFileError_SIZE(0);
    ASSERT_EVENTS_WriteFileError_SIZE(0);
    ASSERT_EVENTS_WriteValidationError_SIZE(0);
    ASSERT_EVENTS_WriteTimeout_SIZE(0);
}

void FileWorkerTester ::testWritingOffset() {
    FwSizeType maxSize = 4096;
    FwSizeType dataSize = 1024;
    U8 data[maxSize];
    U8* dataPtr = data;
    FwSizeType amt = 0;

    // Make data
    for (FwSizeType i = 0; i < dataSize && i < maxSize; i++) {
        data[i] = static_cast<U8>(i % 256);
    }
    for (FwSizeType i = dataSize; i < maxSize; i += dataSize) {
        amt = FW_MIN(dataSize, maxSize - i);
        (void)memcpy(dataPtr + i, data, amt);
    }

    // Make buffer
    Fw::Buffer buffer(data, dataSize);
    const char* fnameChar = "testwriteoffset.txt";
    Fw::String fname = fnameChar;
    FwSizeType offsetBytes = 12;

    // Write
    this->invoke_to_writeIn(0, fname, buffer, offsetBytes, true);
    this->component.doDispatch();

    // Verify file contents
    Os::File file;
    Os::File::Status stat;
    U8 buf[maxSize];

    stat = file.open(fnameChar, Os::File::OPEN_READ);
    ASSERT_EQ(stat, Os::File::OP_OK);

    for (FwSizeType i = offsetBytes; i < dataSize; i += sizeof(buf)) {
        amt = FW_MIN(sizeof(buf), dataSize - i);
        stat = file.read(buf, amt);
        ASSERT_EQ(stat, Os::File::OP_OK);
        I32 cmp = memcmp(data + i, buf, amt);
        ASSERT_EQ(cmp, 0) << "at i=" << i << "\n";
    }

    // Verify validation file
    ASSERT_EVENTS_WriteValidationError_SIZE(0);
    std::string hashFileNameStr = std::string(fnameChar) + std::string(Utils::Hash::getFileExtensionString());
    const char* hashFileName = hashFileNameStr.c_str();
    Os::ValidateFile::Status validateStat = Os::ValidateFile::validate(fnameChar, hashFileName);
    ASSERT_EQ(validateStat, Os::ValidateFile::VALIDATION_OK);

    // Verify evr
    ASSERT_EVENTS_WriteBegin_SIZE(1);
    ASSERT_EVENTS_WriteCompleted_SIZE(1);
    ASSERT_EVENTS_WriteAborted_SIZE(0);
    ASSERT_EVENTS_OpenFileError_SIZE(0);
    ASSERT_EVENTS_WriteFileError_SIZE(0);
    ASSERT_EVENTS_WriteValidationError_SIZE(0);
    ASSERT_EVENTS_WriteTimeout_SIZE(0);
}

void FileWorkerTester ::testAppending() {
    U32 n = 3;  // Number of times to append

    FwSizeType maxSize = 4096;
    FwSizeType dataSize = 1024;
    U8 data[maxSize];
    U8* dataPtr = data;
    FwSizeType amt = 0;

    // Make data
    for (FwSizeType i = 0; i < dataSize && i < maxSize; i++) {
        data[i] = static_cast<U8>(i % 256);
    }
    for (FwSizeType i = dataSize; i < maxSize; i += dataSize) {
        amt = FW_MIN(dataSize, maxSize - i);
        (void)memcpy(dataPtr + i, data, amt);
    }

    // Make buffer
    Fw::Buffer buffer(data, dataSize);
    const char* fnameChar = "testwrite.txt";
    Fw::String fname = fnameChar;
    FwSizeType offsetBytes = 0;

    // Write
    for (U32 i = 0; i < n; i++) {
        this->invoke_to_writeIn(0, fname, buffer, offsetBytes, true);
        this->component.doDispatch();
    }

    // Verify file contents
    Os::File file;
    Os::File::Status stat;
    U8 buf[maxSize];

    stat = file.open(fnameChar, Os::File::OPEN_READ);
    ASSERT_EQ(stat, Os::File::OP_OK);

    for (U32 j = 0; j < n; j++) {
        for (FwSizeType i = 0; i < dataSize; i += sizeof(buf)) {
            amt = FW_MIN(sizeof(buf), dataSize - i);
            stat = file.read(buf, amt);
            ASSERT_EQ(stat, Os::File::OP_OK);
            I32 cmp = memcmp(data + i, buf, amt);
            ASSERT_EQ(cmp, 0) << "at i=" << i << "\n";
        }
    }

    // Verify validation file
    ASSERT_EVENTS_WriteValidationError_SIZE(0);
    std::string hashFileNameStr = std::string(fnameChar) + std::string(Utils::Hash::getFileExtensionString());
    const char* hashFileName = hashFileNameStr.c_str();
    Os::ValidateFile::Status validateStat = Os::ValidateFile::validate(fnameChar, hashFileName);
    ASSERT_EQ(Os::ValidateFile::VALIDATION_OK, validateStat);

    // Verify evr
    ASSERT_EVENTS_WriteBegin_SIZE(n);
    ASSERT_EVENTS_WriteCompleted_SIZE(n);
    ASSERT_EVENTS_WriteAborted_SIZE(0);
    ASSERT_EVENTS_OpenFileError_SIZE(0);
    ASSERT_EVENTS_WriteFileError_SIZE(0);
    ASSERT_EVENTS_WriteValidationError_SIZE(0);
    ASSERT_EVENTS_WriteTimeout_SIZE(0);
}

void FileWorkerTester ::testTimeout() {
    Os::File f;
    Os::File::Status fsStat;
    const char* fnameChar = "nominalread.bin";
    Fw::String fname = fnameChar;
    FwSizeType readBytes = 0;
    FwSizeType maxSize = 4096;
    FwSizeType size = 32;
    U8 data[size];
    Fw::Buffer buf(data, maxSize);
    FileWorkerReadStatus rStat;

    fsStat = f.open(fnameChar, Os::File::OPEN_READ);
    ASSERT_EQ(fsStat, Os::File::OP_OK);
    rStat = this->component.readFileBytes(buf, size, f, readBytes);
    ASSERT_EQ(0, rStat);
    f.close();
}

}  // namespace Svc
