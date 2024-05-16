\page OsQueue Os::Queue (Pthreads implementation)
# Os::Queue (Pthreads implementation)

## 1 Introduction

The ISF `Os::Queue` provides a message queue interface for asynchronous ISF ports. Many operating systems provide
their own inter-process communication (IPC) queues (ie. [POSIX](http://man7.org/linux/man-pages/man7/mq_overview.7.html), VxWorks, [SysV](http://man7.org/linux/man-pages/man7/svipc.7.html)), which have traditionally been used as
the underlying implementation for `Os::Queue`. In contrast, this Pthreads implementation is hand coded C++ code
tailored to the ISF `Os::Queue` interface. Its only dependency on non-standard libraries is [`<pthread.h>`](http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread.h.html), which is 
used for access and control flow of the internal queue data structure (ie. [`pthread_mutex_lock`](http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_mutex_lock.html), [`pthread_cond_wait`](http://pubs.opengroup.org/onlinepubs/007908799/xsh/pthread_cond_wait.html), 
etc.). The Pthread implementation provides the following advantages over the operating system provided IPC queues:

- The Pthreads implementation is NOT an IPC queue. It is designed for message passing between threads in the same
memory address space. This means that it never touches the filesystem and has no persistent state after the
`Os::Queue` deconstructor has been called. ISF currently does not spread different threads of execution in different memory 
spaces on Darwin and Linux, so this is an acceptable solution.

- Memory for Pthreads queues are allocated on the heap at initialization. After initialization, the memory usage of the
queue remains static. The queue sizes are only limited by the amount of memory on your system. The queue size is NOT 
governed by arbitrary limits defined in kernel configurations, as is the case for POSIX and SysV queues.

- Since the Pthreads implementation is handwritten, ISF developers can change the interface and add new features
to support mission needs. In particular, queue usage statistics can be built in, such as high water marking, message
counts, etc.

The Pthread queues can be configured in one of two ways depending on mission needs: first-in-first-out (FIFO) or priority. 
The FIFO queue is implemented using a circular buffer with *O(log(1))* enqueue and dequeue time. The priority queue is
implemented using a stable maximum binary heap with *O(log(n))* enqueue and dequeue time. Note: A *stable* maximum binary heap 
has the property that items pulled off the queue are in order of decreasing priority. Items of equal priority are pulled off 
in FIFO order.

NOTE: POSIX queues use a dynamically sized [red-black tree](https://en.wikipedia.org/wiki/Red%E2%80%93black_tree) for the message queue data structure. This data structure also has an *O(log(n))* enqueue and dequeue time.

## 2 Requirements

Requirement | Description | Rationale | Verification Method
---- | ---- | ---- | ----
ISF-Q-001 | The Pthreads queue shall conform to the ISF `Os::Queue` interface. | Ensures the queue can be used for ISF components | Inspection
ISF-Q-002 | The Pthreads queue shall not use dynamic memory, except at initialization. | Conform to flight software guidelines. | Inspection
ISF-Q-003 | The Pthreads queue shall not use operating system parameters for size limits. | Ensure that the queue sizes can be changed by the user without changing kernel parameters. | Inspection

## 3 Design

### 3.1 Constants

The Pthreads queue uses the following constants, specified at instantiation:

1. <a name="msgSize">*Message Size*</a>: The maximum size of a message that can be stored in the queue.

2. <a name="depth">*Depth*</a>: The maximum number of messages that can stored in the queue.

3. <a name="block">*Block*</a>: Flag which specifies whether the queue should block when reading from an empty queue.

### 3.2 State

The queue maintains the following state:

1. <a name="num_queue">*Number of Queues*</a>: The current number of queues allocated in the system (static variable which starts at 0).

2. <a name="count">*Count*</a>: The current number of messages in the queue (starts at 0).

3. <a name="maxCount">*Maximum Count*</a>: The maximum number of messages ever seen in the queue since instantiation. This is the "high water mark" of the queue.

4. <a name="queue">*Queue*</a>: The queue data structure itself. The queue memory is allocated in a `U8*` buffer of size: ([Message Size](#msgSize) + `sizeof(NATIVE_UINT_TYPE)`) * [Depth](#depth)

5. <a name="heap">*Heap*</a> (for priority queue only): The stable maximum binary heap data structure which orders stored messages with respect to priority. The size of the heap in memory is: `sizeof(NATIVE_UINT_TYPE)` * 3 * [Depth](#depth).

### 3.3 Stable Maximum Binary Heap

A maximum [binary heap data structure](https://en.wikipedia.org/wiki/Binary_heap) is a binary tree with the following constraints:

1. It is complete, that is, all levels of the tree, except possibly the last one (deepest) are fully filled, and, if the last level of the tree is not complete, the nodes of that level are filled from left to right.

2. It satisfies the maximum heap property, that is, all nodes are either greater than or equal to each of its children.

The Pthreads heap implementation is also *stable*, which means it satisfies the following additional constraint:

3. If a node has children nodes of equal value, the parent node must be "older" (inserted before) each equal-valued child node. 

This ensures that messages of equal priority are dequeued in FIFO order.

##4 Implementation

This section provides a summary of the code included in the C++ implementation files.

- **`Queue.cpp`:** This is the implementation of the ISF `Os::Queue` class. This file provides synchronization between threads reading and writing to the queue, and passes data to and from the underlying data structure.

- **`BufferQueue.hpp`:** This file outlines an interface to a generic queue data structure.

- **`FIFOBufferQueue.cpp`:** This file implements a first-in-first-out queue data structure, conforming to `BufferQueue.hpp`.

- **`PriorityBufferQueue.cpp`:** This file implements a priority queue data structure, conforming to `BufferQueue.hpp`. It uses files in MaxHeap/ to perform priority queueing.

- **`BufferQueueCommon.cpp`:** This file implements various common methods for `BufferQueue.hpp`.

- **`MaxHeap/MaxHeap.hpp`:** This file outlines an interface to a generic maximum heap data structure, where `NATIVE_INT_TYPE` is used for priority and a `NATIVE_UINT_TYPE` is stored as data.

- **`MaxHeap/MaxHeap.cpp`:** This file implements a stable maximum heap conforming to `MaxHeap/MaxHeap.hpp`.

Note: To use the FIFO queue implementation, a user must include `Queue.cpp`, `FIFOBufferQueue.cpp`, and `BufferQueueCommon.cpp` in the compilation. To use the priority queue implementation, the user must include `Queue.cpp`, `PriorityBufferQueue.cpp`, `BufferQueueCommon.cpp`, and `MaxHeap/MaxHeap.cpp`.

## 5 Unit Testing

There are 3 unit tests used to validate the Pthreads queue implementation at different levels of abstraction.

### 5.1 Maximum Heap Unit Test

The maximum heap unit tests are located in `Os/Pthread/MaxHeap/test/ut`. These tests validate the functionality of the maximum heap data structure. Test names and descriptions are listed below:

1. **Empty Heap**: Make sure that removing an item from an empty heap returns an error.

2. **Full Heap**: Make sure that adding an item to a full heap returns an error.

3. **Pop**: Test removing an item from a filled heap.

4. **Random Push/Pop**: Test adding and removing items of varied priority and ensure that they are returned off the heap in the correct order.

5. **Equal Priority**: Test that items added to the queue with equal priority are popped off in FIFO order.

6. **Mixed Priority**: Test that items added to the queue with both varied and equal priority are popped off in priority order and FIFO order when priorities are equal.

### 5.2 Buffer Queue Unit Test

The buffer queue unit tests are located in `Os/Pthread/test/ut`. These tests validate the functionality of the underlying queue data structure. Note: These tests do NOT test any blocking behavior of the queue. Test names and descriptions are listed below:

1. **Empty Queue**: Make sure that removing an item from an empty queue returns an error.

2. **Full Queue**: Make sure that adding an item to a full queue returns an error.

3. **Size Mismatch**: Make sure that trying to receive a large message into a too-small buffer returns an error.

4. **Pop**: Test removing an item from a filled queue.

5. **Priorities**: Ensure that the queue returns messages in priority order, and in FIFO order for equal priorities.

### 5.3 Queue Unit Test

The queue unit tests are located in `Os/test/ut`. These tests validate the functionality of the queue as well as the blocking behavior at the component interface level. Test names and descriptions are listed below:

#### 5.3.1 Blocking Queue Unit Test

1. **Send and Receive**: Test successful send and receive from a blocking queue.

2. **Blocking Receive on Empty**: Test blocking behavior for receiving on an empty queue.

3. **Non-blocking Send on Full**: Test non-blocking behavior for sending to a full queue.

4. **Send and Receive with Priorities**: Test that sending and receiving of messages occurs in the correct order with respect to priority.

#### 5.3.2 Non-blocking Queue Unit Test

1. **Send and Receive**: Test successful send and receive from a non-blocking queue.

2. **Non-blocking Receive on Empty**: Test non-blocking behavior for receiving on an empty queue.

3. **Non-blocking Send on Full**: Test non-blocking behavior for sending to a full queue.

4. **Send and Receive with Priorities**: Test that sending and receiving of messages occurs in the correct order with respect to priority.

#### 5.3.3 Queue Performance Unit Test

1. **Shallow Queue Test**: 3 messages are sent on the queue, and then 3 messages are read on the queue. This test is repeated 1000000 times in a single thread and the execution time is printed.

2. **Deep Queue Test**: The queue is first pre-filled to its maximum capacity (default 10). Next, test 1 is run 1000000 times and the execution time is printed.

3. **Concurrent Queue Test**: Test 2 is run simultaneously using 4 threads, 50000 times for each thread, and the execution time is printed.

## 6 Performance

The performance results for tests 5.3.3 for the Pthreads queue and the Posix queue were run on a Virtual Box VM (hosted on a Mac) and given 4 execution cores (April 2016). The test results can be seen below:

**Pthreads queue results:**
```
---------------------
- performance test --
---------------------
Testing shallow queue...
Time: 1.362s (1.362us per)
Testing deep queue...
Time: 1.769s (1.769us per)
Test complete.
---------------------
---------------------
---------------------
-- concurrent test --
---------------------
Testing deep queue...
Time: 0.882s (17.633us per)
Test complete.
---------------------
---------------------
```

**Posix queue results:**
```
---------------------
- performance test --
---------------------
Testing shallow queue...
Time: 3.993s (3.993us per)
Testing deep queue...
Time: 3.942s (3.942us per)
Test complete.
---------------------
---------------------
---------------------
-- concurrent test --
---------------------
Testing deep queue...
Time: 0.701s (14.017us per)
Test complete.
---------------------
---------------------
```

The Pthreads queues are significantly faster than posix queues on a single core. The Pthreads queue is marginally slower in a multi-threaded environment, but its performance is still very similar to the Posix queue implementation. Based on these results the Pthreads queue is performant enough for single core flight systems. It also looks to have reasonable performance when multi-threaded. If we wanted better multi-core performance, we could look into implementing a [lock-free concurrent max heap data structure](http://www.non-blocking.com/download/SunT03_PQueue_TR.pdf).
