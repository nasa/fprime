//
// Created by Michael Starch on 4/12/24.
//

#include "Os/Stub/test/Task.hpp"
namespace Os {
namespace Stub {
namespace Task {
namespace Test {

StaticData StaticData::data;

TestTask::TestTask() {
    StaticData::data.lastCalled = StaticData::LastFn::CONSTRUCT_FN;
}

TestTask::~TestTask() {
    StaticData::data.lastCalled = StaticData::LastFn::DESTRUCT_FN;
}

void TestTask::onStart() {

}

Os::TaskInterface::Status TestTask::join() {
    StaticData::data.lastCalled = StaticData::LastFn::JOIN_FN;
    return StaticData::data.joinStatus;
}

void TestTask::suspend(Os::TaskInterface::SuspensionType suspensionType) {
    StaticData::data.suspensionType = suspensionType;
    StaticData::data.lastCalled = StaticData::LastFn::SUSPEND_FN;

}

void TestTask::resume() {
    StaticData::data.lastCalled = StaticData::LastFn::RESUME_FN;
}

Os::TaskHandle *TestTask::getHandle() {
    return nullptr;
}

Os::TaskInterface::Status TestTask::start(const Os::TaskInterface::Arguments &arguments) {
    StaticData::data.lastCalled = StaticData::LastFn::START_FN;
    return StaticData::data.startStatus;
}

bool TestTask::isCooperative() {
    return true;
}
}
}
}
}
