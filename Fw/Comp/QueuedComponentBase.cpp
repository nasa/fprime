#include <Fw/Comp/QueuedComponentBase.hpp>
#include <Fw/Types/Assert.hpp>
#include <FpConfig.hpp>
#include <Os/QueueString.hpp>

#include <cstdio>

namespace Fw {

    QueuedComponentBase::QueuedComponentBase(const char* name) : PassiveComponentBase(name),m_msgsDropped(0) {

    }

    QueuedComponentBase::~QueuedComponentBase() {

    }

    void QueuedComponentBase::init(NATIVE_INT_TYPE instance) {
        PassiveComponentBase::init(instance);
    }

#if FW_OBJECT_TO_STRING == 1 && FW_OBJECT_NAMES == 1
    void QueuedComponentBase::toString(char* buffer, NATIVE_INT_TYPE size) {
        FW_ASSERT(size > 0);
        FW_ASSERT(buffer != nullptr);
        PlatformIntType status = snprintf(buffer, static_cast<size_t>(size), "QueueComp: %s", this->m_objName.toChar());
        if (status < 0) {
            buffer[0] = 0;
        }
    }
#endif

    Os::Queue::Queue::Status QueuedComponentBase::createQueue(FwSizeType depth, FwSizeType msgSize) {

        Os::QueueString queueName;
#if FW_OBJECT_NAMES == 1
        queueName = this->m_objName;
#else
        char queueNameChar[FW_QUEUE_NAME_BUFFER_SIZE];
        (void)snprintf(queueNameChar,sizeof(queueNameChar),"CompQ_%d",Os::Queue::getNumQueues());
        queueName = queueNameChar;
#endif
    	return this->m_queue.create(queueName, depth, msgSize);
    }

    NATIVE_INT_TYPE QueuedComponentBase::getNumMsgsDropped() {
        return this->m_msgsDropped;
    }

    void QueuedComponentBase::incNumMsgDropped() {
        this->m_msgsDropped++;
    }

}
