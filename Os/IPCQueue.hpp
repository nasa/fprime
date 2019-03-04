#ifndef _IPCQueue_hpp_
#define _IPCQueue_hpp_

#include <Os/Queue.hpp>

namespace Os {
    class IPCQueue : public Os::Queue {
        public:
            IPCQueue();
            ~IPCQueue();
            QueueStatus create(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize); //!<  create a message queue

            // Base class has overloads - NOTE(mereweth) - this didn't work
            //using Queue::send;
            //using Queue::receive;

            // Send serialized buffers
            QueueStatus send(const Fw::SerializeBufferBase &buffer, NATIVE_INT_TYPE priority, QueueBlocking block); //!<  send a message
            QueueStatus receive(Fw::SerializeBufferBase &buffer, NATIVE_INT_TYPE &priority, QueueBlocking block); //!<  receive a message

            // Send raw buffers
            QueueStatus send(const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority, QueueBlocking block); //!<  send a message
            QueueStatus receive(U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority, QueueBlocking block); //!<  receive a message

            NATIVE_INT_TYPE getNumMsgs(void) const; //!< get the number of messages in the queue
            NATIVE_INT_TYPE getMaxMsgs(void) const; //!< get the maximum number of messages (high watermark)
            NATIVE_INT_TYPE getQueueSize(void) const; //!< get the queue depth (maximum number of messages queue can hold)
            NATIVE_INT_TYPE getMsgSize(void) const; //!< get the message size (maximum message size queue can hold)
    };
}

#endif
