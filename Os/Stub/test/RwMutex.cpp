// ======================================================================
// \title Os/Stub/test/RwMutex.cpp
// \brief implementation for TestMutex stubs for interface testing
// ======================================================================

#include "Os/Stub/test/RwMutex.hpp"
namespace Os {
namespace Stub {
namespace RwMutex {
namespace Test {

StaticData StaticData::data;

TestRwMutex::TestRwMutex() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestRwMutex::~TestRwMutex() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

Os::RwMutexInterface::Status TestRwMutex::take() {
    StaticData::data.lastCalled = StaticData::LastFn::TAKE_FN;
    return StaticData::data.takeStatus;
}

Os::RwMutexInterface::Status TestRwMutex::release() {
    StaticData::data.lastCalled = StaticData::LastFn::RELEASE_FN;
    return StaticData::data.releaseStatus;
}

Os::RwMutexInterface::Status TestRwMutex::takeRead() {
    StaticData::data.lastCalled = StaticData::LastFn::TAKE_READ_FN;
    return StaticData::data.takeStatus;
}

Os::RwMutexInterface::Status TestRwMutex::releaseRead() {
    StaticData::data.lastCalled = StaticData::LastFn::RELEASE_READ_FN;
    return StaticData::data.releaseStatus;
}

Os::MutexHandle* TestRwMutex::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::GET_HANDLE_FN;
    return nullptr;
}

}  // namespace Test
}  // namespace RwMutex
}  // namespace Stub
}  // namespace Os
