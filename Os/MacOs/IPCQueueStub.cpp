// ======================================================================
// \title  Queue.cpp
// \author dinkel
// \brief  Queue implementation using the pthread library. This is NOT
//         an IPC queue. It is meant to be used between threads within
//         the same address space.
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Os/Pthreads/BufferQueue.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/IPCQueue.hpp>

#include <cerrno>
#include <pthread.h>
#include <cstdio>
#include <new>

namespace Os {

  // A helper class which stores variables for the queue handle.
  // The queue itself, a pthread condition variable, and pthread
  // mutex are contained within this container class.
  class QueueHandle {
    public:
    QueueHandle() {
      int ret;
      ret = pthread_cond_init(&this->queueNotEmpty, nullptr);
      FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
      ret = pthread_cond_init(&this->queueNotFull, nullptr);
      FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
      ret = pthread_mutex_init(&this->queueLock, nullptr);
      FW_ASSERT(ret == 0, ret); // If this fails, something horrible happened.
    }
    ~QueueHandle() {
      (void) pthread_cond_destroy(&this->queueNotEmpty);
      (void) pthread_cond_destroy(&this->queueNotFull);
      (void) pthread_mutex_destroy(&this->queueLock);
    }
    bool create(NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {
      return queue.create(static_cast<NATIVE_UINT_TYPE>(depth), static_cast<NATIVE_UINT_TYPE>(msgSize));
    }
    BufferQueue queue;
    pthread_cond_t queueNotEmpty;
    pthread_cond_t queueNotFull;
    pthread_mutex_t queueLock;
  };

  IPCQueue::IPCQueue() : Queue() {
  }

  Queue::QueueStatus IPCQueue::create(const Fw::StringBase &name, NATIVE_INT_TYPE depth, NATIVE_INT_TYPE msgSize) {
    QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);

    // Queue has already been created... remove it and try again:
    if (nullptr != queueHandle) {
        delete queueHandle;
        queueHandle = nullptr;
    }

    // Create queue handle:
    queueHandle = new(std::nothrow) QueueHandle;
    if (nullptr == queueHandle) {
      return QUEUE_UNINITIALIZED;
    }
    if( !queueHandle->create(depth, msgSize) ) {
      return QUEUE_UNINITIALIZED;
    }
    this->m_handle = reinterpret_cast<POINTER_CAST>(queueHandle);

#if FW_QUEUE_REGISTRATION
    if (this->s_queueRegistry) {
        this->s_queueRegistry->regQueue(this);
    }
#endif

    return QUEUE_OK;
  }

  IPCQueue::~IPCQueue() {
    // Clean up the queue handle:
    QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
    if (nullptr != queueHandle) {
      delete queueHandle;
    }
    this->m_handle = reinterpret_cast<POINTER_CAST>(nullptr);
  }

  Queue::QueueStatus sendNonBlockIPCStub(QueueHandle* queueHandle, const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority) {

    BufferQueue* queue = &queueHandle->queue;
    pthread_cond_t* queueNotEmpty = &queueHandle->queueNotEmpty;
    pthread_mutex_t* queueLock = &queueHandle->queueLock;
    NATIVE_INT_TYPE ret;
    Queue::QueueStatus status = Queue::QUEUE_OK;

    ////////////////////////////////
    // Locked Section
    ///////////////////////////////
    ret = pthread_mutex_lock(queueLock);
    FW_ASSERT(ret == 0, errno);
    ///////////////////////////////

    // Push item onto queue:
    bool pushSucceeded = queue->push(buffer, static_cast<NATIVE_UINT_TYPE>(size), priority);

    if(pushSucceeded) {
      // Push worked - wake up a thread that might be waiting on
      // the other end of the queue:
      ret = pthread_cond_signal(queueNotEmpty);
      FW_ASSERT(ret == 0, errno); // If this fails, something horrible happened.
    }
    else {
      // Push failed - the queue is full:
      status = Queue::QUEUE_FULL;
    }

    ///////////////////////////////
    ret = pthread_mutex_unlock(queueLock);
    FW_ASSERT(ret == 0, errno);
    ////////////////////////////////
    ///////////////////////////////

    return status;
  }

