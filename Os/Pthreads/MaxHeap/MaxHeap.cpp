// ======================================================================
// \title  MaxHeap.cpp
// \author dinkel
// \brief  An implementation of a stable max heap data structure. Items
//         popped off the heap are guaranteed to be in order of decreasing
//         "value" (max removed first). Items of equal "value" will be
//         popped off in FIFO order. The performance of both push and pop
//         is O(log(n)).
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Os/Pthreads/MaxHeap/MaxHeap.hpp"
#include <FpConfig.hpp>
#include "Fw/Types/Assert.hpp"
#include <Fw/Logger/Logger.hpp>

#include <new>
#include <cstdio>

// Macros for traversing the heap:
#define LCHILD(x) (2 * x + 1)
#define RCHILD(x) (2 * x + 2)
#define PARENT(x) ((x - 1) / 2)

namespace Os {

    MaxHeap::MaxHeap() {
      // Initialize the heap:
      this->capacity = 0;
      this->heap = nullptr;
      this->size = 0;
      this->order = 0;
    }

    MaxHeap::~MaxHeap() {
      delete [] this->heap;
      this->heap = nullptr;
    }

    bool MaxHeap::create(NATIVE_UINT_TYPE capacity)
    {
      // The heap has already been created.. so delete
      // it and try again.
      if( nullptr != this->heap ) {
        delete [] this->heap;
        this->heap = nullptr;
      }

      this->heap = new(std::nothrow) Node[capacity];
      if( nullptr == this->heap ) {
        return false;
      }
      this->capacity = capacity;
      return true;
    }

    bool MaxHeap::push(NATIVE_INT_TYPE value, NATIVE_UINT_TYPE id) {
      // If the queue is full, return false:
      if(this->isFull()) {
        return false;
      }

      // Heap indexes:
      NATIVE_UINT_TYPE parent;
      NATIVE_UINT_TYPE index = this->size;

      // Max loop bounds for bit flip protection:
      NATIVE_UINT_TYPE maxIter = this->size+1;
      NATIVE_UINT_TYPE maxCount = 0;

      // Start at the bottom of the heap and work our ways
      // upwards until we find a parent that has a value
      // greater than ours.
      while(index && maxCount < maxIter) {
        // Get the parent index:
        parent = PARENT(index);
        // The parent index should ALWAYS be less than the
        // current index. Let's verify that.
        FW_ASSERT(parent < index, parent, index);
        // If the current value is less than the parent,
        // then the current index is in the correct place,
        // so break out of the loop:
        if(value <= this->heap[parent].value) {
          break;
        }
        // Swap the parent and child:
        this->heap[index] = this->heap[parent];
        index = parent;
        ++maxCount;
      }

      // Check for programming errors or bit flips:
      FW_ASSERT(maxCount < maxIter, maxCount, maxIter);
      FW_ASSERT(index <= this->size, index);

      // Set the values of the new element:
      this->heap[index].value = value;
      this->heap[index].order = order;
      this->heap[index].id = id;

      ++this->size;
      ++this->order;
      return true;
    }

    bool MaxHeap::pop(NATIVE_INT_TYPE& value, NATIVE_UINT_TYPE& id) {
      // If there is nothing in the heap then
      // return false:
      if(this->isEmpty()) {
        return false;
      }

      // Set the return values to the top (max) of
      // the heap:
      value = this->heap[0].value;
      id = this->heap[0].id;

      // Now place the last element on the heap in
      // the root position, and resize the heap.
      // This will put the smallest value in the
      // heap on the top, violating the heap property.
      NATIVE_UINT_TYPE index = this->size-1;
      // Fw::Logger::logMsg("Putting on top: i: %u v: %d\n", index, this->heap[index].value);
      this->heap[0]= this->heap[index];
      --this->size;

      // Now that the heap property is violated, we
      // need to reorganize the heap to restore it's
      // heapy-ness.
      this->heapify();
      return true;
    }

    // Is the heap full:
    bool MaxHeap::isFull() {
      return (this->size == this->capacity);
    }

    // Is the heap empty:
    bool MaxHeap::isEmpty() {
      return (this->size == 0);
    }

    // Get the current size of the heap:
    NATIVE_UINT_TYPE MaxHeap::getSize() {
      return this->size;
    }

