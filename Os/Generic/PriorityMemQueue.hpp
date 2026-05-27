// ======================================================================
// \title  Os/Generic/PriorityMemQueue.hpp
// \author B. Duckett
// \brief  hpp file for AtomicQueue-based priority queue implementation for Os::Queue
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#include <atomic>
#include "Fw/Types/MemAllocator.hpp"
#include "Os/Queue.hpp"
#include "Os/CountingSemaphore.hpp"
#include "Os/Generic/Types/AtomicQueue.hpp"
#ifndef OS_GENERIC_PRIORITYMEMQUEUE_HPP
#define OS_GENERIC_PRIORITYMEMQUEUE_HPP

namespace Os {
namespace Generic {

// Constants
namespace Queue {
// JPL heritage implementation supported up to 32 priorities per message queue, limiting to 16 to reduce memory
// footprint
constexpr static FwSizeType MAX_PRIORITIES = 16;
constexpr static FwSizeType DEFAULT_PRIORITY = 0;
}  // namespace Queue

// Forward declarations
class PriorityMemQueue;

//! \brief critical data stored for priority queue
//!
//! The priority queue uses AtomicQueue for ISR-safe + SMP-safe operation.
//! Each priority has its own AtomicQueue. Priority tracking uses atomic
//! bitmasks and a counting semaphore for receive notification.
struct PriorityMemQueueHandle : public QueueHandle {
    // Per-priority AtomicQueues (dynamically allocated)
    Types::AtomicQueue* m_atomicQueues;  // Pointer to array of AtomicQueues
    FwQueuePriorityType m_maxPriority;   // Highest priority value (array size = maxPriority + 1)
    Os::CountingSemaphore* m_notEmptySem; // Counting semaphore signaling messages available
    FwEnumStoreType m_id;                // Queue identifier
    FwEnumStoreType m_allocatorId;       // Allocator ID for memory operations
    std::atomic<U32> m_priorityMask;     // Bit mask of enabled priorities
    std::atomic<U32> m_nonEmptyMask;     // Bit mask of priorities with messages (optimization)
    std::atomic<U32>* m_highWaterMarks;  // Pointer to array of per-priority high water marks

    //! \brief Constructor to initialize members
    PriorityMemQueueHandle()
        : m_atomicQueues(nullptr),
          m_maxPriority(0),
          m_notEmptySem(nullptr),
          m_highWaterMarks(nullptr) {}

    //! \brief Initialize the handle
    void init();

    //! \brief Allocate arrays for priority data
    //! \param allocator: memory allocator to use
    //! \param allocatorId: ID for memory allocation
    //! \param maxPriority: highest priority value to support (array size will be maxPriority + 1)
    //! \return true if successful, false otherwise
    bool allocateArrays(Fw::MemAllocator& allocator, FwEnumStoreType allocatorId, FwQueuePriorityType maxPriority);

    //! \brief Deallocate arrays for priority data
    //! \param allocator: memory allocator to use
    //! \param allocatorId: ID for memory deallocation
    void deallocateArrays(Fw::MemAllocator& allocator, FwEnumStoreType allocatorId);

    //! \brief Enable a specific priority (in the handle )
    //!
    //! \param priority: priority to enable
    void enablePriority(FwQueuePriorityType priority);

    //! \brief Disable a specific priority
    //!
    //! \param priority: priority to disable
    void disablePriority(FwQueuePriorityType priority);
};
//! \brief AtomicQueue-based priority queue implementation
//!
//! \warning This Priority Queue is ISR safe and SMP safe
//!
//! An OS-agnostic implementation of a priority queue using AtomicQueue for ISR-safe + SMP-safe operation.
//! Each priority has its own AtomicQueue. Uses counting semaphores for blocking operations.
//!
//! \warning allocates memory using MemAllocator

class PriorityMemQueue : public Os::QueueInterface {
  public:
    //! \brief Configuration for a priority level
    struct QueuePriorityConfig {
        FwQueuePriorityType priority;
        FwSizeType maxMsgSize;
        FwSizeType numMsgs;
    };

