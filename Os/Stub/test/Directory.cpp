// ======================================================================
// \title Os/Stub/test/Directory.cpp
// \brief implementation for TestDirectory stubs for interface testing
// ======================================================================

#include "Os/Stub/test/Directory.hpp"
#include "Os/Directory.hpp"

namespace Os {
namespace Stub {
namespace Directory {
namespace Test {

StaticData StaticData::data;

TestDirectory::TestDirectory() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestDirectory::~TestDirectory() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

TestDirectory::Status TestDirectory::open(const char* path, OpenMode mode) {
    StaticData::data.lastCalled = StaticData::LastFn::OPEN_FN;
    return Status::OP_OK;
}

TestDirectory::Status TestDirectory::rewind() {
    StaticData::data.lastCalled = StaticData::LastFn::REWIND_FN;
    return Status::OP_OK;
}

TestDirectory::Status TestDirectory::read(char * fileNameBuffer, FwSizeType bufSize) {
    StaticData::data.lastCalled = StaticData::LastFn::READ_FN;
    return Status::OP_OK;
}

void TestDirectory::close() {
    StaticData::data.lastCalled = StaticData::LastFn::CLOSE_FN;
}


Os::DirectoryHandle *TestDirectory::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::GET_HANDLE_FN;
    return nullptr;
}


}
}
}
}