    // A non-recursive heapify method.
    // Note: This method had an additional property, such that
    // items pushed of the same priority will be popped in FIFO
    // order.
    void MaxHeap::heapify() {
      NATIVE_UINT_TYPE index = 0;
      NATIVE_UINT_TYPE left;
      NATIVE_UINT_TYPE right;
      NATIVE_UINT_TYPE largest;

      // Max loop bounds for bit flip protection:
      NATIVE_UINT_TYPE maxIter = this->size+1;
      NATIVE_UINT_TYPE maxCount = 0;

      while(index <= this->size && maxCount < maxIter) {
        // Get the children indexes for this node:
        left = LCHILD(index);
        right = RCHILD(index);
        FW_ASSERT(left > index, left, index);
        FW_ASSERT(right > left, right, left);

        // If the left node is bigger than the heap
        // size, we have reached the end of the heap
        // so we can stop:
        if (left >= this->size) {
          break;
        }

        // Initialize the largest node to the current
        // node:
        largest = index;

        // Which one is larger, the current node or
        // the left node?:
        largest = this->max(left, largest);

        // Make sure the right node exists before checking it:
        if (right < this->size) {
          // Which one is larger, the current largest
          // node or the right node?
          largest = this->max(right, largest);
        }

        // If the largest node is the current node
        // then we are done heapifying:
        if (largest == index)
        {
          break;
        }

        // Swap the largest node with the current node:
        // Fw::Logger::logMsg("Swapping: i: %u v: %d with i: %u v: %d\n",
        //   index, this->heap[index].value,
        //   largest, this->heap[largest].value);
        this->swap(index, largest);

        // Set the new index to whichever child was larger:
        index = largest;
      }

      // Check for programming errors or bit flips:
      FW_ASSERT(maxCount < maxIter, maxCount, maxIter);
      FW_ASSERT(index <= this->size, index);
    }

    // Return the maximum priority index between two nodes. If their
    // priorities are equal, return the oldest to keep the heap stable
    NATIVE_UINT_TYPE MaxHeap::max(NATIVE_UINT_TYPE a, NATIVE_UINT_TYPE b) {
      FW_ASSERT(a < this->size, a, this->size);
      FW_ASSERT(b < this->size, b, this->size);

      // Extract the priorities:
      NATIVE_INT_TYPE aValue = this->heap[a].value;
      NATIVE_INT_TYPE bValue = this->heap[b].value;

      // If the priorities are equal, the "larger" one will be
      // the "older" one as determined by order pushed on to the
      // heap. Using this secondary ordering technique makes the
      // heap stable (ie. FIFO for equal priority elements).
      // Note: We check this first, because it is the most common
      // case. Let's save as many ticks as we can...
      if(aValue == bValue) {
        NATIVE_UINT_TYPE aAge = this->order - this->heap[a].order;
        NATIVE_UINT_TYPE bAge = this->order - this->heap[b].order;
        if(aAge > bAge) {
          return a;
        }
        return b;
      }

      // Which priority is larger?:
      if( aValue > bValue ) {
        return a;
      }
      // B is larger:
      return b;
    }

    // Swap two nodes in the heap:
    void MaxHeap::swap(NATIVE_UINT_TYPE a, NATIVE_UINT_TYPE b) {
      FW_ASSERT(a < this->size, a, this->size);
      FW_ASSERT(b < this->size, b, this->size);
      Node temp = this->heap[a];
      this->heap[a] = this->heap[b];
      this->heap[b] = temp;
    }

    // Print heap, for debugging purposes only:
    void MaxHeap::print() {
      NATIVE_UINT_TYPE index = 0;
      NATIVE_UINT_TYPE left;
      NATIVE_UINT_TYPE right;
      Fw::Logger::logMsg("Printing Heap of Size: %d\n", this->size);
      while(index < this->size) {
        left = LCHILD(index);
        right = RCHILD(index);

        if( left >= size && index == 0) {
          Fw::Logger::logMsg("i: %u v: %d d: %u -> (NULL, NULL)\n",
            index, this->heap[index].value, this->heap[index].id);
        }
        else if( right >= size && left < size ) {
          Fw::Logger::logMsg("i: %u v: %d d: %u -> (i: %u v: %d d: %u, NULL)\n",
            index, this->heap[index].value, this->heap[index].id,
            left, this->heap[left].value, this->heap[left].id);
        }
        else if( right < size && left < size ) {
          Fw::Logger::logMsg("i: %u v: %d d: %u -> (i: %u v: %d d: %u, i: %u v: %d d: %u)\n",
            index, this->heap[index].value, this->heap[index].id,
            left, this->heap[left].value,this->heap[left].id,
            right, this->heap[right].value, this->heap[right].id);
        }

        ++index;
      }
    }
}
