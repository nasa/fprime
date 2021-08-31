#include <Os/IPCQueue.hpp>
#include <cstring>

namespace Os {

    Queue::QueueStatus IPCQueue::send(const Fw::SerializeBufferBase &buffer, NATIVE_INT_TYPE priority, QueueBlocking block) {

        const U8* msgBuff = buffer.getBuffAddr();
        NATIVE_INT_TYPE buffLength = buffer.getBuffLength();

        return this->send(msgBuff,buffLength,priority, block);
        
    }

    Queue::QueueStatus IPCQueue::receive(Fw::SerializeBufferBase &buffer, NATIVE_INT_TYPE &priority, QueueBlocking block) {

        U8* msgBuff = buffer.getBuffAddr();
        NATIVE_INT_TYPE buffCapacity = buffer.getBuffCapacity();
        NATIVE_INT_TYPE recvSize = 0;

        Queue::QueueStatus recvStat = this->receive(msgBuff, buffCapacity, recvSize, priority, block);

        if (QUEUE_OK == recvStat) {
            if (buffer.setBuffLen(recvSize) == Fw::FW_SERIALIZE_OK) {
                return QUEUE_OK;
            } else {
                return QUEUE_SIZE_MISMATCH;
            }
        } else {
            return recvStat;
        }
    }

}
