// ======================================================================
// \title  FileWorkerTester.cpp
// \author racheljt
// \brief  cpp file for FileWorker component test harness implementation class
// ======================================================================

#include "FileWorkerErrTester.hpp"

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

void FileWorkerTester ::testReadErr() {
    Os::File f;
    Os::File::Status fsStat;
    FileWorkerStatus wStat;
    FileWorkerReadStatus rStat;
    FwSizeType readBytes = 0;
    const char* fnameChar = "nominalread.bin";
    Fw::String fname = fnameChar;
    FwSizeType size = 1024 * 100;
    U8 data[size];
    Fw::Buffer buf(data, size);

    fTest.setOpen(Os::FileInterface::OP_OK);
    fTest.setSize(Os::FileInterface::OP_OK, 8247);
    fTest.setRead(Os::FileInterface::OP_OK);
    fTest.setWrite(Os::FileInterface::OP_OK);

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::NOT_OPENED);
    rStat = this->component.readFileBytes(buf, size, f, readBytes);
    ASSERT_EQ(FileWorkerReadStatus::FW_READ_ERROR, rStat);
    fTest.setOpen(Os::FileInterface::OP_OK);

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::NOT_OPENED);
    wStat = this->component.readBufferFromFile(buf, fnameChar);
    ASSERT_EQ(FileWorkerStatus::FW_STATUS_FAILED_TO_OPEN, wStat);
    ASSERT_EVENTS_OpenFileError_SIZE(1);
    fTest.setOpen(Os::FileInterface::OP_OK);

    fsStat = f.open(fnameChar, Os::File::OPEN_READ);
    FW_ASSERT(fsStat == Os::File::OP_OK);

    this->clearHistory();
    fTest.setRead(Os::FileInterface::INVALID_MODE);
    this->component.readFile(buf, size, f, fnameChar);
    ASSERT_EVENTS_ReadError_SIZE(1);
    fTest.setRead(Os::FileInterface::OP_OK);

    this->clearHistory();
    this->component.m_abort.store(true);
    this->component.readFile(buf, size, f, fnameChar);
    ASSERT_EVENTS_ReadAborted_SIZE(1);
    this->component.m_abort.store(false);

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::DOESNT_EXIST);
    this->invoke_to_readIn(0, fname, buf);
    this->component.doDispatch();
    fTest.setOpen(Os::FileInterface::OP_OK);

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::INVALID_MODE);
    this->invoke_to_readIn(0, fname, buf);
    this->component.doDispatch();
    ASSERT_from_readDoneOut_SIZE(1);
    ASSERT_from_readDoneOut(0, FileWorkerStatus::FW_STATUS_FAILED_CRC, 0);
    fTest.setOpen(Os::FileInterface::OP_OK);
}

void FileWorkerTester ::testVerifyErr() {
    const char* fnameChar = "testfile.txt";
    Fw::String fname = fnameChar;

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::INVALID_MODE);
    this->invoke_to_verifyIn(0, fname, 0);
    this->component.doDispatch();
    ASSERT_from_verifyDoneOut_SIZE(1);
    ASSERT_from_verifyDoneOut(0, FileWorkerStatus::FW_STATUS_FAILED_FILE_SIZE, 0);
    fTest.setOpen(Os::FileInterface::OP_OK);
}

