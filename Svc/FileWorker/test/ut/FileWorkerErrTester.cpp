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

}  // namespace Svc
