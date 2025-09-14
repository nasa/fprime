// ======================================================================
// \title Os/Posix/DefaultTask.cpp
// \brief sets default Os::Task to posix implementation via linker
// ======================================================================
#include "Os/Delegate.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/Task.hpp"

namespace Os {

TaskInterface* TaskInterface::getDelegate(TaskHandleStorage& aligned_new_memory) {
    return Os::Delegate::makeDelegate<TaskInterface, Os::Posix::Task::PosixTask>(aligned_new_memory);
}

}  // namespace Os
