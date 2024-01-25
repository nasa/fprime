#include "Os/Stub/test/File.hpp"
#include "Os/File.hpp"
#include <new>
namespace Os {
namespace Stub {
namespace File {
namespace Test {

Data Data::testData;

void Data::setNextStatus(Os::File::Status status) {
    Data::testData.nextStatus = status;
}

void Data::setSizeResult(FwSignedSizeType size) {
    Data::testData.sizeResult = size;
}

void Data::setPositionResult(FwSignedSizeType position) {
    Data::testData.positionResult = position;
}

void Data::setReadResult(U8 *buffer, FwSignedSizeType size) {
    Data::testData.readResult = buffer;
    Data::testData.readResultSize = size;
}

void Data::setReadSize(FwSignedSizeType size) {
    Data::testData.readSizeResult = size;
}

void Data::setWriteResult(U8 *buffer, FwSignedSizeType size) {
    Data::testData.writeResult = buffer;
    Data::testData.writeResultSize = size;
}

void Data::setWriteSize(FwSignedSizeType size) {
    Data::testData.writeSizeResult = size;
}

void Data::setReadOverride(Data::ReadOverride function, void *pointer) {
    Data::testData.readOverride = function;
    Data::testData.readOverridePointer = pointer;
}

void Data::setWriteOverride(Data::WriteOverride function, void *pointer) {
    Data::testData.writeOverride = function;
    Data::testData.writeOverridePointer = pointer;
}

Os::File::Status Data::basicRead(U8 *buffer, FwSignedSizeType &size, bool wait, void* pointer) {
    Os::Stub::File::Test::Data::testData.readBuffer = buffer;
    Os::Stub::File::Test::Data::testData.readSize = size;
    Os::Stub::File::Test::Data::testData.readWait = wait;
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::READ_FN;
    // Copy read data if set
    if (nullptr != Os::Stub::File::Test::Data::testData.readResult) {
        size = FW_MIN(size, Os::Stub::File::Test::Data::testData.readResultSize - Os::Stub::File::Test::Data::testData.pointer);
        (void) ::memcpy(buffer, Os::Stub::File::Test::Data::testData.readResult + Os::Stub::File::Test::Data::testData.pointer, size);
        Os::Stub::File::Test::Data::testData.pointer += size;
    } else {
        size = Os::Stub::File::Test::Data::testData.readSizeResult;
    }
    return Os::Stub::File::Test::Data::testData.nextStatus;
}

Os::File::Status Data::basicWrite(const void *buffer, FwSignedSizeType &size, bool wait, void* pointer) {
    Os::Stub::File::Test::Data::testData.writeBuffer = buffer;
    Os::Stub::File::Test::Data::testData.writeSize = size;
    Os::Stub::File::Test::Data::testData.writeWait = wait;
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::WRITE_FN;
    // Copy read data if set
    if (nullptr != Os::Stub::File::Test::Data::testData.writeResult) {
        size = FW_MIN(size, Os::Stub::File::Test::Data::testData.writeResultSize - Os::Stub::File::Test::Data::testData.pointer);
        (void) ::memcpy(Os::Stub::File::Test::Data::testData.writeResult + Os::Stub::File::Test::Data::testData.pointer, buffer, size);
        Os::Stub::File::Test::Data::testData.pointer += size;
    } else {
        size = Os::Stub::File::Test::Data::testData.writeSizeResult;
    }
    return Os::Stub::File::Test::Data::testData.nextStatus;
}


} // namespace Test
} // namespace File
} // namespace Stub

class FileHandle{};

void File::constructInternal() {
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::CONSTRUCT_FN;
    // Placement-new the file handle into the opaque file-handle storage
    this->m_handle = new(&this->m_handle_storage[0]) FileHandle;
}

void File::destructInternal() {
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::DESTRUCT_FN;
}

Os::File::Status File::openInternal(const char *filepath, Os::File::Mode open_mode, bool overwrite) {
    Os::Stub::File::Test::Data::testData.openPath = filepath;
    Os::Stub::File::Test::Data::testData.openMode = open_mode;
    Os::Stub::File::Test::Data::testData.openOverwrite = overwrite;
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::OPEN_FN;
    Os::Stub::File::Test::Data::testData.pointer = 0;
    return Os::Stub::File::Test::Data::testData.nextStatus;
}

void File::closeInternal() {
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::CLOSE_FN;
}

Os::File::Status File::sizeInternal(FwSignedSizeType& size_result) {
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::SIZE_FN;
    size_result = Os::Stub::File::Test::Data::testData.sizeResult;
    return Os::Stub::File::Test::Data::testData.nextStatus;
}

Os::File::Status File::positionInternal(FwSignedSizeType& position_result) {
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::POSITION_FN;
    position_result = Os::Stub::File::Test::Data::testData.positionResult;
    return Os::Stub::File::Test::Data::testData.nextStatus;
}

Os::File::Status File::preallocateInternal(FwSignedSizeType offset, FwSignedSizeType length) {
    Os::Stub::File::Test::Data::testData.preallocateOffset = offset;
    Os::Stub::File::Test::Data::testData.preallocateLength = length;
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::PREALLOCATE_FN;
    return Os::Stub::File::Test::Data::testData.nextStatus;
}

Os::File::Status File::seekInternal(FwSignedSizeType offset, bool absolute) {
    Os::Stub::File::Test::Data::testData.seekOffset = offset;
    Os::Stub::File::Test::Data::testData.seekAbsolute = absolute;
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::SEEK_FN;
    return Os::Stub::File::Test::Data::testData.nextStatus;
}

Os::File::Status File::flushInternal() {
    Os::Stub::File::Test::Data::testData.lastCalled = Os::Stub::File::Test::Data::FLUSH_FN;
    return Os::Stub::File::Test::Data::testData.nextStatus;
}

Os::File::Status File::readInternal(U8 *buffer, FwSignedSizeType &size, bool wait) {
    Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
    if (Os::Stub::File::Test::Data::testData.readOverride != nullptr) {
        status = Os::Stub::File::Test::Data::testData.readOverride(buffer, size, wait, Os::Stub::File::Test::Data::testData.readOverridePointer);
    }
    return status;
}

Os::File::Status File::writeInternal(const void *buffer, FwSignedSizeType &size, bool wait) {
    Os::File::Status status = Os::File::Status::NOT_SUPPORTED;
    if (Os::Stub::File::Test::Data::testData.writeOverride != nullptr) {
        status = Os::Stub::File::Test::Data::testData.writeOverride(buffer, size, wait, Os::Stub::File::Test::Data::testData.writeOverridePointer);
    }
    return status;
}


} // namespace Os