void FileWorkerTester ::testWriteErr() {
    FwSizeType maxSize = 10000;
    FwSizeType dataSize = 1024;
    U8 data[maxSize];
    U8* dataPtr = data;
    FwSizeType amt = 0;
    Fw::Buffer buf(data, dataSize);

    for (FwSizeType i = 0; i < dataSize && i < maxSize; i++) {
        data[i] = static_cast<U8>(i % 256);
    }
    for (FwSizeType i = dataSize; i < maxSize; i += dataSize) {
        amt = FW_MIN(dataSize, maxSize - i);
        (void)memcpy(dataPtr + i, data, amt);
    }

    Os::File f;
    Os::File::Status fStat;
    const char* fnameChar = "testwrite.txt";
    Fw::String fname = fnameChar;
    FwSizeType writeBytes = 0;

    fTest.setOpen(Os::FileInterface::OP_OK);
    fTest.setRead(Os::FileInterface::OP_OK);
    fTest.setWrite(Os::FileInterface::OP_OK);
    fTest.setSize(Os::FileInterface::OP_OK, 8247);

    fStat = f.open(fnameChar, Os::File::OPEN_READ);
    ASSERT_EQ(fStat, Os::File::OP_OK);

    this->clearHistory();
    fTest.setSize(Os::FileInterface::OP_OK, 20 << 20);
    this->component.m_abort.store(true);
    writeBytes = this->component.writeToFile(data, dataSize, f, fnameChar);
    ASSERT_EQ(0, writeBytes);
    ASSERT_EVENTS_WriteFileError_SIZE(1);
    this->component.m_abort.store(false);
    fTest.setSize(Os::FileInterface::OP_OK, 20);

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::INVALID_MODE);
    writeBytes = this->component.writeBufferToFile(buf, fnameChar, 0, false);
    ASSERT_EQ(0, writeBytes);
    ASSERT_EVENTS_OpenFileError_SIZE(1);
    fTest.setOpen(Os::FileInterface::OP_OK);
}

void FileWorkerTester ::testWriteHashErr() {
    FwSizeType offsetBytes = 0;
    const char* fnameChar = "testfile.txt";
    FwSizeType size = 1024 * 100;
    U8 data[size];
    Fw::Buffer buf(data, size);

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::OP_OK);
    this->component.writeBufferHashToFile(buf, fnameChar, offsetBytes, false);

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::INVALID_MODE);
    this->component.writeBufferHashToFile(buf, fnameChar, offsetBytes, false);
    ASSERT_EVENTS_OpenFileError_SIZE(1);
    fTest.setOpen(Os::FileInterface::OP_OK);

    // Construct hash filename
    const char* ext = Utils::Hash::getFileExtensionString();
    FW_ASSERT(ext != nullptr);
    char hashFileName[128];
    Fw::FormatStatus status = Fw::stringFormat(hashFileName, sizeof(hashFileName), "%s%s", fnameChar, ext);
    FW_ASSERT(status == Fw::FormatStatus::SUCCESS);

    // Compute hash
    Utils::HashBuffer hashBuffer;
    FwSizeType hashSize = buf.getSize();
    U8* const hashData = reinterpret_cast<U8*>(buf.getData());
    FW_ASSERT(hashData != nullptr);

    // Apply offset
    U8* const dataFromOffset = reinterpret_cast<U8*>(hashData);
    FW_ASSERT(dataFromOffset != nullptr);

    Utils::Hash hash;

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::INVALID_MODE);
    this->component.getHash(hashFileName, hash, hashBuffer, dataFromOffset, hashSize);
    ASSERT_EVENTS_WriteValidationOpenError_SIZE(1);
    fTest.setOpen(Os::FileInterface::OP_OK);

    this->clearHistory();
    fTest.setOpen(Os::FileInterface::DOESNT_EXIST);
    this->component.getHash(hashFileName, hash, hashBuffer, dataFromOffset, hashSize);
    fTest.setOpen(Os::FileInterface::OP_OK);

    this->clearHistory();
    fTest.setRead(Os::FileInterface::INVALID_MODE);
    this->component.getHash(hashFileName, hash, hashBuffer, dataFromOffset, hashSize);
    fTest.setRead(Os::FileInterface::OP_OK);
}

// ----------------------------------------------------------------------
// Input-validation tests
//
// These exercise the runtime checks that replaced FW_ASSERTs on the
// readIn/verifyIn/writeIn port handlers. Each malformed input should emit
// a single InvalidInput warning event, return FW_STATUS_INVALID_INPUT on the
// corresponding done port, and leave the component in IDLE. None of these
// paths touch the filesystem, so the FileTester mock state is irrelevant.
// ----------------------------------------------------------------------

