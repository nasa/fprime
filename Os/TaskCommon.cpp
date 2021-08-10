#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>
#include <string.h>

namespace Os {

    TaskRegistry* Task::s_taskRegistry = 0;
    NATIVE_INT_TYPE Task::s_numTasks = 0;

    NATIVE_INT_TYPE Task::getNumTasks() {
        return Task::s_numTasks;
    }


    I32 Task::getIdentifier() {
        return m_identifier;
    }

    bool Task::isStarted() {
        return this->m_started;
    }

    void Task::setStarted(bool started) {
        this->m_started = started;
    }

    bool Task::wasSuspended() {
        return this->m_suspendedOnPurpose;
    }

    POINTER_CAST Task::getRawHandle() {
        return this->m_handle;
    }

    void Task::registerTaskRegistry(TaskRegistry* registry) {
        FW_ASSERT(registry);
        Task::s_taskRegistry = registry;
    }

    TaskRegistry::TaskRegistry() {

    }

    TaskRegistry::~TaskRegistry() {

    }

}
