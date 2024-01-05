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
        if (snprintf(buffer, size, "ActComp: %s", this->m_objName) < 0) {
            buffer[0] = 0;
        }
    }
#endif

    void ActiveComponentBase::start(NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize, NATIVE_INT_TYPE cpuAffinity) {
        this->start(static_cast<NATIVE_UINT_TYPE>(priority), static_cast<NATIVE_UINT_TYPE>(stackSize),
                    ((cpuAffinity == -1) ? Os::Task::TASK_DEFAULT : static_cast<NATIVE_UINT_TYPE>(cpuAffinity)),
                    static_cast<NATIVE_UINT_TYPE>(identifier));
    }

    void ActiveComponentBase::start(NATIVE_UINT_TYPE priority, NATIVE_UINT_TYPE stackSize, NATIVE_UINT_TYPE cpuAffinity, NATIVE_UINT_TYPE identifier) {
        Os::TaskString taskName;

#if FW_OBJECT_NAMES == 1
        taskName = this->getObjName();
#else
        char taskNameChar[FW_TASK_NAME_MAX_SIZE];
        (void)snprintf(taskNameChar,sizeof(taskNameChar),"ActComp_%d",Os::Task::getNumTasks());
        taskName = taskNameChar;
#endif
// If running with the baremetal scheduler, use a variant of the task-loop that
// does not loop internal, but waits for an external iteration call.
#if FW_BAREMETAL_SCHEDULER == 1
        Os::Task::taskRoutine routine = this->s_baseBareTask;
#else
        Os::Task::taskRoutine routine = this->s_baseTask;
#endif
        Os::Task::TaskStatus status = this->m_task.start(taskName, routine, this, priority, stackSize, cpuAffinity, identifier);
        FW_ASSERT(status == Os::Task::TASK_OK,static_cast<NATIVE_INT_TYPE>(status));
    }

    void ActiveComponentBase::exit() {
        ActiveComponentExitSerializableBuffer exitBuff;
        SerializeStatus stat = exitBuff.serialize(static_cast<I32>(ACTIVE_COMPONENT_EXIT));
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        (void)this->m_queue.send(exitBuff,0,Os::Queue::QUEUE_NONBLOCKING);
        DEBUG_PRINT("exit %s\n", this->getObjName());
    }

    Os::Task::TaskStatus ActiveComponentBase::join(void **value_ptr) {
        DEBUG_PRINT("join %s\n", this->getObjName());
        return this->m_task.join(value_ptr);
    }

    void ActiveComponentBase::s_baseBareTask(void* ptr) {
        FW_ASSERT(ptr != nullptr);
        ActiveComponentBase* comp = reinterpret_cast<ActiveComponentBase*>(ptr);
        //Start if not started
        if (!comp->m_task.isStarted()) {
            comp->m_task.setStarted(true);
            comp->preamble();
        }
        //Bare components cannot block, so return to the scheduler
        if (comp->m_queue.getNumMsgs() == 0) {
            return;
        }
        ActiveComponentBase::MsgDispatchStatus loopStatus = comp->doDispatch();
        switch (loopStatus) {
            case ActiveComponentBase::MSG_DISPATCH_OK: // if normal message processing, continue
                break;
            case ActiveComponentBase::MSG_DISPATCH_EXIT:
                comp->finalizer();
                comp->m_task.setStarted(false);
                break;
            default:
                FW_ASSERT(0,static_cast<NATIVE_INT_TYPE>(loopStatus));
        }
    }
    void ActiveComponentBase::s_baseTask(void* ptr) {
        // cast void* back to active component
        ActiveComponentBase* comp = static_cast<ActiveComponentBase*> (ptr);
        // indicated that task is started
        comp->m_task.setStarted(true);
        // print out message when task is started
        // printf("Active Component %s task started.\n",comp->getObjName());
        // call preamble
        comp->preamble();
        // call main task loop until exit or error
        comp->loop();
        // if main loop exits, call finalizer
        comp->finalizer();
    }

    void ActiveComponentBase::loop() {

        bool quitLoop = false;
        while (!quitLoop) {
            MsgDispatchStatus loopStatus = this->doDispatch();
            switch (loopStatus) {
                case MSG_DISPATCH_OK: // if normal message processing, continue
                    break;
                case MSG_DISPATCH_EXIT:
                    quitLoop = true;
                    break;
                default:
                    FW_ASSERT(0,static_cast<NATIVE_INT_TYPE>(loopStatus));
            }
        }

    }

    void ActiveComponentBase::preamble() {
    }

    void ActiveComponentBase::finalizer() {
    }

}
