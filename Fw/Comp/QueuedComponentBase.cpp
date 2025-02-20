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

#if FW_OBJECT_TO_STRING == 1
    const char* QueuedComponentBase::getToStringFormatString() {
        return "QueueComp: %s";
    }
#endif

    Os::Queue::Queue::Status QueuedComponentBase::createQueue(FwSizeType depth, FwSizeType msgSize) {

        Os::QueueString queueName;
#if FW_OBJECT_NAMES == 1
        queueName = this->m_objName;
#else
        queueName.format("CompQ_%" PRI_FwSizeType,Os::Queue::getNumQueues());
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
