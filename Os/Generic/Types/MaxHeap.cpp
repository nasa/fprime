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

#include "Os/Generic/Types/MaxHeap.hpp"
#include <FpConfig.hpp>
#include <Fw/Logger/Logger.hpp>
#include "Fw/Types/Assert.hpp"

#include <cstdio>
#include <new>

// Macros for traversing the heap:
#define LCHILD(x) (2 * x + 1)
#define RCHILD(x) (2 * x + 2)
#define PARENT(x) ((x - 1) / 2)

namespace Types {

MaxHeap::MaxHeap() {
    // Initialize the heap:
    this->m_capacity = 0;
    this->m_heap = nullptr;
    this->m_size = 0;
    this->m_order = 0;
}

MaxHeap::~MaxHeap() {
    delete[] this->m_heap;
    this->m_heap = nullptr;
}

bool MaxHeap::create(FwSizeType capacity) {
    FW_ASSERT(this->m_heap == nullptr);
    // Loop bounds will overflow if capacity set to the max allowable value
    FW_ASSERT(capacity < std::numeric_limits<FwSizeType>::max());
    this->m_heap = new (std::nothrow) Node[capacity];
    if (nullptr == this->m_heap) {
        return false;
    }
    this->m_capacity = capacity;
    return true;
}

bool MaxHeap::push(FwQueuePriorityType value, FwSizeType id) {
    // If the queue is full, return false:
    if (this->isFull()) {
        return false;
    }

    // Heap indexes:
    FwSizeType parent;
    FwSizeType index = this->m_size;

    // Max loop bounds for bit flip protection:
    const FwSizeType maxIter = this->m_size + 1;
    FW_ASSERT(maxIter != 0);
    // Start at the bottom of the heap and work our ways
    // upwards until we find a parent that has a value
    // greater than ours.
    FwSizeType i = 0;
    for (i = 0; (i < maxIter) && (index != 0); i++) {
        // Get the parent index:
        parent = PARENT(index);
        // The parent index should ALWAYS be less than the
        // current index. Let's verify that.
        FW_ASSERT(parent < index, static_cast<FwAssertArgType>(parent), static_cast<FwAssertArgType>(index));
        // If the current value is less than the parent,
        // then the current index is in the correct place,
        // so break out of the loop:
        if (value <= this->m_heap[parent].value) {
            break;
        }
        // Swap the parent and child:
        this->m_heap[index] = this->m_heap[parent];
        index = parent;
    }

    // Check for programming errors or bit flips:
    FW_ASSERT(i < maxIter, static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(maxIter));
    FW_ASSERT(index <= this->m_size, static_cast<FwAssertArgType>(index));

    // Set the values of the new element:
    this->m_heap[index].value = value;
    this->m_heap[index].order = m_order;
    this->m_heap[index].id = id;

    ++this->m_size;
    ++this->m_order;
    return true;
}

bool MaxHeap::pop(FwQueuePriorityType& value, FwSizeType& id) {
    // If there is nothing in the heap then
    // return false:
    if (this->isEmpty()) {
        return false;
    }

    // Set the return values to the top (max) of
    // the heap:
    value = this->m_heap[0].value;
    id = this->m_heap[0].id;

    // Now place the last element on the heap in
    // the root position, and resize the heap.
    // This will put the smallest value in the
    // heap on the top, violating the heap property.
    FwSizeType index = this->m_size - 1;
    // Fw::Logger::log("Putting on top: i: %u v: %d\n", index, this->m_heap[index].value);
    this->m_heap[0] = this->m_heap[index];
    --this->m_size;

    // Now that the heap property is violated, we
    // need to reorganize the heap to restore it's
    // heapy-ness.
    this->heapify();
    return true;
}

// Is the heap full:
bool MaxHeap::isFull() {
    return (this->m_size == this->m_capacity);
}

// Is the heap empty:
bool MaxHeap::isEmpty() {
    return (this->m_size == 0);
}

// Get the current size of the heap:
FwSizeType MaxHeap::getSize() const {
    return this->m_size;
}

// A non-recursive heapify method.
// Note: This method had an additional property, such that
// items pushed of the same priority will be popped in FIFO
// order.
void MaxHeap::heapify() {
    FwSizeType index = 0;
    FwSizeType left;
    FwSizeType right;
    FwSizeType largest;

    // Max loop bounds for bit flip protection:
    const FwSizeType maxIter = this->m_size + 1;
    FwSizeType i = 0;

    for (i = 0; (i < maxIter) && (index <= this->m_size); i++) {
        // Get the children indexes for this node:
        left = LCHILD(index);
        right = RCHILD(index);
        FW_ASSERT(left > index, static_cast<FwAssertArgType>(left), static_cast<FwAssertArgType>(index));
        FW_ASSERT(right > left, static_cast<FwAssertArgType>(right), static_cast<FwAssertArgType>(left));

        // If the left node is bigger than the heap
        // size, we have reached the end of the heap
        // so we can stop:
        if (left >= this->m_size) {
            break;
        }

        // Initialize the largest node to the current
        // node:
        largest = index;

        // Which one is larger, the current node or
        // the left node?:
        largest = this->max(left, largest);

        // Make sure the right node exists before checking it:
        if (right < this->m_size) {
            // Which one is larger, the current largest
            // node or the right node?
            largest = this->max(right, largest);
        }

        // If the largest node is the current node
        // then we are done heapifying:
        if (largest == index) {
            break;
        }

        // Swap the largest node with the current node:
        this->swap(index, largest);

        // Set the new index to whichever child was larger:
        index = largest;
    }

    // Check for programming errors or bit flips:
    FW_ASSERT(i < maxIter, static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(maxIter));
    FW_ASSERT(index <= this->m_size, static_cast<FwAssertArgType>(index));
}

// Return the maximum priority index between two nodes. If their
// priorities are equal, return the oldest to keep the heap stable
FwSizeType MaxHeap::max(FwSizeType a, FwSizeType b) {
    static_assert(not std::numeric_limits<FwSizeType>::is_signed, "FwSizeType must be unsigned");
    FW_ASSERT(a < this->m_size, static_cast<FwAssertArgType>(a), static_cast<FwAssertArgType>(this->m_size));
    FW_ASSERT(b < this->m_size, static_cast<FwAssertArgType>(b), static_cast<FwAssertArgType>(this->m_size));

    // Extract the priorities:
    FwQueuePriorityType aValue = this->m_heap[a].value;
    FwQueuePriorityType bValue = this->m_heap[b].value;

    // If the priorities are equal, the "larger" one will be
    // the "older" one as determined by order pushed on to the
    // heap. Using this secondary ordering technique makes the
    // heap stable (ie. FIFO for equal priority elements).
    // Note: We check this first, because it is the most common
    // case. Let's save as many ticks as we can...
    if (aValue == bValue) {
        FwSizeType aAge = this->m_order - this->m_heap[a].order;
        FwSizeType bAge = this->m_order - this->m_heap[b].order;
        if (aAge > bAge) {
            return a;
        }
        return b;
    }

    // Which priority is larger?:
    if (aValue > bValue) {
        return a;
    }
    // B is larger:
    return b;
}

// Swap two nodes in the heap:
void MaxHeap::swap(FwSizeType a, FwSizeType b) {
    FW_ASSERT(a < this->m_size, static_cast<FwAssertArgType>(a), static_cast<FwAssertArgType>(this->m_size));
    FW_ASSERT(b < this->m_size, static_cast<FwAssertArgType>(b), static_cast<FwAssertArgType>(this->m_size));
    Node temp = this->m_heap[a];
    this->m_heap[a] = this->m_heap[b];
    this->m_heap[b] = temp;
}

}  // namespace Types