    //!\brief Configuration for a queue with multiple priorities
    struct QueueConfig {
        FwEnumStoreType instanceId;            // Per component creates pass in instance ID
        FwSizeType numPriorities;              // Number of priorities
        QueuePriorityConfig* priorityConfigs;  // Array of priority configurations
    };
    //! \brief Register per-priority sizes for component instances which do
    //! queueing with multiple priorities
    //! \warning This function must be called before any queues are created
    //! \param queueConfigs: configuration for the queue
    //! \param numQueueConfigs: number of queue configurations
    //! \param required: If true, fatal if a non-default priority is enqueued
    //! \param allocatorId: ID to use for memory allocation
    static void configure(QueueConfig* queueConfigs,
                          FwSizeType numQueueConfigs,
                          bool required,
                          FwEnumStoreType allocatorId);

    //! \brief Reset static configuration (test environments only)
    //!
    //! Deallocates internal config tracking memory and resets state.
    //! \warning Only call this in test harnesses after all queues are destroyed
    static void resetConfig();

  public:
    //! \brief queue interface constructor - initializes handle
    PriorityMemQueue();

    //! \brief default queue destructor
    virtual ~PriorityMemQueue();

    //! \brief copy constructor is forbidden
    PriorityMemQueue(const QueueInterface& other) = delete;

    //! \brief copy constructor is forbidden
    PriorityMemQueue(const QueueInterface* other) = delete;

    //! \brief assignment operator is forbidden
    PriorityMemQueue& operator=(const QueueInterface& other) override = delete;

    //! \brief create queue storage
    //!
    //! Creates a queue ensuring sufficient storage to hold `depth` messages of `messageSize` size each.
    //!
    //! \warning allocates memory through the memory allocator registry
    //!
    //! \param id: identifier for the queue, used for memory allocation
    //! \param name: name of queue
    //! \param depth: depth of queue in number of messages
    //! \param messageSize: size of an individual message
    //! \return: status of the creation
    Status create(FwEnumStoreType id,
                  const Fw::ConstStringBase& name,
                  FwSizeType depth,
                  FwSizeType messageSize) override;

    //! \brief teardown the queue
    //!
    //! Allow for queues to deallocate resources as part of system shutdown. This delegates to the underlying queue
    //! implementation.
    void teardown() override;

    //! \brief teardown the queue
    //!
    //! Allow for queues to deallocate resources as part of system shutdown. This delegates to the underlying queue
    //! implementation.
    //!
    //! Note: this is a helper to allow this to be called from the destructor.
    void teardownInternal();

    //! \brief send a message into the queue
    //!
    //! Send a message into the queue, providing the message data, size, priority, and blocking type. When
    //! `blockType` is set to BLOCKING, this call will block on queue full. Otherwise, this will return an error
    //! status on queue full.
    //!
    //! \warning It is invalid to send a null buffer
    //! \warning This method will block if the queue is full and blockType is set to BLOCKING
    //!
    //! \param buffer: message data
    //! \param size: size of message data
    //! \param priority: priority of the message
    //! \param blockType: BLOCKING to block for space or NONBLOCKING to return error when queue is full
    //! \return: status of the send
    Status send(const U8* buffer, FwSizeType size, FwQueuePriorityType priority, BlockingType blockType) override;

