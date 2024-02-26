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
#include <cstring>

namespace Os {

  /////////////////////////////////////////////////////
  // Class functions:
  /////////////////////////////////////////////////////

  BufferQueue::BufferQueue() {
    // Set member variables:
    this->m_queue = nullptr;
    this->m_msgSize = 0;
    this->m_depth = 0;
    this->m_count = 0;
    this->m_maxCount = 0;
  }

  BufferQueue::~BufferQueue() {
    this->finalize();
  }

  bool BufferQueue::create(NATIVE_UINT_TYPE depth, NATIVE_UINT_TYPE msgSize) {
    // Queue is already set up. destroy it and try again:
    if (nullptr != this->m_queue) {
      this->finalize();
    }
    FW_ASSERT(nullptr == this->m_queue, reinterpret_cast<POINTER_CAST>(this->m_queue));

    // Set member variables:
    this->m_msgSize = msgSize;
    this->m_depth = depth;
    return this->initialize(depth, msgSize);
  }

  bool BufferQueue::push(const U8* buffer, NATIVE_UINT_TYPE size, NATIVE_INT_TYPE priority) {

    FW_ASSERT(size <= this->m_msgSize);
    if( this->isFull() ) {
      return false;
    }

    // Enqueue the data:
    bool ret = enqueue(buffer, size, priority);
    if( !ret ) {
      return false;
    }

    // Increment count:
    ++this->m_count;
    if( this->m_count > this->m_maxCount ) {
      this->m_maxCount = this->m_count;
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
    --this->m_count;

    return true;
  }

  bool BufferQueue::isFull() {
    return (this->m_count == this->m_depth);
  }

  bool BufferQueue::isEmpty() {
    return (this->m_count == 0);
  }

  NATIVE_UINT_TYPE BufferQueue::getCount() {
    return this->m_count;
  }

  NATIVE_UINT_TYPE BufferQueue::getMaxCount() {
    return this->m_maxCount;
  }


  NATIVE_UINT_TYPE BufferQueue::getMsgSize() {
    return this->m_msgSize;
  }

  NATIVE_UINT_TYPE BufferQueue::getDepth() {
    return this->m_depth;
  }

  NATIVE_UINT_TYPE BufferQueue::getBufferIndex(NATIVE_INT_TYPE index) {
    return (index % this->m_depth) * (sizeof(NATIVE_INT_TYPE) + this->m_msgSize);
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
    // the size of the message:
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
