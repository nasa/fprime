// ======================================================================
// \title Os/Stub/test/Mutex.cpp
// \brief implementation for TestMutex stubs for interface testing
// ======================================================================

#include "Os/Stub/test/Mutex.hpp"
namespace Os {
namespace Stub {
namespace Mutex {
namespace Test {

StaticData StaticData::data;

TestMutex::TestMutex() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestMutex::~TestMutex() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

Os::MutexInterface::Status TestMutex::take() {
    StaticData::data.lastCalled = StaticData::LastFn::TAKE_FN;
    return StaticData::data.takeStatus;
}

Os::MutexInterface::Status TestMutex::release() {
    StaticData::data.lastCalled = StaticData::LastFn::RELEASE_FN;
    return StaticData::data.releaseStatus;
}

Os::MutexHandle *TestMutex::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::GET_HANDLE_FN;
    return nullptr;
}


}
}
}
}