  Queue::QueueStatus sendBlockIPCStub(QueueHandle* queueHandle, const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority) {

    BufferQueue* queue = &queueHandle->queue;
    pthread_cond_t* queueNotEmpty = &queueHandle->queueNotEmpty;
    pthread_cond_t* queueNotFull = &queueHandle->queueNotFull;
    pthread_mutex_t* queueLock = &queueHandle->queueLock;
    NATIVE_INT_TYPE ret;

    ////////////////////////////////
    // Locked Section
    ///////////////////////////////
    ret = pthread_mutex_lock(queueLock);
    FW_ASSERT(ret == 0, errno);
    ///////////////////////////////

    // If the queue is full, wait until a message is taken off the queue:
    while( queue->isFull() ) {
      ret = pthread_cond_wait(queueNotFull, queueLock);
      FW_ASSERT(ret == 0, errno);
    }

    // Push item onto queue:
    bool pushSucceeded = queue->push(buffer, static_cast<NATIVE_UINT_TYPE>(size), priority);

    // The only reason push would not succeed is if the queue
    // was full. Since we waited for the queue to NOT be full
    // before sending on the queue, the push must have succeeded
    // unless there was a programming error or a bit flip.
    FW_ASSERT(pushSucceeded, pushSucceeded);

    // Push worked - wake up a thread that might be waiting on
    // the other end of the queue:
    ret = pthread_cond_signal(queueNotEmpty);
    FW_ASSERT(ret == 0, errno); // If this fails, something horrible happened.

    ///////////////////////////////
    ret = pthread_mutex_unlock(queueLock);
    FW_ASSERT(ret == 0, errno);
    ////////////////////////////////
    ///////////////////////////////

    return Queue::QUEUE_OK;
  }


  Queue::QueueStatus IPCQueue::send(const U8* buffer, NATIVE_INT_TYPE size, NATIVE_INT_TYPE priority, QueueBlocking block) {
    (void) block; // Always non-blocking for now
    QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
    BufferQueue* queue = &queueHandle->queue;

    if (nullptr == queueHandle) {
        return QUEUE_UNINITIALIZED;
    }

    if (nullptr == buffer) {
        return QUEUE_EMPTY_BUFFER;
    }

    if (size < 0 || static_cast<NATIVE_UINT_TYPE>(size) > queue->getMsgSize()) {
        return QUEUE_SIZE_MISMATCH;
    }

    if( QUEUE_NONBLOCKING == block ) {
      return sendNonBlockIPCStub(queueHandle, buffer, size, priority);
    }

    return sendBlockIPCStub(queueHandle, buffer, size, priority);
  }

