// ======================================================================
// \title  PriorityBufferQueue.hpp
// \author dinkel
// \brief  An implementation of BufferQueue which uses a stable max heap
//         data structure for the queue. Items of highest priority will
//         be popped off of the queue first. Items of equal priority will
//         be popped off the queue in FIFO order.
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Os/Pthreads/BufferQueue.hpp"
#include "Os/Pthreads/MaxHeap/MaxHeap.hpp"
#include <Fw/Types/Assert.hpp>
#include <string.h>
#include <stdio.h>

// This is a priority queue implementation implemented using a stable max heap.
// Elements pushed onto the queue will be popped off in priority order.
// Elements of the same priority will be popped off in FIFO order.
namespace Os {

  /////////////////////////////////////////////////////
  // Queue handler:
  /////////////////////////////////////////////////////

  struct PriorityQueue {
    MaxHeap* heap;
    U8* data;
    NATIVE_UINT_TYPE* indexes;
    NATIVE_UINT_TYPE startIndex;
    NATIVE_UINT_TYPE stopIndex;
  };

  /////////////////////////////////////////////////////
  // Helper functions:
  /////////////////////////////////////////////////////

  NATIVE_UINT_TYPE checkoutIndex(PriorityQueue* pQueue, NATIVE_UINT_TYPE depth) {
    NATIVE_UINT_TYPE* indexes = pQueue->indexes;

    // Get an available index from the index pool:
    NATIVE_UINT_TYPE index = indexes[pQueue->startIndex % depth];
    ++pQueue->startIndex;
    NATIVE_UINT_TYPE diff = pQueue->stopIndex - pQueue->startIndex;
    FW_ASSERT(diff <= depth, diff, depth, pQueue->stopIndex, pQueue->startIndex);
    return index;
  }

  void returnIndex(PriorityQueue* pQueue, NATIVE_UINT_TYPE depth, NATIVE_UINT_TYPE index) {
    NATIVE_UINT_TYPE* indexes = pQueue->indexes;

    // Return the index back to the index pool:
    indexes[pQueue->stopIndex % depth] = index;
    ++pQueue->stopIndex;
    NATIVE_UINT_TYPE diff = pQueue->stopIndex - pQueue->startIndex;
    FW_ASSERT(diff <= depth, diff, depth, pQueue->stopIndex, pQueue->startIndex);
  }

  /////////////////////////////////////////////////////
  // Class functions:
  /////////////////////////////////////////////////////

  bool BufferQueue::initialize(NATIVE_UINT_TYPE depth, NATIVE_UINT_TYPE msgSize) {
    // Create the priority queue data structure on the heap:
    MaxHeap* heap = new MaxHeap;
    if (NULL == heap) {
      return false;
    }
    if( !heap->create(depth) ) {
      return false;
    }
    U8* data = new U8[depth*(sizeof(msgSize) + msgSize)];  
    if (NULL == data) {
      return false;
    }
    NATIVE_UINT_TYPE* indexes = new NATIVE_UINT_TYPE[depth];
    if (NULL == indexes) {
      return false;
    }
    for(NATIVE_UINT_TYPE ii = 0; ii < depth; ++ii) {
        indexes[ii] = getBufferIndex(ii);
    }
    PriorityQueue* priorityQueue = new PriorityQueue;
    if (NULL == priorityQueue) {
      return false;
    }
    priorityQueue->heap = heap;
    priorityQueue->data = data;
    priorityQueue->indexes = indexes;
    priorityQueue->startIndex = 0;
    priorityQueue->stopIndex = depth;
    this->queue = priorityQueue;
    return true;
  }

  void BufferQueue::finalize() {
    PriorityQueue* pQueue = static_cast<PriorityQueue*>(this->queue);
    if (NULL != pQueue)
    {
      MaxHeap* heap = pQueue->heap;
      if (NULL != heap) {
        delete heap; 
      }
      U8* data = pQueue->data;
      if (NULL != data) {
        delete [] data; 
      }
      NATIVE_UINT_TYPE* indexes = pQueue->indexes;
      if (NULL != indexes)
      {
        delete [] indexes; 
      }
      delete pQueue; 
    }
    this->queue = NULL;
  }

  bool BufferQueue::enqueue(const U8* buffer, NATIVE_UINT_TYPE size, NATIVE_INT_TYPE priority) {

    // Extract queue handle variables:
    PriorityQueue* pQueue = static_cast<PriorityQueue*>(this->queue);
    MaxHeap* heap = pQueue->heap;
    U8* data = pQueue->data;

    // Get an available data index:
    NATIVE_UINT_TYPE index = checkoutIndex(pQueue, this->depth);
    
    // Insert the data into the heap:
    bool ret = heap->push(priority, index);
    FW_ASSERT(ret, ret);

    // Store the buffer to the queue:
    this->enqueueBuffer(buffer, size, data, index);

    return true;
  }
 
  bool BufferQueue::dequeue(U8* buffer, NATIVE_UINT_TYPE& size, NATIVE_INT_TYPE &priority) {

    // Extract queue handle variables:
    PriorityQueue* pQueue = static_cast<PriorityQueue*>(this->queue);
    MaxHeap* heap = pQueue->heap;
    U8* data = pQueue->data;

    // Get the highest priority data from the heap:
    NATIVE_UINT_TYPE index = 0;
    bool ret = heap->pop(priority, index);
    FW_ASSERT(ret, ret);

    ret = this->dequeueBuffer(buffer, size, data, index);
    if(!ret) {
      // The dequeue failed, so push the popped
      // value back on the heap.
      ret = heap->push(priority, index);
      FW_ASSERT(ret, ret);
      return false;
    }

    // Return the index to the available indexes:
    returnIndex(pQueue, this->depth, index);

    return true;
  }
}
