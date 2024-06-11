// ======================================================================
// \title  Queue.cpp
// \author mstarch, borrowed from @dinkel
// \brief  Queue implementation for Baremetal devices. No IPC nor thread
//         safety is implemented as this intended for baremetal devices.
//         Based on Os/Pthreads/Queue.cpp from @dinkel
// ======================================================================
#include <FpConfig.hpp>
#include <Os/Pthreads/BufferQueue.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Queue.hpp>

#include <new>
#include <cstdio>

namespace Os {
/**
* Wrapper class used to convert the BufferQueue into a "handler" used by
* the Queue class.
*/
class BareQueueHandle {
    public:
        BareQueueHandle() : m_init(false) {}
        /**
         * Create a new queue for use in the system.
         * WARNING: this **must** be called during initialization.
         */
        bool create(NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {
            bool ret = m_queue.create(depth, msgSize);
            m_init = ret;
            return ret;
        }
        bool m_init;
        //!< Actual queue used to store
        BufferQueue m_queue;
};

Queue::Queue() :
    m_handle(reinterpret_cast<POINTER_CAST>(nullptr))
{ }

Queue::QueueStatus Queue::createInternal(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {
    BareQueueHandle* handle = reinterpret_cast<BareQueueHandle*>(this->m_handle);
    // Queue has already been created... remove it and try again:
    if (nullptr != handle) {
        delete handle;
        handle = nullptr;
    }
    //New queue handle, check for success or return error
    handle = new(std::nothrow) BareQueueHandle;
    if (nullptr == handle || !handle->create(depth, msgSize)) {
        return QUEUE_UNINITIALIZED;
    }
    //Set handle member variable
    this->m_handle = reinterpret_cast<POINTER_CAST>(handle);
    //Register the queue
    #if FW_QUEUE_REGISTRATION
    if (this->s_queueRegistry) {
        this->s_queueRegistry->regQueue(this);
    }
    #endif
    return QUEUE_OK;
}

Queue::~Queue() {
    // Clean up the queue handle:
    BareQueueHandle* handle = reinterpret_cast<BareQueueHandle*>(this->m_handle);
    if (nullptr != handle) {
        delete handle;
    }
    this->m_handle = reinterpret_cast<POINTER_CAST>(nullptr);
}

Queue::QueueStatus bareSendNonBlock(BareQueueHandle& handle, const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority) {
    FW_ASSERT(handle.m_init);
    BufferQueue& queue = handle.m_queue;
    Queue::QueueStatus status = Queue::QUEUE_OK;
    // Push item onto queue:
    bool success = queue.push(buffer, size, priority);
    if(!success) {
        status = Queue::QUEUE_FULL;
    }
    return status;
}

Queue::QueueStatus bareSendBlock(BareQueueHandle& handle, const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority) {
    FW_ASSERT(handle.m_init);
    BufferQueue& queue = handle.m_queue;
    // If the queue is full, wait until a message is taken off the queue.
    while(queue.isFull()) {
        //Forced to assert, as blocking would destroy timely-ness
        FW_ASSERT(false);
    }
    // Push item onto queue:
    bool success = queue.push(buffer, size, priority);
    // The only reason push would not succeed is if the queue
    // was full. Since we waited for the queue to NOT be full
    // before sending on the queue, the push must have succeeded
    // unless there was a programming error or a bit flip.
    FW_ASSERT(success, success);
    return Queue::QUEUE_OK;
}

Queue::QueueStatus Queue::send(const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority, QueueBlocking block) {
    //Check if the handle is null or check the underlying queue is null
    if ((nullptr == reinterpret_cast<BareQueueHandle*>(this->m_handle)) ||
        (!reinterpret_cast<BareQueueHandle*>(this->m_handle)->m_init)) {
        return QUEUE_UNINITIALIZED;
    }
    BareQueueHandle& handle = *reinterpret_cast<BareQueueHandle*>(this->m_handle);
    BufferQueue& queue = handle.m_queue;
    //Check that the buffer is non-null
    if (nullptr == buffer) {
        return QUEUE_EMPTY_BUFFER;
    }
    //Fail if there is a size miss-match
    if (size < 0 || static_cast<NATIVE_UINT_TYPE>(size) > queue.getMsgSize()) {
        return QUEUE_SIZE_MISMATCH;
    }
    //Send to the queue
    if( QUEUE_NONBLOCKING == block ) {
        return bareSendNonBlock(handle, buffer, size, priority);
    }

    return bareSendBlock(handle, buffer, size, priority);
}

Queue::QueueStatus bareReceiveNonBlock(BareQueueHandle& handle, U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority) {
    FW_ASSERT(handle.m_init);
    BufferQueue& queue = handle.m_queue;
    NATIVE_UINT_TYPE size = static_cast<NATIVE_UINT_TYPE>(capacity);
    NATIVE_INT_TYPE pri = 0;
    Queue::QueueStatus status = Queue::QUEUE_OK;
    // Get an item off of the queue:
    bool success = queue.pop(buffer, size, pri);
    if(success) {
        // Pop worked - set the return size and priority:
        actualSize = static_cast<NATIVE_INT_TYPE>(size);
        priority = pri;
    }
    else {
        actualSize = 0;
        if( size > static_cast<NATIVE_UINT_TYPE>(capacity) ) {
            // The buffer capacity was too small!
            status = Queue::QUEUE_SIZE_MISMATCH;
        }
        else if( size == 0 ) {
            // The queue is empty:
            status = Queue::QUEUE_NO_MORE_MSGS;
        }
        else {
          // If this happens, a programming error or bit flip occurred:
          FW_ASSERT(0);
        }
    }
    return status;
}

Queue::QueueStatus bareReceiveBlock(BareQueueHandle& handle, U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority) {
    FW_ASSERT(handle.m_init);
    BufferQueue& queue = handle.m_queue;
    NATIVE_UINT_TYPE size = static_cast<NATIVE_UINT_TYPE>(capacity);
    NATIVE_INT_TYPE pri = 0;
    Queue::QueueStatus status = Queue::QUEUE_OK;
    // If the queue is full, wait until a message is taken off the queue.
    while(queue.isEmpty()) {
        //Forced to assert, as blocking would destroy timely-ness
        FW_ASSERT(false);
    }
    // Get an item off of the queue:
    bool success = queue.pop(buffer, size, pri);
    if(success) {
        // Pop worked - set the return size and priority:
        actualSize = static_cast<NATIVE_INT_TYPE>(size);
        priority = pri;
    }
    else {
        actualSize = 0;
        if( size > (static_cast<NATIVE_UINT_TYPE>(capacity)) ) {
            // The buffer capacity was too small!
            status = Queue::QUEUE_SIZE_MISMATCH;
        }
        else {
            // If this happens, a programming error or bit flip occurred:
            // The only reason a pop should fail is if the user's buffer
            // was too small.
            FW_ASSERT(0);
        }
    }
    return status;
}

Queue::QueueStatus Queue::receive(U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority, QueueBlocking block) {
    //Check if the handle is null or check the underlying queue is null
    if ((nullptr == reinterpret_cast<BareQueueHandle*>(this->m_handle)) ||
        (!reinterpret_cast<BareQueueHandle*>(this->m_handle)->m_init)) {
        return QUEUE_UNINITIALIZED;
    }
    BareQueueHandle& handle = *reinterpret_cast<BareQueueHandle*>(this->m_handle);
    //Check the buffer can hold the out-going message
    if (capacity < this->getMsgSize()) {
        return QUEUE_SIZE_MISMATCH;
    }
    //Receive either non-blocking or blocking
    if(QUEUE_NONBLOCKING == block) {
        return bareReceiveNonBlock(handle, buffer, capacity, actualSize, priority);
    }
    return bareReceiveBlock(handle, buffer, capacity, actualSize, priority);
}

NATIVE_INT_TYPE Queue::getNumMsgs() const {
    //Check if the handle is null or check the underlying queue is null
    if ((nullptr == reinterpret_cast<BareQueueHandle*>(this->m_handle)) ||
        (!reinterpret_cast<BareQueueHandle*>(this->m_handle)->m_init)) {
        return 0;
    }
    BareQueueHandle& handle = *reinterpret_cast<BareQueueHandle*>(this->m_handle);
    BufferQueue& queue = handle.m_queue;
    return queue.getCount();
}

NATIVE_INT_TYPE Queue::getMaxMsgs() const {
    //Check if the handle is null or check the underlying queue is null
    if ((nullptr == reinterpret_cast<BareQueueHandle*>(this->m_handle)) ||
        (!reinterpret_cast<BareQueueHandle*>(this->m_handle)->m_init)) {
        return 0;
    }
    BareQueueHandle& handle = *reinterpret_cast<BareQueueHandle*>(this->m_handle);
    BufferQueue& queue = handle.m_queue;
    return queue.getMaxCount();
}

NATIVE_INT_TYPE Queue::getQueueSize() const {
      //Check if the handle is null or check the underlying queue is null
      if ((nullptr == reinterpret_cast<BareQueueHandle*>(this->m_handle)) ||
          (!reinterpret_cast<BareQueueHandle*>(this->m_handle)->m_init)) {
          return 0;
      }
      BareQueueHandle& handle = *reinterpret_cast<BareQueueHandle*>(this->m_handle);
      BufferQueue& queue = handle.m_queue;
      return queue.getDepth();
  }

NATIVE_INT_TYPE Queue::getMsgSize() const {
    //Check if the handle is null or check the underlying queue is null
    if ((nullptr == reinterpret_cast<BareQueueHandle*>(this->m_handle)) ||
        (!reinterpret_cast<BareQueueHandle*>(this->m_handle)->m_init)) {
        return 0;
    }
    BareQueueHandle& handle = *reinterpret_cast<BareQueueHandle*>(this->m_handle);
    BufferQueue& queue = handle.m_queue;
    return queue.getMsgSize();
}
}//Namespace Os