    //! \brief receive a message from the queue
    //!
    //! Receive a message from the queue, providing the message destination, capacity, priority, and blocking type.
    //! When `blockType` is set to BLOCKING, this call will block on queue empty. Otherwise, this will return an
    //! error status on queue empty. Actual size received and priority of message is set on success status.
    //!
    //! \warning It is invalid to send a null buffer
    //! \warning This method will block if the queue is empty and blockType is set to BLOCKING
    //!
    //! \param destination: destination for message data
    //! \param capacity: maximum size of message data
    //! \param blockType: BLOCKING to wait for message or NONBLOCKING to return error when queue is empty
    //! \param actualSize: (output) actual size of message read
    //! \param priority: (output) priority of message read
    //! \return: status of the send
    Status receive(U8* destination,
                   FwSizeType capacity,
                   BlockingType blockType,
                   FwSizeType& actualSize,
                   FwQueuePriorityType& priority) override;

    //! \brief get number of messages available
    //!
    //! \return number of messages available
    FwSizeType getMessagesAvailable() const override;

    //! \brief get maximum messages stored at any given time
    //!
    //! Returns the maximum number of messages in this queue at any given time. This is the high-water mark for this
    //! queue.
    //! \return queue message high-water mark
    FwSizeType getMessageHighWaterMark() const override;

    QueueHandle* getHandle() override;

    PriorityMemQueueHandle m_handle;

  private:
    //! \brief Static configuration storage
    //! \warning must be initialized by config
    static QueueConfig* s_configs;
    static FwSizeType s_numConfigs;
    static bool s_requirePrioritySizing;
    static std::atomic<bool>* s_configsUsed;
    static bool s_configured;

  private:
    //! \brief Find the highest priority with available messages
    //! \return highest priority with available messages, or MAX_PRIORITIES if none
    FwQueuePriorityType findHighestPriority(U32 priorities = 0);

    //! \brief Check if a priority is enabled
    //! \param priority: priority to check
    //! \return true if the priority is enabled, false otherwise
    bool isPriorityEnabled(FwQueuePriorityType priority);

    //! \brief Enable or disable a priority (internal)
    //! \param priority: priority to enable or disable
    //! \param enabled: true to enable, false to disable
    void setPriorityEnabled(FwQueuePriorityType priority, bool enabled);

    //! \brief Get the memory allocator for this queue
    //! \return reference to the memory allocator
    Fw::MemAllocator& getAllocator();

  private:
    //! \brief Find a matching configuration for the given ID
    //!
    //! \param id: identifier for the queue
    //! \return: pointer to the matching configuration, or nullptr if none found
    QueueConfig* findMatchingConfig(FwEnumStoreType id);

    //! \brief Create queues based on configuration
    //!
    //! \param queueConfig: configuration for the queue
    //! \param allocator: memory allocator to use
    //! \param allocatorId: ID to use for memory allocation
    //! \return: status of the creation
    QueueInterface::Status createConfiguredQueues(QueueConfig* queueConfig,
                                                  Fw::MemAllocator& allocator,
                                                  FwEnumStoreType allocatorId);

    //! \brief Create a single default priority queue
    //!
    //! \param depth: depth of queue in number of messages
    //! \param messageSize: size of an individual message
    //! \param allocator: memory allocator to use
    //! \param allocatorId: ID to use for memory allocation
    //! \return: status of the creation
    QueueInterface::Status createDefaultQueue(FwSizeType depth,
                                              FwSizeType messageSize,
                                              Fw::MemAllocator& allocator,
                                              FwEnumStoreType allocatorId);

    //! \brief Create a single priority queue
    //!
    //! \param priority: priority of the queue
    //! \param maxMsgSize: maximum size of a message
    //! \param numMsgs: number of messages to allocate space for
    //! \param allocator: memory allocator to use
    //! \param allocatorId: ID to use for memory allocation
    //! \return: status of the creation
    QueueInterface::Status createPriorityQueue(FwQueuePriorityType priority,
                                               FwSizeType maxMsgSize,
                                               FwSizeType numMsgs,
                                               Fw::MemAllocator& allocator,
                                               FwEnumStoreType allocatorId);
};
}  // namespace Generic
}  // namespace Os

#endif  // OS_GENERIC_PRIORITYMEMQUEUE_HPP