  Queue::QueueStatus receiveNonBlockIPCStub(QueueHandle* queueHandle, U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority) {

      BufferQueue* queue = &queueHandle->queue;
      pthread_mutex_t* queueLock = &queueHandle->queueLock;
      pthread_cond_t* queueNotFull = &queueHandle->queueNotFull;
      NATIVE_INT_TYPE ret;

      NATIVE_UINT_TYPE size = static_cast<NATIVE_UINT_TYPE>(capacity);
      NATIVE_INT_TYPE pri = 0;
      Queue::QueueStatus status = Queue::QUEUE_OK;

      ////////////////////////////////
      // Locked Section
      ///////////////////////////////
      ret = pthread_mutex_lock(queueLock);
      FW_ASSERT(ret == 0, errno);
      ///////////////////////////////

      // Get an item off of the queue:
      bool popSucceeded = queue->pop(buffer, size, pri);

      if(popSucceeded) {
        // Pop worked - set the return size and priority:
        actualSize = static_cast<NATIVE_INT_TYPE>(size);
        priority = pri;

        // Pop worked - wake up a thread that might be waiting on
        // the send end of the queue:
        ret = pthread_cond_signal(queueNotFull);
        FW_ASSERT(ret == 0, errno); // If this fails, something horrible happened.
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

      ///////////////////////////////
      ret = pthread_mutex_unlock(queueLock);
      FW_ASSERT(ret == 0, errno);
      ////////////////////////////////
      ///////////////////////////////

      return status;
  }

  Queue::QueueStatus receiveBlockIPCStub(QueueHandle* queueHandle, U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority) {

      BufferQueue* queue = &queueHandle->queue;
      pthread_cond_t* queueNotEmpty = &queueHandle->queueNotEmpty;
      pthread_cond_t* queueNotFull = &queueHandle->queueNotFull;
      pthread_mutex_t* queueLock = &queueHandle->queueLock;
      NATIVE_INT_TYPE ret;

      NATIVE_UINT_TYPE size = static_cast<NATIVE_UINT_TYPE>(capacity);
      NATIVE_INT_TYPE pri = 0;
      Queue::QueueStatus status = Queue::QUEUE_OK;

      ////////////////////////////////
      // Locked Section
      ///////////////////////////////
      ret = pthread_mutex_lock(queueLock);
      FW_ASSERT(ret == 0, errno);
      ///////////////////////////////

      // If the queue is empty, wait until a message is put on the queue:
      while( queue->isEmpty() ) {
        ret = pthread_cond_wait(queueNotEmpty, queueLock);
        FW_ASSERT(ret == 0, errno);
      }

      // Get an item off of the queue:
      bool popSucceeded = queue->pop(buffer, size, pri);

      if(popSucceeded) {
        // Pop worked - set the return size and priority:
        actualSize = static_cast<NATIVE_INT_TYPE>(size);
        priority = pri;

        // Pop worked - wake up a thread that might be waiting on
        // the send end of the queue:
        ret = pthread_cond_signal(queueNotFull);
        FW_ASSERT(ret == 0, errno); // If this fails, something horrible happened.
      }
      else {
        actualSize = 0;
        if( size > static_cast<NATIVE_UINT_TYPE>(capacity) ) {
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

      ///////////////////////////////
      ret = pthread_mutex_unlock(queueLock);
      FW_ASSERT(ret == 0, errno);
      ////////////////////////////////
      ///////////////////////////////

      return status;
  }

  Queue::QueueStatus IPCQueue::receive(U8* buffer, NATIVE_INT_TYPE capacity, NATIVE_INT_TYPE &actualSize, NATIVE_INT_TYPE &priority, QueueBlocking block) {

      if( reinterpret_cast<POINTER_CAST>(nullptr) == this->m_handle ) {
        return QUEUE_UNINITIALIZED;
      }

      QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);

      if (nullptr == queueHandle) {
        return QUEUE_UNINITIALIZED;
      }

      // Do not need to check the upper bound of capacity, We don't care
      // how big the user's buffer is.. as long as it's big enough.
      if (capacity < 0) {
          return QUEUE_SIZE_MISMATCH;
      }

      if( QUEUE_NONBLOCKING == block ) {
        return receiveNonBlockIPCStub(queueHandle, buffer, capacity, actualSize, priority);
      }

      return receiveBlockIPCStub(queueHandle, buffer, capacity, actualSize, priority);
  }

  NATIVE_INT_TYPE IPCQueue::getNumMsgs() const {
      QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
      if (nullptr == queueHandle) {
          return 0;
      }
      BufferQueue* queue = &queueHandle->queue;
      return static_cast<NATIVE_INT_TYPE>(queue->getCount());
  }

  NATIVE_INT_TYPE IPCQueue::getMaxMsgs() const {
      QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
      if (nullptr == queueHandle) {
          return 0;
      }
      BufferQueue* queue = &queueHandle->queue;
      return static_cast<NATIVE_INT_TYPE>(queue->getMaxCount());
  }

  NATIVE_INT_TYPE IPCQueue::getQueueSize() const {
      QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
      if (nullptr == queueHandle) {
          return 0;
      }
      BufferQueue* queue = &queueHandle->queue;
      return static_cast<NATIVE_INT_TYPE>(queue->getDepth());
  }

  NATIVE_INT_TYPE IPCQueue::getMsgSize() const {
      QueueHandle* queueHandle = reinterpret_cast<QueueHandle*>(this->m_handle);
      if (nullptr == queueHandle) {
          return 0;
      }
      BufferQueue* queue = &queueHandle->queue;
      return static_cast<NATIVE_INT_TYPE>(queue->getMsgSize());
  }

}
