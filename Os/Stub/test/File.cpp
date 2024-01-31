#include "Os/Stub/test/File.hpp"
#include "Os/File.hpp"
#include <new>
namespace Os {
namespace Stub {
namespace File {
namespace Test {

StaticData StaticData::data;

void StaticData::setNextStatus(Os::File::Status status) {
    StaticData::data.nextStatus = status;
}

void StaticData::setSizeResult(FwSignedSizeType size) {
    StaticData::data.sizeResult = size;
}

void StaticData::setPositionResult(FwSignedSizeType position) {
    StaticData::data.positionResult = position;
}

void StaticData::setReadResult(U8 *buffer, FwSignedSizeType size) {
    StaticData::data.readResult = buffer;
    StaticData::data.readResultSize = size;
}

void StaticData::setReadSize(FwSignedSizeType size) {
    StaticData::data.readSizeResult = size;
}

void StaticData::setWriteResult(U8 *buffer, FwSignedSizeType size) {
    StaticData::data.writeResult = buffer;
    StaticData::data.writeResultSize = size;
}

void StaticData::setWriteSize(FwSignedSizeType size) {
    StaticData::data.writeSizeResult = size;
}

TestFile::TestFile() {
    StaticData::data.lastCalled = StaticData::CONSTRUCT_FN;
}

TestFile::~TestFile() {
    StaticData::data.lastCalled = StaticData::DESTRUCT_FN;
}

FileInterface::Status TestFile::open(const char *filepath, Mode open_mode, OverwriteType overwrite) {
    StaticData::data.openPath = filepath;
    StaticData::data.openMode = open_mode;
    StaticData::data.openOverwrite = overwrite;
    StaticData::data.lastCalled = StaticData::OPEN_FN;
    StaticData::data.pointer = 0;
    return StaticData::data.nextStatus;
}

void TestFile::close() {
    StaticData::data.lastCalled = StaticData::CLOSE_FN;
}

FileInterface::Status TestFile::size(FwSignedSizeType& size_result) {
    StaticData::data.lastCalled = StaticData::SIZE_FN;
    size_result = StaticData::data.sizeResult;
    return StaticData::data.nextStatus;
}

FileInterface::Status TestFile::position(FwSignedSizeType& position_result) {
    StaticData::data.lastCalled = StaticData::POSITION_FN;
    position_result = StaticData::data.positionResult;
    return StaticData::data.nextStatus;
}

FileInterface::Status TestFile::preallocate(FwSignedSizeType offset, FwSignedSizeType length) {
    StaticData::data.preallocateOffset = offset;
    StaticData::data.preallocateLength = length;
    StaticData::data.lastCalled = StaticData::PREALLOCATE_FN;
    return StaticData::data.nextStatus;
}

FileInterface::Status TestFile::seek(FwSignedSizeType offset, SeekType seekType) {
    StaticData::data.seekOffset = offset;
    StaticData::data.seekType = seekType;
    StaticData::data.lastCalled = StaticData::SEEK_FN;
    return StaticData::data.nextStatus;
}

FileInterface::Status TestFile::flush() {
    StaticData::data.lastCalled = StaticData::FLUSH_FN;
    return StaticData::data.nextStatus;
}

FileInterface::Status TestFile::read(U8 *buffer, FwSignedSizeType &size, WaitType wait) {
    StaticData::data.readBuffer = buffer;
    StaticData::data.readSize = size;
    StaticData::data.readWait = wait;
    StaticData::data.lastCalled = StaticData::READ_FN;
    // Copy read data if set
    if (nullptr != StaticData::data.readResult) {
        size = FW_MIN(size, StaticData::data.readResultSize - StaticData::data.pointer);
        (void) ::memcpy(buffer, StaticData::data.readResult + StaticData::data.pointer, size);
        StaticData::data.pointer += size;
    } else {
        size = StaticData::data.readSizeResult;
    }
    return StaticData::data.nextStatus;
}

FileInterface::Status TestFile::write(const U8* buffer, FwSignedSizeType &size, WaitType wait) {
    StaticData::data.writeBuffer = buffer;
    StaticData::data.writeSize = size;
    StaticData::data.writeWait = wait;
    StaticData::data.lastCalled = StaticData::WRITE_FN;
    // Copy read data if set
    if (nullptr != StaticData::data.writeResult) {
        size = FW_MIN(size, StaticData::data.writeResultSize - StaticData::data.pointer);
        (void) ::memcpy(StaticData::data.writeResult + StaticData::data.pointer, buffer, size);
        StaticData::data.pointer += size;
    } else {
        size = StaticData::data.writeSizeResult;
    }
    return StaticData::data.nextStatus;
}

FileHandle* TestFile::getHandle() {
    return &this->m_handle;
}

} // namespace Test
} // namespace File
} // namespace Stub
} // namespace Os
