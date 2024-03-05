#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Os/Task.hpp>
#include <Os/Baremetal/TaskRunner/BareTaskHandle.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstdio>
#include <new>

namespace Os {

Task::Task() :
    m_handle(0),
    m_identifier(0),
    m_affinity(-1),
    m_started(false),
    m_suspendedOnPurpose(false)
{}

Task::TaskStatus Task::start(const Fw::StringBase &name, taskRoutine routine, void* arg, ParamType priority, ParamType stackSize, ParamType cpuAffinity, ParamType identifier) {
    //Get a task handle, and set it up
    BareTaskHandle* handle = new(std::nothrow) BareTaskHandle();
    if (handle == nullptr) {
       return Task::TASK_UNKNOWN_ERROR;
    }
    //Set handle member variables
    handle->m_enabled = true;
    handle->m_priority = priority;
    handle->m_routine = routine;
    handle->m_argument = arg;
    //Register this task using our custom task handle
    m_handle = reinterpret_cast<POINTER_CAST>(handle);
    this->m_name = "BR_";
    this->m_name += name;
    this->m_identifier = identifier;
    // If a registry has been registered, register task
    if (Task::s_taskRegistry) {
        Task::s_taskRegistry->addTask(this);
    }
    //Running the task the first time allows setup activities for the task
    handle->m_routine(handle->m_argument);
    return Task::TASK_OK;
}

Task::TaskStatus Task::delay(NATIVE_UINT_TYPE milliseconds)
{
    //Task delays are a bad idea in baremetal tasks
    return Task::TASK_DELAY_ERROR;
}

Task::~Task() {
    if (this->m_handle) {
        delete reinterpret_cast<BareTaskHandle*>(this->m_handle);
    }
    // If a registry has been registered, remove task
    if (Task::s_taskRegistry) {
        Task::s_taskRegistry->removeTask(this);
    }
}

void Task::suspend(bool onPurpose) {
    FW_ASSERT(reinterpret_cast<BareTaskHandle*>(this->m_handle) != nullptr);
    reinterpret_cast<BareTaskHandle*>(this->m_handle)->m_enabled = false;
}

void Task::resume() {
    FW_ASSERT(reinterpret_cast<BareTaskHandle*>(this->m_handle) != nullptr);
    reinterpret_cast<BareTaskHandle*>(this->m_handle)->m_enabled = true;
}

bool Task::isSuspended() {
    FW_ASSERT(reinterpret_cast<BareTaskHandle*>(this->m_handle) != nullptr);
    return !reinterpret_cast<BareTaskHandle*>(this->m_handle)->m_enabled;
}

Task::TaskStatus Task::join(void **value_ptr) {
    return TASK_OK;
}

}
