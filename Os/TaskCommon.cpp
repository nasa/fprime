#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Logger/Logger.hpp>
#include <string.h>

namespace Os {
    
    TaskRegistry* Task::s_taskRegistry = 0;
    NATIVE_INT_TYPE Task::s_numTasks = 0;
    
    NATIVE_INT_TYPE Task::getNumTasks(void) {
        return Task::s_numTasks;
    }


    I32 Task::getIdentifier() {
        return m_identifier;
    }
    
    bool Task::isStarted(void) {
        return this->m_started;
    }
    
    void Task::setStarted(bool started) {
        this->m_started = started;
    }

    bool Task::wasSuspended(void) {
        return this->m_suspendedOnPurpose;
    }

    POINTER_CAST Task::getRawHandle(void) {
        return this->m_handle;
    }
    
    void Task::registerTaskRegistry(TaskRegistry* registry) {
        FW_ASSERT(registry);
        Task::s_taskRegistry = registry;
    }
    
    TaskRegistry::TaskRegistry() {
        
    }

    Task::TaskStatus Task::start(const Fw::StringBase &name, NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, taskRoutine routine, void* arg, NATIVE_INT_TYPE cpuAffinity) {
        Fw::Logger::logMsg("[WARNING] Os::Task.start(name, identifier, priority, stackSize, routine, arg, cpuAffinity) is deprecated.\n");
        Fw::Logger::logMsg("[WARNING] Please migrate to the form: Os::Task.start(name, routine, arg, priority, stackSize,  cpuAffinity, identifier).\n");
        return this->start(name, routine, arg, priority, stackSize, cpuAffinity, identifier);
    }

    TaskRegistry::~TaskRegistry() {
        
    }
    
}
