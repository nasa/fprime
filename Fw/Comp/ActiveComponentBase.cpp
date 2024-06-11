#include <FpConfig.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/TaskString.hpp>
#include <cstdio>

//#define DEBUG_PRINT(...) printf(##__VA_ARGS__); fflush(stdout)
#define DEBUG_PRINT(...)

namespace Fw {

    class ActiveComponentExitSerializableBuffer : public Fw::SerializeBufferBase {

        public:
            NATIVE_UINT_TYPE getBuffCapacity() const {
                return sizeof(m_buff);
            }

            U8* getBuffAddr() {
                return m_buff;
            }

            const U8* getBuffAddr() const {
                return m_buff;
            }

        private:

            U8 m_buff[sizeof(ActiveComponentBase::ACTIVE_COMPONENT_EXIT)];

    };

    ActiveComponentBase::ActiveComponentBase(const char* name) : QueuedComponentBase(name) {

    }

    ActiveComponentBase::~ActiveComponentBase() {
        DEBUG_PRINT("ActiveComponent %s destructor.\n",this->getObjName());
    }

    void ActiveComponentBase::init(NATIVE_INT_TYPE instance) {
        QueuedComponentBase::init(instance);
    }

#if FW_OBJECT_TO_STRING == 1 && FW_OBJECT_NAMES == 1
    void ActiveComponentBase::toString(char* buffer, NATIVE_INT_TYPE size) {
        FW_ASSERT(size > 0);
        FW_ASSERT(buffer != nullptr);
        PlatformIntType status = snprintf(buffer, static_cast<size_t>(size), "ActComp: %s", this->m_objName.toChar());
        if (status < 0) {
            buffer[0] = 0;
        }
    }
#endif

    void ActiveComponentBase::start(Os::Task::ParamType priority, Os::Task::ParamType stackSize, Os::Task::ParamType cpuAffinity, Os::Task::ParamType identifier) {
        Os::TaskString taskName;

#if FW_OBJECT_NAMES == 1
        taskName = this->getObjName();
#else
        char taskNameChar[FW_TASK_NAME_BUFFER_SIZE];
        (void)snprintf(taskNameChar,sizeof(taskNameChar),"ActComp_%d",Os::Task::getNumTasks());
        taskName = taskNameChar;
#endif
        // Cooperative threads tasks externalize the task loop, and as such use the state machine as their task function
        // Standard multithreading tasks use the task loop to respectively call the state machine
        Os::Task::taskRoutine routine = (m_task.isCooperative()) ? this->s_taskStateMachine : this->s_taskLoop;
        Os::Task::Arguments arguments(taskName, routine, this, priority, stackSize, cpuAffinity, static_cast<PlatformUIntType>(identifier));
        Os::Task::Status status = this->m_task.start(arguments);
        FW_ASSERT(status == Os::Task::Status::OP_OK,static_cast<NATIVE_INT_TYPE>(status));
    }

    void ActiveComponentBase::exit() {
        ActiveComponentExitSerializableBuffer exitBuff;
        SerializeStatus stat = exitBuff.serialize(static_cast<I32>(ACTIVE_COMPONENT_EXIT));
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        (void)this->m_queue.send(exitBuff,0,Os::Queue::QUEUE_NONBLOCKING);
        DEBUG_PRINT("exit %s\n", this->getObjName());
    }

    Os::Task::Status ActiveComponentBase::join() {
        DEBUG_PRINT("join %s\n", this->getObjName());
        return this->m_task.join();
    }

    Os::Task::Status ActiveComponentBase::join(void** pointer) {
       return this->m_task.join();
    }

    void ActiveComponentBase::s_taskStateMachine(void* component_pointer) {
        FW_ASSERT(component_pointer != nullptr);
        // cast void* back to active component
        ActiveComponentBase* component = static_cast<ActiveComponentBase*>(component_pointer);

        // Each invocation of this function runs a single stage of the thread lifecycle. This has moved the thread
        // while loop to the top level such that it can be replaced by something else (e.g. cooperative thread
        // dispatcher) and is not intrinsic to this code.
        switch (component->m_stage) {
            // The first stage the active component triggers the "preamble" call before moving into the dispatching
            // stage of the component thread.
            case Lifecycle::CREATED:
                component->preamble();
                component->m_stage = Lifecycle::DISPATCHING;
                break;
            // The second stage of the active component triggers the dispatching loop dispatching messages until an
            // exit message is received.
            case Lifecycle::DISPATCHING:
                if (component->dispatch() == MsgDispatchStatus::MSG_DISPATCH_EXIT) {
                    component->m_stage = Lifecycle::FINALIZING;
                }
                break;
            // The second-to-last stage is where the finalizer is called. This will transition to the final stage
            // automatically after the finalizer is called
            case Lifecycle::FINALIZING:
                component->finalizer();
                component->m_stage = Lifecycle::DONE;
                break;
            // The last stage does nothing, cooperative tasks live here forever, threaded tasks exit on this condition
            case Lifecycle::DONE:
                break;
            default:
                FW_ASSERT(0);
                break;
        }
    }

    void ActiveComponentBase::s_taskLoop(void* component_pointer) {
        FW_ASSERT(component_pointer != nullptr);
        ActiveComponentBase* component = static_cast<ActiveComponentBase*>(component_pointer);
        // A non-cooperative task switching implementation is just a while-loop around the active component
        // state-machine. Here the while loop is at top-level.
        while (component->m_stage != ActiveComponentBase::Lifecycle::DONE) {
            ActiveComponentBase::s_taskStateMachine(component);
        }
    }

    ActiveComponentBase::MsgDispatchStatus ActiveComponentBase::dispatch() {
        // Cooperative tasks should return rather than block when no messages are available
        if (this->m_task.isCooperative() and m_queue.getNumMsgs() == 0) {
            return MsgDispatchStatus::MSG_DISPATCH_EMPTY;
        }
       return this->doDispatch();
    }

    void ActiveComponentBase::preamble() {
    }

    void ActiveComponentBase::finalizer() {
    }

}
