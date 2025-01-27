//
// Created by Michael Starch on 8/27/24.
//
#include "ConditionVariable.hpp"
#include <cstring>
#include "Fw/Types/Assert.hpp"

namespace Os {
namespace Stub {
namespace ConditionVariable {
namespace Test {

StaticData StaticData::data;

TestConditionVariable::TestConditionVariable() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestConditionVariable::~TestConditionVariable() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

TestConditionVariable::Status TestConditionVariable::pend(Os::Mutex& mutex) {
    StaticData::data.lastCalled = StaticData::LastFn::WAIT_FN;
    StaticData::data.passed = &mutex;
    return TestConditionVariable::Status::OP_OK;
}

void TestConditionVariable::notify() {
    StaticData::data.lastCalled = StaticData::LastFn::NOTIFY_FN;
}

void TestConditionVariable::notifyAll() {
    StaticData::data.lastCalled = StaticData::LastFn::NOTIFY_ALL_FN;
}
ConditionVariableHandle* TestConditionVariable::getHandle() {
    StaticData::data.lastCalled = StaticData::LastFn::HANDLE_FN;
    StaticData::data.handle = &this->m_handle;
    return &this->m_handle;
}

}  // namespace Test
}  // namespace ConditionVariable
}  // namespace Stub
}  // namespace Os
