# Generic OSAL Implementations

This package provides generic implementations of various OSAL modules. These are implemented using generic data structures, other OSAL types, and C++ code. These specifically avoid calls down into any underlying operating system.

Available implementations:

1. [Os::PriorityQueue](#ospriorityqueue) - Heap-based priority queue implementation using Os::Mutex for thread safety
2. [Os::PriorityMemQueue](#osprioritymemqueue) - ISR-safe, heap-based priority queue with per-priority memory pools and configuration


## Os::PriorityQueue

Os::PriorityQueue is an in-memory implementation of Os::Queue. It allows projects that desire in-memory queue support to use Os::Queues.  Os::PriorityQueue allocates memory for its underlying data structures using `new (nothrow)` and deallocates memory with `delete`. These actions are taken during `create` and object destruction. This implies that Os::PriorityQueue should be instantiated and initialized during system initialization.

For memory protection, Os::PriorityQueue delegates to Os::Mutex and Os::ConditionVariable.

> [!WARNING]
> This Queue implementation is insufficient to be used for sending messages in ISR context due to the use of Os::Mutex as mentioned in above.

**Priority Ordering**: Larger priority numbers have higher priority than lower priority numbers (i.e., priority 0 is the lowest priority, there is no upper bound on priority value).

### Requirements

The requirements for `Os::Generic::PriorityQueue` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
PQ-001 | The PriorityQueue shall implement the Os::QueueInterface for compatibility with F´ components | Inspection, Unit Test
PQ-002 | The PriorityQueue shall support message prioritization with higher priority messages dequeued before lower priority messages | Unit Test
PQ-003 | The PriorityQueue shall allocate memory dynamically using new/delete during create and destruction | Inspection
PQ-004 | The PriorityQueue shall use a single shared memory pool for all message priorities | Inspection
PQ-005 | The PriorityQueue shall support blocking and non-blocking send operations | Unit Test
PQ-006 | The PriorityQueue shall support blocking and non-blocking receive operations | Unit Test
PQ-007 | The PriorityQueue shall use Os::Mutex for thread-safe access to queue data structures | Inspection
PQ-008 | The PriorityQueue shall use Os::ConditionVariable to implement blocking send/receive | Inspection, Unit Test
PQ-009 | The PriorityQueue shall track high water marks for message queue depth | Unit Test
PQ-010 | The PriorityQueue shall validate message sizes against the configured maximum | Unit Test
PQ-011 | The PriorityQueue shall use the F´ memory allocator registry for all dynamic allocations | Inspection
PQ-012 | The PriorityQueue shall return appropriate error codes for queue full, empty, and size mismatch conditions | Unit Test
PQ-013 | The PriorityQueue shall use a max-heap data structure for O(log n) priority ordering | Inspection
PQ-014 | The PriorityQueue shall NOT be ISR-safe due to mutex usage | Inspection

> [!NOTE]
> Os::PriorityQueue is simpler than Os::PriorityMemQueue but lacks ISR safety and per-priority configuration capabilities.

### Os::PriorityQueue Key Algorithms

Os::PriorityQueue stores messages in a set of dynamically allocated unordered parallel arrays. These arrays store: message data, and message data size respectively. There is also an index-free list that stores the indices that are available for storage in the fixed size arrays.

In order to prioritize messages, a [Types::MaxHeap](#typesmaxheap-data-structure) data structure is used.

When a message is received from a calling sender, `find_index` returns a free index from the free list. The data is copied into the message data array, and the size into the size array using this free index via `store_data`. The index is then inserted into the max heap structure for prioritization. When the queue is full and the `BLOCKING` option was supplied, the sender will block on the `m_full` condition variable until notified of a dequeue.

When a message is dequeued, the highest priority index is removed from the max heap. The data is copied out from the data array, and the size from the size array using that index via `load_data` and sent to the calling receiver. The index is then returned to the free list via `return_index` to indicate that it may be reused. When the queue is empty and the `BLOCKING` option was supplied, the receiver will block on the `m_empty` condition variable until notified of a enqueue.

If the queue is empty and data was received, the `m_empty` condition variable is notified to unblock waiting receivers. If the queue is full and data was dequeued, the `m_full` condition variable is notified to unblock waiting receivers.

### Types::MaxHeap Data Structure

The Types::MaxHeap data structure is used to prioritize a list of indices using the given priority. This heap uses a dynamically allocated maximum-length array to back a binary tree storage structure. The first element is the root of the tree, left children are calculated using `2x + 1` and right children using `2x + 2`. A node's parent is at `(x - 1)/2`.

When an index is pushed into this structure, it starts at the first unused element (first free child of some node) and iteratively swaps with its parent as long as the new nodes priority is larger than the parent. This ensures that the higher-priority elements are closer to the root of the tree. Specifically, the root *is* the highest priority element.

When an index is pulled from this structure, the root is removed as it is the highest priority data structure. The last leaf of the tree is elevated to the root and `heapify` is then called to restore the max-heap invariant of the data structure.

`heapify` starts at the newly ill-ordered root. It iteratively swaps this node with the highest-priority child until this node is the largest of the three (parent, left child, and right child) or until this node is swapped into a leaf position without children. The max-heap invariant is now restored.


## Os::PriorityMemQueue

Os::PriorityMemQueue is an ISR-safe and SMP-safe, priority-based memory queue implementation for F´ using VxWorks message queues (msgQ). This implementation leverages the VxWorks msgQ API to provide robust multi-core synchronization without custom spinlock management, eliminating interrupt starvation issues while maintaining ISR safety.

The key components are:

1. **VxWorks msgQ** - One msgQ per priority level for ISR-safe + SMP-safe message storage
2. **Atomic Priority Tracking** - Lock-free bitmasks to track which priorities have messages
3. **Counting Semaphore** - Notification mechanism for blocking receive operations
4. **PriorityMemQueue** - Main queue implementation that implements Os::QueueInterface

### Requirements

The requirements for `Os::Generic::PriorityMemQueue` are as follows:

Requirement | Description | Verification Method
----------- | ----------- | -------------------
PMQ-001 | The PriorityMemQueue shall implement the Os::QueueInterface for compatibility with F´ components | Inspection, Unit Test
PMQ-002 | The PriorityMemQueue shall support up to 16 priority levels | Inspection, Unit Test
PMQ-003 | The PriorityMemQueue shall allocate separate memory pools for each priority level | Inspection, Unit Test
PMQ-004 | The PriorityMemQueue shall support per-priority configuration of message size and depth | Inspection, Unit Test
PMQ-005 | The PriorityMemQueue shall support blocking and non-blocking send operations | Unit Test
PMQ-006 | The PriorityMemQueue shall support blocking and non-blocking receive operations | Unit Test
PMQ-007 | The PriorityMemQueue shall dequeue messages in priority order, with the highest enabled priority serviced first | Unit Test
PMQ-008 | The PriorityMemQueue shall support dynamic enable/disable of individual priority levels | Unit Test
PMQ-009 | The PriorityMemQueue shall be configurable to be ISR-safe for message enqueue and dequeue operations | Platform dependent
PMQ-010 | The PriorityMemQueue shall track high water marks for message queue depth | Unit Test
PMQ-011 | The PriorityMemQueue shall validate message sizes against priority-specific limits | Unit Test
PMQ-012 | The PriorityMemQueue shall use the F´ memory allocator registry for all dynamic allocations | Inspection
PMQ-013 | The PriorityMemQueue shall return appropriate error codes for queue full, empty, and invalid priority conditions | Unit Test
PMQ-014 | The PriorityMemQueue shall provide per-priority O(1) enqueue and dequeue operations | Inspection

> [!WARNING]
> Send/receive operations from ISR context must not use blocking behavior 

> [!NOTE]
> Os::PriorityMemQueue provides ISR safety and per-priority configuration at the cost of increased complexity and memory usage compared to Os::PriorityQueue.

### VxWorks msgQ-Based Architecture

#### Overview

`PriorityMemQueue` uses VxWorks message queues (`msgQLib`) for message storage and synchronization. This approach leverages Wind River's platform-optimized queue implementation that provides:
- **ISR Safety**: `msgQSend` can be called from interrupt context
- **SMP Safety**: Atomic operations prevent multi-core race conditions
- **No Interrupt Starvation**: msgQ uses task-level synchronization internally, not global interrupt disable
- **Platform Optimization**: Wind River has tuned msgQ performance for VxWorks SMP systems

#### Per-Priority Message Queues

Each priority level has its own dedicated VxWorks message queue created with `msgQCreate()`:

```cpp
MSG_Q_ID msgQ = msgQCreate(numMsgs, maxMsgSize, MSG_Q_FIFO);
```

**Parameters**:
- `numMsgs`: Maximum number of messages for this priority
- `maxMsgSize`: Maximum message size in bytes
- `MSG_Q_FIFO`: FIFO ordering within the priority level

**Storage**:
```cpp
struct PriorityMemQueueHandle {
    MSG_Q_ID m_msgQueues[MAX_PRIORITIES];   // VxWorks msgQ handles
    FwSizeType m_msgSizes[MAX_PRIORITIES];  // Max size per priority
    FwSizeType m_depths[MAX_PRIORITIES];    // Depth per priority
    ...
};
```

#### Priority Tracking with Atomics

To implement priority-based dequeue ordering, `PriorityMemQueue` maintains lock-free atomic bitmasks:

```cpp
std::atomic<U32> m_priorityMask;       // Which priorities are enabled (bit per priority)
std::atomic<U32> m_prioritiesWithMsg;  // Which priorities have messages
```

**Send Flow**:
1. Call `msgQSend()` - VxWorks handles synchronization
2. Atomically set bit: `m_prioritiesWithMsg.fetch_or(1 << priority)`
3. Post semaphore to wake receiver: `m_notEmpty.post()`

**Receive Flow**:
1. Read bitmask (lock-free): `priorities = m_prioritiesWithMsg & m_priorityMask`
2. Find highest priority bit set
3. Call `msgQReceive(msgQ[priority], ..., NO_WAIT)`
4. If queue now empty, atomically clear bit: `m_prioritiesWithMsg.fetch_and(~(1 << priority))`

This design minimizes synchronization overhead - only atomic operations on small integers, no spinlocks needed.

### PriorityMemQueue Overview

`PriorityMemQueue` is the main class that implements `Os::QueueInterface`. It provides a multi-priority message queue with configurable ISR-safe operations and flexible per-priority configuration.

**Priority Ordering**: Larger priority numbers have higher priority than lower priority numbers (i.e., priority 0 is the lowest priority, priority 15 is the highest).

**Key Features**:
- **Multi-Priority Support**: Up to 16 independent priority levels (0-15)
- **Per-Priority Memory Pools**: Each priority has dedicated buffer allocation 
- **ISR-Safe Operations**: Non-blocking send/receive can be called from interrupt context (platform-dependent)
- **Flexible Configuration**: Static configuration allows per-component, per-priority sizing
- **Priority Management**: Runtime enable/disable of individual priority levels
- **Blocking Support**: Optional blocking behavior for send and receive operations using counting semaphores
- **High Water Mark Tracking**: Monitors peak queue usage for system analysis

#### Synchronization

**PriorityMemQueue** uses a counting semaphore for blocking receive operations:
- **m_notEmpty**: Semaphore count represents number of messages available across all priorities
  - Initialized to 0 (queue starts empty)
  - `post()` called after successful enqueue to signal message availability
  - `wait()` blocks receiver when count reaches 0 (queue empty)
  - After `wait()` returns, receiver dequeues from highest priority with messages

A semaphore is used (as opposed to a condition variable & mutex) because:
- Semaphores are ISR safe (true for most RTOSs, including VxWorks, FreeRTOS, RTEMS, Greenhills Integrity, etc.)
- Simplifies synchronization 
    - No need for separate mutex acquisition before checking queue state
    - No double-check pattern required to prevent lost notifications
    - Semaphore count intrinsically tracks message availability

### Configuration 

The static configuration system allows per-component, per-priority queue sizing. This enables fine-grained memory allocation tailored to each component's messaging patterns.

If a configuration is not provided for a given queue instance ID, then create() will use the message max size and depth arguments for priority Os::Queue::DEFAULT_PRIORITY (0).

#### Using `priority_buffer_analyzer.py` for Message Size Calculation

F´ provides `priority_buffer_analyzer.py` to automatically calculate the maximum message sizes for each priority level based on the topology's port connections. This eliminates manual calculation and ensures correct buffer sizing.

**Script Location**: `${FPRIME_FRAMEWORK_PATH}/cmake/autocoder/scripts/priority_buffer_analyzer.py`

**Integration Steps**:

1. **Add CMake hook in Deployment CMakeLists.txt**:
   ```cmake
   # Add hook for port priority analyzer
   set(PRIORITY_BUFFER_HEADER_DIR "${CMAKE_BINARY_DIR}/${FPRIME_CURRENT_MODULE}/Top")
   set(PRIORITY_BUFFER_HEADER "${PRIORITY_BUFFER_HEADER_DIR}/PriorityBufferSizesAc.hpp")
   
   add_custom_command(
       OUTPUT "${PRIORITY_BUFFER_HEADER}"
       COMMAND ${CMAKE_COMMAND} -E make_directory "${PRIORITY_BUFFER_HEADER_DIR}"
       COMMAND ${PYTHON}
           "${FPRIME_FRAMEWORK_PATH}/cmake/autocoder/scripts/priority_buffer_analyzer.py"
           --build-dir "${CMAKE_BINARY_DIR}"
           --topology-path "${PRIORITY_BUFFER_HEADER_DIR}"
           --output "${PRIORITY_BUFFER_HEADER}"
       COMMENT "Generating PriorityBufferSizesAc.hpp for ${FPRIME_CURRENT_MODULE} deployment"
       VERBATIM
   )
   
   add_custom_target(priority_buffer_header DEPENDS "${PRIORITY_BUFFER_HEADER}")
   
   # Ensure priority buffer header is generated before building deployment
   add_dependencies(${FPRIME_CURRENT_MODULE} priority_buffer_header)
   ```

2. **Include generated header in Main.cpp**:
   ```cpp
   #include <Deployment/Top/PriorityBufferSizesAc.hpp>
   ```

3. **Use generated constants for configuration**:
   ```cpp
   // The script analyzes topology and generates constants per component/priority
   Os::Generic::PriorityMemQueue::QueuePriorityConfig cmdDispPriorityConfigs[] = {
       {Os::Generic::Queue::DEFAULT_PRIORITY, 
        PriorityBufferConfig::F_Prime_Svc_CmdDispatcher::PRIORITY_0, 10},
       {2, PriorityBufferConfig::F_Prime_Svc_CmdDispatcher::PRIORITY_2, 20},
       {3, PriorityBufferConfig::F_Prime_Svc_CmdDispatcher::PRIORITY_3, 20},
       {10, PriorityBufferConfig::F_Prime_Svc_CmdDispatcher::PRIORITY_10, 4},
   };
   
   Os::Generic::PriorityMemQueue::QueueConfig queueConfigs[] = {
       {Deployment::InstanceIds::CdhCore_cmdDisp, 
        FW_NUM_ARRAY_ELEMENTS(cmdDispPriorityConfigs), 
        &cmdDispPriorityConfigs[0]},
   };
   
   Os::Generic::PriorityMemQueue::configure(queueConfigs, 
                                            FW_NUM_ARRAY_ELEMENTS(queueConfigs), 
                                            false, 
                                            allocatorId);
   ```

**Generated Header Structure**:

The script generates `PriorityBufferSizesAc.hpp` with constants for each component and priority level:
```cpp
namespace PriorityBufferConfig {
    constexpr FwSizeType DATA_OFFSET = sizeof(FwEnumStoreType) + sizeof(FwIndexType);
    
    namespace F_Prime_Svc_CmdDispatcher {
        // Maximum size for priority 0 messages (Cmd port)
        static constexpr FwSizeType PRIORITY_0 = 
            Fw::CmdPortBuffer::CAPACITY + DATA_OFFSET;
        
        // Maximum size for priority 2 messages (CmdResponse port)
        static constexpr FwSizeType PRIORITY_2 = 
            Fw::CmdResponsePortBuffer::CAPACITY + DATA_OFFSET;
        // ... etc
    }
}
```

**How It Works**:
1. Script parses topology dictionaries to identify all port connections by priority
2. For each component and priority, finds maximum serialized size across all port types
3. Adds `DATA_OFFSET` (priority + port ID overhead) to each size
4. Generates namespaced constants for use in configuration

**Benefits**:
- Eliminates manual message size calculations
- Automatically accounts for port buffer overhead (`DATA_OFFSET`)
- Updates automatically when topology or port types change
- Reduces risk of undersized buffers causing runtime assertions 

### ISR Safety

The VxWorks msgQ-based implementation provides ISR safety through:

1. **msgQSend()**: ISR-safe by design in VxWorks
   - Can be called with `NO_WAIT` timeout from interrupt context
   - Uses atomic operations internally for SMP safety
   
2. **Atomic Bitmasks**: Lock-free priority tracking
   - `std::atomic<U32>` operations are compiler-intrinsic atomics
   - No interrupt disable or mutex needed
   
3. **msgQReceive()**: ISR-safe with `NO_WAIT` timeout
   - Non-blocking variant safe from interrupt context

**ISR Usage Pattern**:
```cpp
// From ISR context - use NONBLOCKING
queue.send(data, size, priority, QueueInterface::BlockingType::NONBLOCKING);
queue.receive(dest, capacity, QueueInterface::BlockingType::NONBLOCKING, size, pri);
```

> [!WARNING]
> **Blocking operations** (`BlockingType::BLOCKING`) use counting semaphores and **must NOT** be called from ISR context. VxWorks will assert if blocking is attempted in interrupt context.


