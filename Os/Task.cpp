// ======================================================================
// \title Os/Task.cpp
// \brief common function implementation for Os::Task
// ======================================================================
#include <Os/Task.hpp>
#include <Fw/Types/Assert.hpp>

namespace Os {

TaskInterface::Arguments::Arguments(const Fw::StringBase &name, const Os::TaskInterface::taskRoutine routine,
                                    void * const routine_argument, const FwSizeType priority,
                                    const FwSizeType stackSize, const FwIndexType cpuAffinity,
                                    const PlatformUIntType identifier) :
    m_name(name),
    m_routine(routine),
    m_routine_argument(routine_argument),
    m_priority(priority),
    m_stackSize(stackSize),
    m_cpuAffinity(cpuAffinity),
    m_identifier(identifier)
{
    FW_ASSERT(routine != nullptr);
}

void Task::TaskRoutineWrapper::run(void* wrapper_pointer) {
    FW_ASSERT(wrapper_pointer != nullptr);
    Task& task = *reinterpret_cast<Task*>(wrapper_pointer);
    FW_ASSERT(task.m_wrapper.m_user_function != nullptr);
    task.setStarted();
    task.m_wrapper.m_user_function(task.m_wrapper.m_user_argument);
}

Task::Task() : m_handle_storage(), m_delegate(*TaskInterface::getDelegate(m_handle_storage)) {}

Task::~Task() {
    // If a registry has been registered, remove task from it
    if (Task::s_taskRegistry) {
        Task::s_taskRegistry->removeTask(this);
    }
}

void Task::suspend() {
    this->suspend(Task::SuspensionType::UNINTENTIONAL);
}

Task::State Task::getState() {
    Task::State state;
    this->m_lock.lock();
    state = this->m_state;
    this->m_lock.unlock();
    return state;
}

Task::Status Task::start(const Fw::StringBase &name, const taskRoutine routine, void* const arg,
                         const ParamType priority, const ParamType stackSize, const ParamType cpuAffinity,
                         const ParamType identifier) {
    FW_ASSERT(routine != nullptr);
    return this->start(Task::Arguments(name, routine, arg,
                                       priority,
                                       stackSize,
                                       static_cast<FwIndexType>(cpuAffinity),
                                       static_cast<PlatformUIntType>(identifier)));
}


Task::Status Task::start(const Task::Arguments& arguments) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<TaskInterface*>(&this->m_handle_storage[0]));
    FW_ASSERT(arguments.m_routine != nullptr);
    this->m_name = arguments.m_name;

    // Set up our wrapper data
    this->m_wrapper.m_user_function = arguments.m_routine;
    this->m_wrapper.m_user_argument = arguments.m_routine_argument;

    // TODO: should this be made more efficient?
    Arguments wrapped_arguments = arguments;
    wrapped_arguments.m_routine = Task::TaskRoutineWrapper::run;
    wrapped_arguments.m_routine_argument = this;

    Task::Status status = this->m_delegate.start(arguments);
    if (status == Task::Status::OP_OK) {
        Task::s_taskMutex.lock();
        Task::s_numTasks++;
        Task::s_taskMutex.unlock();

        // If a registry has been registered, register task to it
        if (Task::s_taskRegistry) {
            Task::s_taskRegistry->addTask(this);
        }
    }
    return status;
}

Task::Status Task::join() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<TaskInterface*>(&this->m_handle_storage[0]));
    Task::Status status = Task::Status::INVALID_STATE;
    Task::State state = this->getState();
    if (state == Task::RUNNING) {
        status = this->m_delegate.join();
        this->m_lock.lock();
        if (status == Task::Status::OP_OK) {
            this->m_state = Task::State::EXITED;
        } else {
            this->m_state = Task::State::UNKNOWN;
        }
        this->m_lock.unlock();
    }
    return status;
}

void Task::suspend(Task::SuspensionType suspensionType) {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<TaskInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.suspend(suspensionType);
    this->m_lock.lock();
    this->m_state = (suspensionType == Task::SuspensionType::INTENTIONAL) ? State::SUSPENDED_INTENTIONALLY : State::SUSPENDED_UNINTENTIONALLY;
    this->m_lock.unlock();
}

void Task::resume() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<TaskInterface*>(&this->m_handle_storage[0]));
    this->m_delegate.resume();
}

TaskHandle* Task::getHandle() {
    FW_ASSERT(&this->m_delegate == reinterpret_cast<TaskInterface*>(&this->m_handle_storage[0]));
    return this->m_delegate.getHandle();
}

void Task::setStarted() {
    this->m_lock.lock();
    this->m_state = Task::State::RUNNING;
    this->m_lock.unlock();
}

FwSizeType Task::getNumTasks() {
    return Task::s_numTasks;
}

void Task::registerTaskRegistry(TaskRegistry* registry) {
    Task::s_taskRegistry = registry;
}

}