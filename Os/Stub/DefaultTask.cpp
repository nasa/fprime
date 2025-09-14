// ======================================================================
// \title Os/Stub/test/DefaultTask.cpp
// \brief sets default Os::Task to test stub implementation via linker
// ======================================================================
#include <sys/time.h>
#include <cerrno>
#include "Os/Delegate.hpp"
#include "Os/Stub/Task.hpp"
#include "Os/Task.hpp"

namespace Os {
TaskInterface* TaskInterface::getDelegate(TaskHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<TaskInterface, Os::Stub::Task::StubTask>(aligned_new_memory);
}

}  // namespace Os
