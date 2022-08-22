// ======================================================================
// \title  BufferQueue.hpp
// \author dinkel
// \brief  A generic buffer queue data structure.
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef OS_PTHREADS_BUFFER_QUEUE_HPP
#define OS_PTHREADS_BUFFER_QUEUE_HPP

#include <FpConfig.hpp>

// This is a generic buffer queue interface. 
namespace Os {

  //! \class BufferQueue
  //! \brief A generic buffer queue data structure
  //!
  //! This is a generic queue interface that can be implemented using a variety
  //! of different data structures
  class BufferQueue {
    // Public interface:
    public:
    //! \brief BufferQueue constructor
    //!
    //! Create a BufferQueue object.
    //!
    BufferQueue();
    //! \brief BufferQueue deconstructor
    //!
    //! Deallocate the queue.
    //!
    ~BufferQueue();
    //! \brief BufferQueue creation
    //!
    //! Create a queue with buffer allocated at initialization. Messages 
    //! will have a maximum size "msgSize" and the buffer with be "depth"
    //! elements deep.
    //!
    //! \param depth the maximum number of buffers to store on queue
    //! \param msgSize the maximum size of a buffer that can be stored on
    //! the queue
    //!
    bool create(NATIVE_UINT_TYPE depth, NATIVE_UINT_TYPE msgSize);
    //! \brief push an item onto the queue
    //!
    //! Push an item onto the queue with the specified size and priority
    //!
    //! \param buffer the buffer to push onto the queue
    //! \param size the size of buffer
    //! \param priority the priority of the buffer on the queue (higher 
    //! priority means that the buffer will be popped off sooner.
    //!
    bool push(const U8* buffer, NATIVE_UINT_TYPE size, NATIVE_INT_TYPE priority);
    //! \brief pop an item off the queue
    //!
    //! Pull an item off of the queue and put it in "buffer" which is of size
    //! "size". Returns the size of the pulled item in "size" and the priority
    //! of the pulled item in "priority" on a success.
    //!
    //! \param buffer the buffer to fill from the queue
    //! \param size the size of buffer. The size of the popped buffer
    //! will also be returned in this variable. It is used as both input
    //! and output
    //! \param priority the priority of the buffer popped off the queue
    //!
    bool pop(U8* buffer, NATIVE_UINT_TYPE& size, NATIVE_INT_TYPE &priority);
    //! \brief check if the queue is full
    //!
    //! Is the queue full?
    //!
    bool isFull();
    //! \brief check if the queue is empty
    //!
    //! Is the queue empty?
    //!
    bool isEmpty();
    //! \brief Get the current number of items on the queue
    //!
    //! Get the number of items on the queue.
    //!
    NATIVE_UINT_TYPE getCount();
    //! \brief Get the maximum number of items seen on the queue
    //!
    //! Get the maximum number of items that have been on the queue since the
    //! instantiation of the queue. This is a "high water mark" count.
    //!
    NATIVE_UINT_TYPE getMaxCount();
    //! \brief Get the maximum message size
    //!
    //! Get the maximum message size allowed on the queue
    //!
    NATIVE_UINT_TYPE getMsgSize();
    //! \brief Get the queue depths
    //!
    //! Get the maximum number of messages allowed on the queue
    //!
    NATIVE_UINT_TYPE getDepth();

    // Internal member functions:
    private:
    // Initialize data structures necessary for the queue:
    bool initialize(NATIVE_UINT_TYPE depth, NATIVE_UINT_TYPE msgSize);
    // Destroy queue data structures:
    void finalize();
    // Enqueue a message into the data structure:
    bool enqueue(const U8* buffer, NATIVE_UINT_TYPE size, NATIVE_INT_TYPE priority);
    // Dequeue a message from the data structure:
    bool dequeue(U8* buffer, NATIVE_UINT_TYPE& size, NATIVE_INT_TYPE &priority);
    // Low level enqueue which does the copying onto the queue:
    void enqueueBuffer(const U8* buffer, NATIVE_UINT_TYPE size, U8* data, NATIVE_UINT_TYPE index);
    // Low level dequeue which does the copying from the queue:
    bool dequeueBuffer(U8* buffer, NATIVE_UINT_TYPE& size, U8* data, NATIVE_UINT_TYPE index);
    // Helper function to get the buffer index into the queue for particular
    // queue index.
    NATIVE_UINT_TYPE getBufferIndex(NATIVE_INT_TYPE index);

    // Member variables:
    void* queue; // The queue can be implemented in various ways
    NATIVE_UINT_TYPE msgSize; // Max size of message on the queue
    NATIVE_UINT_TYPE depth; // Max number of messages on the queue
    NATIVE_UINT_TYPE count; // Current number of messages on the queue
    NATIVE_UINT_TYPE maxCount; // Maximum number of messages ever seen on the queue
  };
}

#endif // OS_PTHREADS_BUFFER_QUEUE_HPP
