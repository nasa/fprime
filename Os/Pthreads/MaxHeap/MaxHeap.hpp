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

#ifndef OS_PTHREADS_MAX_HEAP_HPP
#define OS_PTHREADS_MAX_HEAP_HPP

#include <FpConfig.hpp>

namespace Os {

  //! \class MaxHeap
  //! \brief A stable max heap data structure
  //!
  //! This is a max heap data structure. Items of the highest value will
  //! be popped off the heap first. Items of equal value will be popped
  //! off in FIFO order. Insertion and deletion from the heap are both
  //! O(log(n)) time.
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
    //!
    //! \param capacity the maximum number of elements to store in the heap
    //!
    bool create(NATIVE_UINT_TYPE capacity);
    //! \brief Push an item onto the heap.
    //!
    //! The item will be put into the heap according to its value. The
    //! id field is a data field set by the user which can be used to 
    //! identify the element when it is popped off the heap.
    //!
    //! \param value the value of the element to push onto the heap
    //! \param id the identifier of the element to push onto the heap
    //!
    bool push(NATIVE_INT_TYPE value, NATIVE_UINT_TYPE id);
    //! \brief Pop an item from the heap.
    //!
    //! The item with the maximum value in the heap will be returned.
    //! If there are items with equal values, the oldest item will be
    //! returned.
    //!
    //! \param value the value of the element to popped from the heap
    //! \param id the identifier of the element popped from the heap
    //!
    bool pop(NATIVE_INT_TYPE& value, NATIVE_UINT_TYPE& id);
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
    NATIVE_UINT_TYPE getSize();
    //! \brief Print the contents of the heap to stdout.
    //!
    //! This function here is for debugging purposes.
    //!
    void print();
  
    private:
    // Private functions:
    // Ensure the heap meets the heap property:
    void heapify();
    // Swap two elements on the heap:
    void swap(NATIVE_UINT_TYPE a, NATIVE_UINT_TYPE b);
    // Return the max between two elements on the heap:
    NATIVE_UINT_TYPE max(NATIVE_UINT_TYPE a, NATIVE_UINT_TYPE b);

    // The data structure for a node on the heap:
    struct Node {
      NATIVE_INT_TYPE value; // the priority of the node
      NATIVE_UINT_TYPE order; // order in which node was pushed
      NATIVE_UINT_TYPE id; // unique id for this node 
    };
  
    // Private members:
    Node* heap; // the heap itself
    NATIVE_UINT_TYPE size; // the current size of the heap
    NATIVE_UINT_TYPE order; // the current count of heap pushes
    NATIVE_UINT_TYPE capacity; // the maximum capacity of the heap
  };

}

#endif // OS_PTHREADS_MAX_HEAP_HPP
