// ======================================================================
// \title  BufferQueueCommon.hpp
// \author dinkel
// \brief  This file implements some of the methods for the generic 
//         buffer queue data structure declared in BufferQueue.hpp that
//         are common amongst different queue implementations.
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Os/Pthreads/BufferQueue.hpp"
#include <Fw/Types/Assert.hpp>
#include <string.h>

namespace Os {

  /////////////////////////////////////////////////////
  // Class functions:
  /////////////////////////////////////////////////////

  BufferQueue::BufferQueue() {
    // Set member variables:
    this->queue = NULL;
    this->msgSize = 0;
    this->depth = 0;
    this->count = 0;
    this->maxCount = 0;
  }

  BufferQueue::~BufferQueue() {
    this->finalize();
  }

  bool BufferQueue::create(NATIVE_UINT_TYPE depth, NATIVE_UINT_TYPE msgSize) {
    // Queue is already set up. destroy it and try again:
    if (NULL != this->queue) {
      this->finalize();
    }
    FW_ASSERT(NULL == this->queue, (POINTER_CAST) this->queue);

    // Set member variables:
    this->msgSize = msgSize;
    this->depth = depth;
    return this->initialize(depth, msgSize);
  }

  bool BufferQueue::push(const U8* buffer, NATIVE_UINT_TYPE size, NATIVE_INT_TYPE priority) {

    FW_ASSERT(size <= this->msgSize);   
    if( this->isFull() ) {
      return false;
    }

    // Enqueue the data:
    bool ret = enqueue(buffer, size, priority);
    if( !ret ) {
      return false;
    }

    // Increment count:
    ++this->count;
    if( this->count > this->maxCount ) {
      this->maxCount = this->count;
    }
    return true;
  }
 
  bool BufferQueue::pop(U8* buffer, NATIVE_UINT_TYPE& size, NATIVE_INT_TYPE &priority) {

    if( this->isEmpty() ) {
      size = 0;
      return false;
    }

    // Dequeue the data:
    bool ret = dequeue(buffer, size, priority);
    if( !ret ) {
      return false;
    }
    
    // Decrement count:
    --this->count;

    return true;
  }
 
  bool BufferQueue::isFull() {
    return (this->count == this->depth);
  }
  
  bool BufferQueue::isEmpty() {
    return (this->count == 0);
  }

  NATIVE_UINT_TYPE BufferQueue::getCount() {
    return this->count;
  }

  NATIVE_UINT_TYPE BufferQueue::getMaxCount() {
    return this->maxCount;
  }


  NATIVE_UINT_TYPE BufferQueue::getMsgSize() { 
    return this->msgSize; 
  }

  NATIVE_UINT_TYPE BufferQueue::getDepth() { 
    return this->depth; 
  }

  NATIVE_UINT_TYPE BufferQueue::getBufferIndex(NATIVE_INT_TYPE index) {
    return (index % this->depth) * (sizeof(NATIVE_INT_TYPE) + this->msgSize);
  }

  void BufferQueue::enqueueBuffer(const U8* buffer, NATIVE_UINT_TYPE size, U8* data, NATIVE_UINT_TYPE index) {
    // Copy size of buffer onto queue:
    void* dest = &data[index];
    void* ptr = memcpy(dest, &size, sizeof(size));
    FW_ASSERT(ptr == dest);

    // Copy buffer onto queue:
    index += sizeof(size);
    dest = &data[index];
    ptr = memcpy(dest, buffer, size);
    FW_ASSERT(ptr == dest);
  }

  bool BufferQueue::dequeueBuffer(U8* buffer, NATIVE_UINT_TYPE& size, U8* data, NATIVE_UINT_TYPE index) {
    // Copy size of buffer from queue:
    NATIVE_UINT_TYPE storedSize;
    void* source = &data[index];
    void* ptr = memcpy(&storedSize, source, sizeof(size));
    FW_ASSERT(ptr == &storedSize);

    // If the buffer passed in is not big
    // enough, return false, and pass out
    // the size of the messsage:
    if(storedSize > size){
      size = storedSize;
      return false;
    }
    size = storedSize;

    // Copy buffer from queue:
    index += sizeof(size);
    source = &data[index];
    ptr = memcpy(buffer, source, storedSize);
    FW_ASSERT(ptr == buffer);
    return true;
  }
}
