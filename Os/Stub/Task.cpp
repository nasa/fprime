// ======================================================================
// \title Os/Stub/Task.cpp
// \brief stub implementations for Os::Task
// ======================================================================
#include "Os/Stub/Task.hpp"
namespace Os {
namespace Stub {
namespace Task {

void StubTask::onStart() {

}

Os::TaskInterface::Status StubTask::join() {
    return Os::TaskInterface::Status::UNKNOWN_ERROR;
}

void StubTask::suspend(Os::TaskInterface::SuspensionType suspensionType) {}

void StubTask::resume() {}

Os::TaskHandle *StubTask::getHandle() {
    return nullptr;
}

Os::TaskInterface::Status StubTask::start(const Os::TaskInterface::Arguments &arguments) {
    return Os::TaskInterface::Status::UNKNOWN_ERROR;
}

bool StubTask::isCooperative() {
    return true;
}

}
}
}
