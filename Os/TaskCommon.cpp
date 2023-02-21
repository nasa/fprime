#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>


namespace Os {
    const NATIVE_UINT_TYPE Task::TASK_DEFAULT = std::numeric_limits<PlatformUIntType>::max();
    
    TaskRegistry* Task::s_taskRegistry = nullptr;
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

    Task::TaskStatus Task::start(const Fw::StringBase &name, NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, taskRoutine routine, void* arg, NATIVE_INT_TYPE cpuAffinity) {
        return this->start(name, routine, arg, priority, stackSize, cpuAffinity, identifier);
    }

    TaskRegistry::~TaskRegistry() {

    }

}
