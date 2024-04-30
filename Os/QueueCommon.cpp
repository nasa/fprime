#include <Os/Queue.hpp>
#include <Fw/Types/Assert.hpp>
#include <cstring>

namespace Os {

#if FW_QUEUE_REGISTRATION
    QueueRegistry* Queue::s_queueRegistry = nullptr;
#endif

    NATIVE_INT_TYPE Queue::s_numQueues = 0;

    Queue::QueueStatus Queue::send(const Fw::SerializeBufferBase &buffer, NATIVE_INT_TYPE priority, QueueBlocking block) {

        const U8* msgBuff = buffer.getBuffAddr();
        NATIVE_INT_TYPE buffLength = static_cast<NATIVE_INT_TYPE>(buffer.getBuffLength());

        return this->send(msgBuff,buffLength,priority, block);

    }

    Queue::QueueStatus Queue::receive(Fw::SerializeBufferBase &buffer, NATIVE_INT_TYPE &priority, QueueBlocking block) {

        U8* msgBuff = buffer.getBuffAddr();
        NATIVE_INT_TYPE buffCapacity = static_cast<NATIVE_INT_TYPE>(buffer.getBuffCapacity());
        NATIVE_INT_TYPE recvSize = 0;

        Queue::QueueStatus recvStat = this->receive(msgBuff, buffCapacity, recvSize, priority, block);

        if (QUEUE_OK == recvStat) {
            if (buffer.setBuffLen(static_cast<NATIVE_UINT_TYPE>(recvSize)) == Fw::FW_SERIALIZE_OK) {
                return QUEUE_OK;
            } else {
                return QUEUE_SIZE_MISMATCH;
            }
        } else {
            return recvStat;
        }
    }

    Queue::QueueStatus Queue::create(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {
        FW_ASSERT(depth > 0, depth);
        FW_ASSERT(msgSize > 0, depth);
        return createInternal(name, depth, msgSize);
    }


#if FW_QUEUE_REGISTRATION

    void Queue::setQueueRegistry(QueueRegistry* reg) {
        // NULL is okay if a deregistration is desired
        Queue::s_queueRegistry = reg;
    }

#endif

    NATIVE_INT_TYPE Queue::getNumQueues() {
        return Queue::s_numQueues;
    }

    const QueueString& Queue::getName() {
        return this->m_name;
    }

}
