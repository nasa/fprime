# Fw/DataStructures: Basic Data Structures

`Fw/DataStructures` contains a library of basic data structures.
All the definitions in this directory are in the
namespace `Fw`.

The data structures defined here use the following concepts:

* **size:** The number of elements currently stored in a data structure.

* **capacity:** The maximum number of elements stored in a data structure.

For a fixed-size array, the size and the capacity are the same.
For other data structures, the size and the capacity are not
in general the same.
For example, at all times a map has a fixed capacity _C_ and a size between 0 
and _C_.

## 1. Arrays

An **array** _A_ stores _S_ elements for _S > 0_ at indices
0, 1, ..., _S - 1_.
The elements are stored in **backing memory** _M_.
An array provides bounds-checked access to the array elements
stored in _M_.

`Fw/DataStructures` provides the following array templates:

1. [`ExternalArray`](ExternalArray.md)

1. [`Array`](Array.md)

## 2. FIFO Queues

A **FIFO queue** is a data structure backed by an array.
It supports enqueue and dequeue operations in
first in first out (FIFO) order.

`Fw/DataStructures` provides the following FIFO queue templates:

1. [`ExternalFifoQueue`](ExternalFifoQueue.md)

1. [`FifoQueue`](FifoQueue.md)

## 3. Maps

### 3.1. Map

TODO

### 3.2. Array Map

TODO

### 3.3. AVL Tree Map

TODO

## 4. Sets

### 4.1. Set

TODO

### 4.2. Array Set

TODO

### 4.3. AVL Tree Set

TODO
