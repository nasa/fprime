// ======================================================================
// \title Os/Stub/test/CountingSemaphore.cpp
// \brief implementation for TestCountingSemaphore stubs for interface testing
// ======================================================================

#include "Os/Stub/test/CountingSemaphore.hpp"
namespace Os {
namespace Stub {
namespace CountingSemaphore {
namespace Test {

StaticData StaticData::data;

TestCountingSemaphore::TestCountingSemaphore(U32 initial_count) {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestCountingSemaphore::~TestCountingSemaphore() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

Os::CountingSemaphoreInterface::Status TestCountingSemaphore::wait() {
    StaticData::data.lastCalled = StaticData::LastFn::WAIT_FN;
    return StaticData::data.waitStatus;
}

Os::CountingSemaphoreInterface::Status TestCountingSemaphore::waitTimeout(const Fw::TimeInterval& interval) {
    StaticData::data.lastCalled = StaticData::LastFn::WAIT_TIMEOUT_FN;
    return StaticData::data.waitTimeoutStatus;
}

Os::CountingSemaphoreInterface::Status TestCountingSemaphore::tryWait() {
    StaticData::data.lastCalled = StaticData::LastFn::TRY_WAIT_FN;
    return StaticData::data.tryWaitStatus;
}

Os::CountingSemaphoreInterface::Status TestCountingSemaphore::post() {
    StaticData::data.lastCalled = StaticData::LastFn::POST_FN;
    return StaticData::data.postStatus;
}

Os::CountingSemaphoreHandle* TestCountingSemaphore::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::GET_HANDLE_FN;
    return nullptr;
}

}  // namespace Test
}  // namespace CountingSemaphore
}  // namespace Stub
}  // namespace Os
