#include <Fw/Cfg/Config.hpp>
#include <Fw/Comp/ActiveComponentBase.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/EightyCharString.hpp>
#include <stdio.h>


namespace Fw {
    
    class ActiveComponentExitSerializableBuffer : public Fw::SerializeBufferBase {
        
        public:
            NATIVE_UINT_TYPE getBuffCapacity(void) const {
                return sizeof(m_buff);
            }
        
            U8* getBuffAddr(void) {
                return m_buff;
            }
        
            const U8* getBuffAddr(void) const {
                return m_buff;
            }

        private:
            
            U8 m_buff[sizeof(ActiveComponentBase::ACTIVE_COMPONENT_EXIT)];
            
    };
    
#if FW_OBJECT_NAMES == 1
    ActiveComponentBase::ActiveComponentBase(const char* name) : QueuedComponentBase(name) {

    }
#else
    ActiveComponentBase::ActiveComponentBase() : QueuedComponentBase() {

    }
#endif
    ActiveComponentBase::~ActiveComponentBase() {

    }
    
    void ActiveComponentBase::init(NATIVE_INT_TYPE instance) {
        QueuedComponentBase::init(instance);
    }

#if FW_OBJECT_TO_STRING == 1 && FW_OBJECT_NAMES == 1
    void ActiveComponentBase::toString(char* buffer, NATIVE_INT_TYPE size) {
        (void)snprintf(buffer, size, "ActComp: %s", this->m_objName);
        buffer[size-1] = 0;
    }
#endif
    
    void ActiveComponentBase::start(NATIVE_INT_TYPE identifier, NATIVE_INT_TYPE priority, NATIVE_INT_TYPE stackSize) {
        
        Fw::EightyCharString taskName;
        
#if FW_OBJECT_NAMES == 1
        taskName = this->getObjName();
#else
        char taskNameChar[FW_TASK_NAME_MAX_SIZE];
        (void)snprintf(taskNameChar,sizeof(taskNameChar),"ActComp_%d",Os::Task::getNumTasks());
        taskName = taskNameChar;
#endif
        
    	Os::Task::TaskStatus status = this->m_task.start(taskName, identifier, priority, stackSize, this->s_baseTask,
                        this);
    	FW_ASSERT(status == Os::Task::TASK_OK,(NATIVE_INT_TYPE)status);
    }
    
    void ActiveComponentBase::exit(void) {
        ActiveComponentExitSerializableBuffer exitBuff;
        SerializeStatus stat = exitBuff.serialize((I32)ACTIVE_COMPONENT_EXIT);
        FW_ASSERT(FW_SERIALIZE_OK == stat,static_cast<NATIVE_INT_TYPE>(stat));
        Os::Queue::QueueStatus qStat = this->m_queue.send(exitBuff,0,Os::Queue::QUEUE_NONBLOCKING);
        FW_ASSERT(Os::Queue::QUEUE_OK == qStat,static_cast<NATIVE_INT_TYPE>(qStat));
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

    void ActiveComponentBase::loop(void) {

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
                    FW_ASSERT(0,(NATIVE_INT_TYPE)loopStatus);
            }
        }
        
    }
    
    void ActiveComponentBase::preamble(void) {
    }

    void ActiveComponentBase::finalizer(void) {
    }

}
