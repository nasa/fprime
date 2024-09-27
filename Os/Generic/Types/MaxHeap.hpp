// ======================================================================
// \title  MaxHeap.hpp
// \author dinkel
// \brief  An implementation of a stable max heap data structure
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef UTILS_TYPES_MAX_HEAP_HPP
#define UTILS_TYPES_MAX_HEAP_HPP

#include <FpConfig.hpp>

namespace Types {

//! \class MaxHeap
//! \brief A stable max heap data structure
//!
//! This is a max heap data structure. Items of the highest value will
//! be popped off the heap first. Items of equal value will be popped
//! off in FIFO order. Insertion and deletion from the heap are both
//! O(log(n)) time.
//! \warning allocates memory on the heap
class MaxHeap {
  public:
    //! \brief MaxHeap constructor
    //!
    //! Create a max heap object
    //!
    MaxHeap();
    //! \brief MaxHeap deconstructor
    //!
    //! Free memory for the heap that was allocated in the constructor
    //!
    ~MaxHeap();
    //! \brief MaxHeap creation
    //!
    //! Create the max heap with a given maximum size
    //! \warning allocates memory on the heap
    //!
    //! \param capacity the maximum number of elements to store in the heap
    //!
    bool create(FwSizeType capacity);
    //! \brief Push an item onto the heap.
    //!
    //! The item will be put into the heap according to its value. The
    //! id field is a data field set by the user which can be used to
    //! identify the element when it is popped off the heap.
    //!
    //! \param value the value of the element to push onto the heap
    //! \param id the identifier of the element to push onto the heap
    //!
    bool push(FwQueuePriorityType value, FwSizeType id);
    //! \brief Pop an item from the heap.
    //!
    //! The item with the maximum value in the heap will be returned.
    //! If there are items with equal values, the oldest item will be
    //! returned.
    //!
    //! \param value the value of the element to popped from the heap
    //! \param id the identifier of the element popped from the heap
    //!
    bool pop(FwQueuePriorityType& value, FwSizeType& id);
    //! \brief Is the heap full?
    //!
    //! Has the heap reached max size. No new items can be put on the
    //! heap if this function returns true.
    //!
    bool isFull();
    //! \brief Is the heap empty?
    //!
    //! Is the heap empty? No item can be popped from the heap if
    //! this function returns true.
    //!
    bool isEmpty();
    //! \brief Get the current number of elements on the heap.
    //!
    //! This function returns the current number of items on the
    //! heap.
    //!
    FwSizeType getSize() const;

  private:
    // Private functions:
    // Ensure the heap meets the heap property:
    void heapify();
    // Swap two elements on the heap:
    void swap(FwSizeType a, FwSizeType b);
    // Return the max between two elements on the heap:
    FwSizeType max(FwSizeType a, FwSizeType b);

    // The data structure for a node on the heap:
    struct Node {
        FwQueuePriorityType value;  // the priority of the node
        FwSizeType order;           // order in which node was pushed
        FwSizeType id;              // unique id for this node
    };

    // Private members:
    Node* m_heap;           // the heap itself
    FwSizeType m_size;      // the current size of the heap
    FwSizeType m_order;     // the current count of heap pushes
    FwSizeType m_capacity;  // the maximum capacity of the heap
};

}  // namespace Types

#endif  // UTILS_TYPES_MAX_HEAP_HPP