void FileWorkerTester ::testReadInvalidInput() {
    FwSizeType size = 1024;
    U8 data[size];
    Fw::Buffer validBuf(data, size);

    // Empty path
    this->clearHistory();
    Fw::String emptyPath("");
    this->invoke_to_readIn(0, emptyPath, validBuf);
    this->component.doDispatch();
    ASSERT_EVENTS_InvalidInput_SIZE(1);
    ASSERT_EVENTS_InvalidInput(0, "readIn", "empty path");
    ASSERT_from_readDoneOut_SIZE(1);
    ASSERT_from_readDoneOut(0, FileWorkerStatus::FW_STATUS_INVALID_INPUT, 0);
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);

    // Invalid (null/zero-size) buffer with an otherwise valid path
    this->clearHistory();
    Fw::String validPath("nominalread.bin");
    Fw::Buffer invalidBuf;  // default construction: null data, zero size -> !isValid()
    this->invoke_to_readIn(0, validPath, invalidBuf);
    this->component.doDispatch();
    ASSERT_EVENTS_InvalidInput_SIZE(1);
    ASSERT_EVENTS_InvalidInput(0, "readIn", "invalid buffer");
    ASSERT_from_readDoneOut_SIZE(1);
    ASSERT_from_readDoneOut(0, FileWorkerStatus::FW_STATUS_INVALID_INPUT, 0);
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);
}

void FileWorkerTester ::testVerifyInvalidInput() {
    // Empty path
    this->clearHistory();
    Fw::String emptyPath("");
    this->invoke_to_verifyIn(0, emptyPath, 0);
    this->component.doDispatch();
    ASSERT_EVENTS_InvalidInput_SIZE(1);
    ASSERT_EVENTS_InvalidInput(0, "verifyIn", "empty path");
    ASSERT_from_verifyDoneOut_SIZE(1);
    ASSERT_from_verifyDoneOut(0, FileWorkerStatus::FW_STATUS_INVALID_INPUT, 0);
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);
}

void FileWorkerTester ::testWriteInvalidInput() {
    FwSizeType size = 1024;
    U8 data[size];
    Fw::Buffer validBuf(data, size);
    Fw::String validPath("testwrite.txt");

    // Empty path
    this->clearHistory();
    Fw::String emptyPath("");
    this->invoke_to_writeIn(0, emptyPath, validBuf, 0, false);
    this->component.doDispatch();
    ASSERT_EVENTS_InvalidInput_SIZE(1);
    ASSERT_EVENTS_InvalidInput(0, "writeIn", "empty path");
    ASSERT_from_writeDoneOut_SIZE(1);
    ASSERT_from_writeDoneOut(0, FileWorkerStatus::FW_STATUS_INVALID_INPUT, 0);
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);

    // Invalid (null/zero-size) buffer
    this->clearHistory();
    Fw::Buffer invalidBuf;  // default construction -> !isValid()
    this->invoke_to_writeIn(0, validPath, invalidBuf, 0, false);
    this->component.doDispatch();
    ASSERT_EVENTS_InvalidInput_SIZE(1);
    ASSERT_EVENTS_InvalidInput(0, "writeIn", "invalid buffer");
    ASSERT_from_writeDoneOut_SIZE(1);
    ASSERT_from_writeDoneOut(0, FileWorkerStatus::FW_STATUS_INVALID_INPUT, 0);
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);

    // Offset past the end of the buffer
    this->clearHistory();
    FwSizeType badOffset = size + 1024;
    this->invoke_to_writeIn(0, validPath, validBuf, badOffset, false);
    this->component.doDispatch();
    ASSERT_EVENTS_InvalidInput_SIZE(1);
    ASSERT_EVENTS_InvalidInput(0, "writeIn", "invalid offset");
    ASSERT_from_writeDoneOut_SIZE(1);
    ASSERT_from_writeDoneOut(0, FileWorkerStatus::FW_STATUS_INVALID_INPUT, 0);
    ASSERT_EQ(this->component.m_state, FileWorkerState::FW_STATE_IDLE);
}

}  // namespace Svc
