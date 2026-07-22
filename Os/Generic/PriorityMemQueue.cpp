// ======================================================================
// \title  Os/Generic/PriorityMemQueue.cpp
// \author B. Duckett
// \brief  cpp file for AtomicQueue-based priority queue implementation for Os::Queue
//
// \copyright
// Copyright 2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================
#include "PriorityMemQueue.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>
#include "Fw/LanguageHelpers.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/MemAllocator.hpp"
#include "config/MemoryAllocatorTypeEnumAc.hpp"

namespace Os {
namespace Generic {

// Arbitrary (but small) limit to prevent infinite loops
// Don't expect a message queue depth to ever exceed three digits
constexpr U32 LOOP_GUARD_LIMIT = 2000;

// ======================================================================
// Static Configuration State (Global)
// ======================================================================
// THREAD-SAFETY: Static configuration is designed for single-threaded
// initialization at system startup (before any queues are created).
// configure() asserts if called multiple times. Individual queue
// creation uses atomic s_configsUsed[] to prevent race conditions when
// multiple components instantiate queues concurrently.
//
// LIFECYCLE:
// 1. System startup: Call configure() once (single-threaded)
// 2. Component init: create() claims config atomically (multi-threaded safe)
// 3. Runtime: No modification to static state
// 4. Teardown: Each queue marks config unused atomically
// 5. Test only: resetConfig() deallocates (single-threaded after all queues destroyed)
// ======================================================================
PriorityMemQueue::QueueConfig* PriorityMemQueue::s_configs = nullptr;
FwSizeType PriorityMemQueue::s_numConfigs = 0;
bool PriorityMemQueue::s_requirePrioritySizing = false;
std::atomic<bool>* PriorityMemQueue::s_configsUsed = nullptr;
bool PriorityMemQueue::s_configured = false;
FwEnumStoreType PriorityMemQueue::s_allocatorId = 0;

//! \brief Get the bit mask for a priority
//! \param priority: priority to get mask for
//! \return bit mask with the priority bit set
static constexpr U32 priorityBitMask(FwQueuePriorityType priority) {
    return 1U << priority;
}

void PriorityMemQueueHandle::init() {
    // NOTE: Do NOT reset m_priorityMap here - it's already populated by create()

    // If arrays were allocated, teardown AtomicQueues
    if (this->m_atomicQueues != nullptr) {
        for (FwSizeType i = 0; i < this->m_numActivePriorities; ++i) {
            this->m_atomicQueues[i].teardown();
        }
    }

    // Initialize high water marks to zero if array is allocated
    if (this->m_highWaterMarks != nullptr) {
        for (FwSizeType i = 0; i < this->m_numActivePriorities; ++i) {
            this->m_highWaterMarks[i].store(0, std::memory_order_relaxed);
        }
    }

    // Initialize the not-empty semaphore with count 0 (queue starts empty)
    if (this->m_notEmptySem != nullptr) {
        this->m_notEmptySem->~CountingSemaphore();
        this->m_notEmptySem = nullptr;
    }

    // Initialize atomic variables
    this->m_priorityMask.store(1U << Os::Generic::Queue::DEFAULT_PRIORITY, std::memory_order_relaxed);
}

bool PriorityMemQueueHandle::allocateArrays(Fw::MemAllocator& allocator, FwEnumStoreType allocatorId) {
    this->m_allocatorId = allocatorId;

    // Scan m_priorityMap to count configured priorities and find max priority
    this->m_numActivePriorities = 0;
    this->m_maxPriority = 0;

    for (FwSizeType p = 0; p < Os::Generic::Queue::MAX_PRIORITIES; ++p) {
        if (this->m_priorityMap[p] >= 0) {
            this->m_numActivePriorities++;
            if (static_cast<FwQueuePriorityType>(p) > this->m_maxPriority) {
                this->m_maxPriority = static_cast<FwQueuePriorityType>(p);
            }
        }
    }

    FW_ASSERT(this->m_numActivePriorities > 0, allocatorId);

    // Allocate memory for atomicQueues array (sized to actual configured priorities)
    FwSizeType atomicQueuesSize = sizeof(Types::AtomicQueue) * this->m_numActivePriorities;
    void* atomicQueuesMem = allocator.checkedAllocate(allocatorId, atomicQueuesSize, alignof(Types::AtomicQueue));
    if (atomicQueuesMem == nullptr) {
        this->deallocateArrays(allocator, allocatorId);
        return false;
    }
    // Use placement new to construct array
    this->m_atomicQueues = static_cast<Types::AtomicQueue*>(atomicQueuesMem);
    for (FwSizeType i = 0; i < this->m_numActivePriorities; ++i) {
        new (&this->m_atomicQueues[i]) Types::AtomicQueue();
    }

    // Allocate memory for highWaterMarks array (sized to actual configured priorities)
    FwSizeType hwmSize = sizeof(std::atomic<U32>) * this->m_numActivePriorities;
    void* hwmMem = allocator.checkedAllocate(allocatorId, hwmSize, alignof(std::atomic<U32>));
    if (hwmMem == nullptr) {
        this->deallocateArrays(allocator, allocatorId);
        return false;
    }
    // Cast required: MemAllocator returns void*, reinterpret_cast converts to typed pointer for placement new
    this->m_highWaterMarks = reinterpret_cast<std::atomic<U32>*>(hwmMem);
    for (FwSizeType i = 0; i < this->m_numActivePriorities; ++i) {
        // Placement new constructs std::atomic in pre-allocated memory (standard F' allocator pattern)
        new (&this->m_highWaterMarks[i]) std::atomic<U32>(0);
    }

    return true;
}

void PriorityMemQueueHandle::deallocateArrays(Fw::MemAllocator& allocator, FwEnumStoreType allocatorId) {
    // Deallocate arrays in reverse order
    if (this->m_highWaterMarks != nullptr) {
        // std::atomic<U32> is trivially destructible — no explicit destructor needed
        allocator.deallocate(allocatorId, this->m_highWaterMarks);
        this->m_highWaterMarks = nullptr;
    }
    if (this->m_atomicQueues != nullptr) {
        // Explicitly destroy AtomicQueues before deallocation
        for (FwSizeType i = 0; i < this->m_numActivePriorities; ++i) {
            this->m_atomicQueues[i].~AtomicQueue();
        }
        allocator.deallocate(allocatorId, this->m_atomicQueues);
        this->m_atomicQueues = nullptr;
    }

    // Reset priority map
    for (FwSizeType i = 0; i < Os::Generic::Queue::MAX_PRIORITIES; ++i) {
        this->m_priorityMap[i] = -1;
    }

    this->m_maxPriority = 0;
    this->m_numActivePriorities = 0;
    this->m_allocatorId = 0;
}

void PriorityMemQueueHandle::enablePriority(FwQueuePriorityType priority) {
    FW_ASSERT(priority < Os::Generic::Queue::MAX_PRIORITIES, priority, this->m_id);
    // Enabling a priority is only allowed if it's in use
    FW_ASSERT(this->m_atomicQueues != nullptr, this->m_id, priority);

    // MEMORY ORDERING: seq_cst for control path operations ensures total ordering
    // Atomic update of priority mask using fetch_or with seq_cst (control path)
    (void)this->m_priorityMask.fetch_or(priorityBitMask(priority), std::memory_order_seq_cst);
}

void PriorityMemQueueHandle::disablePriority(FwQueuePriorityType priority) {
    FW_ASSERT(priority < Os::Generic::Queue::MAX_PRIORITIES, this->m_id);

    // MEMORY ORDERING: seq_cst for control path operations ensures total ordering
    // Atomic update of priority mask using fetch_and with seq_cst (control path)
    (void)this->m_priorityMask.fetch_and(~priorityBitMask(priority), std::memory_order_seq_cst);
}

PriorityMemQueue::PriorityMemQueue() {
    // Initialize handle to safe defaults
    this->m_handle.init();
}

//! \brief Find most significant bit set (IPC-style priority finding)
//! \param value: bit mask to search
//! \return bit position of MSB, or -1 if no bits set
static inline I32 findMSB(U32 value) {
    if (value == 0) {
        return -1;
    }
    // Use compiler builtin for CLZ (count leading zeros) if available
#if defined(__GNUC__) || defined(__clang__)
    return 31 - __builtin_clz(value);
#else
    // Fallback: software implementation with explicit bound (32 bits maximum)
    I32 msb = 31;
    U32 mask = 0x80000000;
    for (FwSizeType bit = 0; bit < 32; ++bit) {
        if (mask & value) {
            return msb;
        }
        --msb;
        mask >>= 1;
    }
    return -1;
#endif
}

FwQueuePriorityType PriorityMemQueue::findHighestPriority(U32 priorities) {
    // MEMORY ORDERING: Use acquire to synchronize with priority enable/disable operations
    // Get enabled priorities
    if (priorities == 0) {
        priorities = this->m_handle.m_priorityMask.load(std::memory_order_acquire);
    }

    if (priorities == 0) {
        return Os::Generic::Queue::MAX_PRIORITIES;
    }

    // Use IPC-style MSB finding for performance
    I32 msb = findMSB(priorities);
    if (msb < 0 || msb >= static_cast<I32>(Os::Generic::Queue::MAX_PRIORITIES)) {
        return Os::Generic::Queue::MAX_PRIORITIES;
    }
    return static_cast<FwQueuePriorityType>(msb);
}

bool PriorityMemQueue::isPriorityEnabled(FwQueuePriorityType priority) {
    FW_ASSERT(priority < Os::Generic::Queue::MAX_PRIORITIES, this->m_handle.m_id);
    // MEMORY ORDERING: Use acquire to synchronize with enable/disable operations
    return (this->m_handle.m_priorityMask.load(std::memory_order_acquire) & priorityBitMask(priority)) != 0;
}

void PriorityMemQueue::setPriorityEnabled(FwQueuePriorityType priority, bool enabled) {
    // Delegate to handle methods (SSOT)
    if (enabled) {
        this->m_handle.enablePriority(priority);
    } else {
        this->m_handle.disablePriority(priority);
    }
}

Fw::MemAllocator& PriorityMemQueue::getAllocator() {
    return Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
        Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
}

//! \brief Validate queue configuration structures
//! \param queueConfigs: array of queue configurations to validate
//! \param numQueueConfigs: number of configurations
static void validateQueueConfigs(PriorityMemQueue::QueueConfig* queueConfigs, FwSizeType numQueueConfigs) {
    for (FwSizeType i = 0; i < numQueueConfigs; ++i) {
        PriorityMemQueue::QueueConfig* currentConfig = &queueConfigs[i];

        // Assert if numPriorities is 0 or exceeds maximum
        FW_ASSERT(
            currentConfig->numPriorities > 0 && currentConfig->numPriorities <= Os::Generic::Queue::MAX_PRIORITIES,
            static_cast<FwAssertArgType>(i), currentConfig->instanceId,
            static_cast<FwAssertArgType>(currentConfig->numPriorities));

        // Check for duplicate instance IDs
        for (FwSizeType j = i + 1; j < numQueueConfigs; ++j) {
            PriorityMemQueue::QueueConfig* otherConfig = &queueConfigs[j];
            FW_ASSERT(currentConfig->instanceId != otherConfig->instanceId, currentConfig->instanceId,
                      static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(j));
        }

        // Check priority configurations
        PriorityMemQueue::QueuePriorityConfig* priorityConfigs = currentConfig->priorityConfigs;
        FW_ASSERT(priorityConfigs != nullptr, static_cast<FwAssertArgType>(i), currentConfig->instanceId,
                  static_cast<FwAssertArgType>(currentConfig->numPriorities));
        for (FwSizeType p = 0; p < currentConfig->numPriorities; ++p) {
            PriorityMemQueue::QueuePriorityConfig* pConfig = &priorityConfigs[p];

            // Assert if maxMsgSize or numMsgs is 0
            FW_ASSERT(pConfig->maxMsgSize > 0, static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(p),
                      pConfig->priority);
            FW_ASSERT(pConfig->numMsgs > 0, static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(p),
                      pConfig->priority);
            FW_ASSERT(pConfig->priority >= 0 && pConfig->priority < Os::Generic::Queue::MAX_PRIORITIES,
                      static_cast<FwAssertArgType>(i), static_cast<FwAssertArgType>(p), pConfig->priority);

            // Check for duplicate priority values
            for (FwSizeType q = p + 1; q < currentConfig->numPriorities; ++q) {
                PriorityMemQueue::QueuePriorityConfig* qConfig = &priorityConfigs[q];
                FW_ASSERT(pConfig->priority != qConfig->priority, static_cast<FwAssertArgType>(i),
                          currentConfig->instanceId, pConfig->priority);
            }
        }
    }
}

void PriorityMemQueue::configure(QueueConfig* queueConfigs,
                                 FwSizeType numQueueConfigs,
                                 bool required,
                                 FwEnumStoreType allocatorId) {
    // Accept NULL pointer if and only if numQueueConfigs is 0
    FW_ASSERT((queueConfigs != nullptr) || (numQueueConfigs == 0), 0);

    // Assert if already configured - that's not a supported use case
    FW_ASSERT(!s_configured, 0);

    s_configured = true;

    // Assert if required is true but num priorities is 0
    FW_ASSERT(!(required && numQueueConfigs == 0), required, static_cast<FwAssertArgType>(numQueueConfigs));

    // Validate all configurations
    if (queueConfigs != nullptr) {
        validateQueueConfigs(queueConfigs, numQueueConfigs);
    }
    // Get the memory allocator configured for priority queues
    Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
        Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
    // Allocate memory for tracking used configurations
    if (numQueueConfigs > 0) {
        FwSizeType expSize = numQueueConfigs * sizeof(std::atomic<bool>);
        s_configsUsed = static_cast<std::atomic<bool>*>(
            allocator.checkedAllocate(allocatorId, expSize, alignof(std::atomic<bool>)));
        FW_ASSERT(s_configsUsed != nullptr);
        // Initialize all entries to false using placement new
        for (FwSizeType i = 0; i < numQueueConfigs; ++i) {
            new (&s_configsUsed[i]) std::atomic<bool>(false);
        }
    }

    // Deep copy into a single contiguous block: QueueConfig[] followed by all QueuePriorityConfig[] sub-arrays.
    static_assert(sizeof(QueueConfig) % alignof(QueuePriorityConfig) == 0,
                  "QueueConfig array must be naturally aligned with QueuePriorityConfig");
    if (numQueueConfigs > 0) {
        FwSizeType totalSize = numQueueConfigs * sizeof(QueueConfig);
        for (FwSizeType i = 0; i < numQueueConfigs; ++i) {
            totalSize += queueConfigs[i].numPriorities * sizeof(QueuePriorityConfig);
        }
        void* configsMem = allocator.checkedAllocate(allocatorId, totalSize, alignof(QueueConfig));
        FW_ASSERT(configsMem != nullptr);
        s_configs = static_cast<QueueConfig*>(configsMem);
        QueuePriorityConfig* priorityBase = reinterpret_cast<QueuePriorityConfig*>(s_configs + numQueueConfigs);
        for (FwSizeType i = 0; i < numQueueConfigs; ++i) {
            s_configs[i] = queueConfigs[i];
            FwSizeType priorityConfigsSize = queueConfigs[i].numPriorities * sizeof(QueuePriorityConfig);
            (void)memcpy(priorityBase, queueConfigs[i].priorityConfigs, priorityConfigsSize);
            s_configs[i].priorityConfigs = priorityBase;
            priorityBase += queueConfigs[i].numPriorities;
        }
    }
    s_numConfigs = numQueueConfigs;
    s_requirePrioritySizing = required;
    s_allocatorId = allocatorId;
}

void PriorityMemQueue::resetConfig() {
    // Only call this in test environments after all queues are destroyed
    if (s_configsUsed != nullptr || s_configs != nullptr) {
        // Get allocator (same as used in config())
        Fw::MemAllocator& allocator = Fw::MemAllocatorRegistry::getInstance().getAnAllocator(
            Fw::MemoryAllocation::MemoryAllocatorType::OS_GENERIC_PRIORITY_QUEUE);
        FwEnumStoreType allocatorId = s_allocatorId;

        // Deallocate the tracking array
        if (s_configsUsed != nullptr) {
            allocator.deallocate(allocatorId, s_configsUsed);
            s_configsUsed = nullptr;
        }

        // Deallocate the single contiguous config block (QueueConfig[] + all QueuePriorityConfig[] sub-arrays)
        if (s_configs != nullptr) {
            allocator.deallocate(allocatorId, s_configs);
        }
    }

    // Reset all static state
    s_configs = nullptr;
    s_numConfigs = 0;
    s_requirePrioritySizing = false;
    s_configured = false;
}

PriorityMemQueue::~PriorityMemQueue() {
    this->teardownInternal();
}

QueueInterface::Status PriorityMemQueue::create(FwEnumStoreType id,
                                                const Fw::ConstStringBase& name,
                                                FwSizeType depth,
                                                FwSizeType messageSize) {
    FW_ASSERT(depth > 0, id);
    FW_ASSERT(messageSize > 0, id);

    // Initialize the handle ID
    this->m_handle.m_id = id;

    // Get the memory allocator for queue operations
    Fw::MemAllocator& allocator = this->getAllocator();

    // Find a matching configuration if one exists
    QueueConfig* queueConfig = findMatchingConfig(id);

    // Build priority map from configuration
    if (queueConfig != nullptr) {
        // Map each configured priority to array index
        for (FwSizeType i = 0; i < queueConfig->numPriorities; ++i) {
            FwQueuePriorityType p = queueConfig->priorityConfigs[i].priority;
            FW_ASSERT(p < Os::Generic::Queue::MAX_PRIORITIES, id, static_cast<FwAssertArgType>(p),
                      Os::Generic::Queue::MAX_PRIORITIES);
            this->m_handle.m_priorityMap[p] = static_cast<I8>(i);
        }
    } else {
        // Default: single priority at DEFAULT_PRIORITY
        this->m_handle.m_priorityMap[Os::Generic::Queue::DEFAULT_PRIORITY] = 0;
    }

    // Allocate arrays for priority data (reads from m_priorityMap)
    if (!this->m_handle.allocateArrays(allocator, id)) {
        return Os::QueueInterface::Status::ALLOCATION_FAILED;
    }

    // Initialize the handle with allocated arrays
    this->m_handle.init();

    // Allocate and create the not-empty semaphore (initial count 0)
    FwSizeType semSize = sizeof(Os::CountingSemaphore);
    void* semMem = allocator.checkedAllocate(id, semSize, alignof(Os::CountingSemaphore));
    if (semMem == nullptr) {
        this->m_handle.deallocateArrays(allocator, id);
        return Os::QueueInterface::Status::ALLOCATION_FAILED;
    }
    this->m_handle.m_notEmptySem = new (semMem) Os::CountingSemaphore(static_cast<U32>(0));

    // Create the priority queues based on configuration
    if (queueConfig != nullptr) {
        // Use the found configuration to create multiple priority queues
        return createConfiguredQueues(queueConfig, allocator, id);
    } else {
        // No configuration found, create a single default priority queue
        return createDefaultQueue(depth, messageSize, allocator, id);
    }
}

// Helper method to find a matching configuration for the given ID
PriorityMemQueue::QueueConfig* PriorityMemQueue::findMatchingConfig(FwEnumStoreType id) {
    if (s_configs != nullptr && s_configsUsed != nullptr) {
        for (FwSizeType i = 0; i < s_numConfigs; ++i) {
            if (s_configs[i].instanceId == id) {
                // Atomic check-and-set to claim configuration
                bool expected = false;
                if (s_configsUsed[i].compare_exchange_strong(expected, true, std::memory_order_acq_rel)) {
                    return &s_configs[i];
                } else {
                    // Configuration already in use, assert failure
                    FW_ASSERT(false, id, s_configs[i].instanceId);
                }
            }
        }
    }
    return nullptr;
}

// Helper method to create queues based on configuration
QueueInterface::Status PriorityMemQueue::createConfiguredQueues(QueueConfig* queueConfig,
                                                                Fw::MemAllocator& allocator,
                                                                FwEnumStoreType allocatorId) {
    FW_ASSERT(queueConfig != nullptr, this->m_handle.m_id);
    FW_ASSERT(queueConfig->priorityConfigs != nullptr, this->m_handle.m_id,
              static_cast<FwAssertArgType>(queueConfig->numPriorities));

    for (FwSizeType i = 0; i < queueConfig->numPriorities; ++i) {
        const QueuePriorityConfig& priorityConfig = queueConfig->priorityConfigs[i];
        FwQueuePriorityType priority = priorityConfig.priority;

        // Create and initialize the priority queue
        QueueInterface::Status status = this->createPriorityQueue(priority, priorityConfig.maxMsgSize,
                                                                  priorityConfig.numMsgs, allocator, allocatorId);

        if (status != Os::QueueInterface::Status::OP_OK) {
            return status;
        }

        // Enable this priority
        this->setPriorityEnabled(priority, true);
    }

    return Os::QueueInterface::Status::OP_OK;
}

// Helper method to create a single default priority queue
QueueInterface::Status PriorityMemQueue::createDefaultQueue(FwSizeType depth,
                                                            FwSizeType messageSize,
                                                            Fw::MemAllocator& allocator,
                                                            FwEnumStoreType allocatorId) {
    // Create and initialize the default priority queue
    return this->createPriorityQueue(Os::Generic::Queue::DEFAULT_PRIORITY, messageSize, depth, allocator, allocatorId);
}

// Helper method to create a single priority queue using AtomicQueue
QueueInterface::Status PriorityMemQueue::createPriorityQueue(FwQueuePriorityType priority,
                                                             FwSizeType maxMsgSize,
                                                             FwSizeType numMsgs,
                                                             Fw::MemAllocator& allocator,
                                                             FwEnumStoreType allocatorId) {
    FW_ASSERT(this->m_handle.m_atomicQueues != nullptr, this->m_handle.m_id, priority);
    FW_ASSERT(priority < Os::Generic::Queue::MAX_PRIORITIES, this->m_handle.m_id, priority);
    FW_ASSERT(priority <= this->m_handle.m_maxPriority, this->m_handle.m_id, priority, this->m_handle.m_maxPriority);

    // Get array index for this priority
    I8 index = this->m_handle.getPriorityIndex(priority);
    FW_ASSERT(index >= 0, this->m_handle.m_id, priority);

    // Get pointer to the AtomicQueue at mapped index (already constructed in allocateArrays)
    Types::AtomicQueue* atomicQueue = &this->m_handle.m_atomicQueues[index];

    // Create the AtomicQueue with the specified parameters
    atomicQueue->create(numMsgs, maxMsgSize, allocator, allocatorId);

    // Check if creation was successful (both capacity and slots must be initialized)
    if (!atomicQueue->isCreated()) {
        this->teardownInternal();
        return Os::QueueInterface::Status::ALLOCATION_FAILED;
    }

    // Enable this priority
    this->setPriorityEnabled(priority, true);

    return Os::QueueInterface::Status::OP_OK;
}

void PriorityMemQueue::teardown() {
    this->teardownInternal();
}

void PriorityMemQueue::teardownInternal() {
    FW_ASSERT(this->m_handle.m_atomicQueues != nullptr || this->m_handle.m_maxPriority == 0, this->m_handle.m_id);

    // Teardown all AtomicQueues if arrays are allocated
    if (this->m_handle.m_atomicQueues != nullptr) {
        for (FwSizeType i = 0; i < this->m_handle.m_numActivePriorities; ++i) {
            this->m_handle.m_atomicQueues[i].teardown();
        }
    }

    // Delete the not-empty semaphore
    if (this->m_handle.m_notEmptySem != nullptr) {
        Fw::MemAllocator& allocator = this->getAllocator();
        FW_ASSERT(this->m_handle.m_allocatorId != 0 || this->m_handle.m_notEmptySem != nullptr, this->m_handle.m_id);
        this->m_handle.m_notEmptySem->~CountingSemaphore();
        allocator.deallocate(this->m_handle.m_allocatorId, this->m_handle.m_notEmptySem);
        this->m_handle.m_notEmptySem = nullptr;
    }

    // Reset handle state
    this->m_handle.m_priorityMask.store(1U << Os::Generic::Queue::DEFAULT_PRIORITY, std::memory_order_relaxed);

    // Deallocate arrays using stored allocator ID
    Fw::MemAllocator& allocator = this->getAllocator();
    this->m_handle.deallocateArrays(allocator, this->m_handle.m_allocatorId);

    // If we were using a configuration, mark it as unused
    FW_ASSERT(s_configs != nullptr || s_configsUsed == nullptr, this->m_handle.m_id);
    FW_ASSERT(s_configsUsed != nullptr || s_configs == nullptr, this->m_handle.m_id);

    if (s_configs != nullptr && s_configsUsed != nullptr) {
        for (FwSizeType i = 0; i < s_numConfigs; ++i) {
            if (s_configs[i].instanceId == this->m_handle.m_id && s_configsUsed[i].load()) {
                s_configsUsed[i].store(false);
                break;
            }
        }
    }
}

//! \brief Resolve priority to a valid AtomicQueue, fallback to DEFAULT if needed
//! \param handle: queue handle
//! \param priority: input/output priority (may be modified to DEFAULT)
//! \param queueId: queue ID for assertions
//! \param requirePrioritySizing: whether to assert on fallback
//! \return pointer to AtomicQueue or nullptr if uninitialized
static Types::AtomicQueue* resolvePriorityQueue(PriorityMemQueueHandle& handle,
                                                FwQueuePriorityType& priority,
                                                FwEnumStoreType queueId,
                                                bool requirePrioritySizing) {
    // Look up priority in sparse map
    I8 index = handle.getPriorityIndex(priority);

    // If priority not configured, fall back to default
    if (index < 0) {
        if (requirePrioritySizing) {
            FW_ASSERT(false, queueId, requirePrioritySizing, priority, handle.m_maxPriority);
        }
        priority = Os::Generic::Queue::DEFAULT_PRIORITY;
        index = handle.getPriorityIndex(priority);
        FW_ASSERT(index >= 0, queueId, priority);
    }

    // Get AtomicQueue at mapped index
    FW_ASSERT(index < static_cast<I8>(handle.m_numActivePriorities), queueId, static_cast<FwAssertArgType>(index),
              static_cast<FwAssertArgType>(handle.m_numActivePriorities));
    Types::AtomicQueue* atomicQueue = &handle.m_atomicQueues[index];
    FW_ASSERT(atomicQueue->isCreated(), queueId, priority);
    return atomicQueue;
}

//! \brief Update per-priority high water mark atomically
//! \param highWaterMarks: array of HWM atomics
//! \param index: array index (not priority value)
//! \param currentDepth: current queue depth
//! \param queueId: queue ID for assertions
static void updateHighWaterMark(std::atomic<U32>* highWaterMarks,
                                FwSizeType index,
                                U32 currentDepth,
                                FwEnumStoreType queueId) {
    FW_ASSERT(highWaterMarks != nullptr, queueId, static_cast<FwAssertArgType>(index));
    U32 prevMax = highWaterMarks[index].load(std::memory_order_acquire);
    // This is best effort, debug data only. So if retry count is exceeded, just give up
    constexpr U32 MAX_CAS_RETRIES = 100;
    for (U32 casRetries = 0; casRetries < MAX_CAS_RETRIES; ++casRetries) {
        if (currentDepth <= prevMax) {
            return;  // No update needed
        }
        if (highWaterMarks[index].compare_exchange_weak(prevMax, currentDepth, std::memory_order_release,
                                                        std::memory_order_acquire)) {
            return;  // Update succeeded
        }
    }
}

QueueInterface::Status PriorityMemQueue::send(const U8* buffer,
                                              FwSizeType size,
                                              FwQueuePriorityType priority,
                                              QueueInterface::BlockingType blockType) {
    // Validate input parameters
    FW_ASSERT(buffer != nullptr, this->m_handle.m_id, priority, blockType);
    FW_ASSERT(size > 0, this->m_handle.m_id, static_cast<FwAssertArgType>(size), priority);

    // Check if priority is valid
    if (priority >= Os::Generic::Queue::MAX_PRIORITIES) {
        return QueueInterface::Status::INVALID_PRIORITY;
    }

    // Check if the queue is initialized
    if (this->m_handle.m_atomicQueues == nullptr) {
        return QueueInterface::Status::UNINITIALIZED;
    }

    // Resolve priority to valid queue
    Types::AtomicQueue* atomicQueue =
        resolvePriorityQueue(this->m_handle, priority, this->m_handle.m_id, s_requirePrioritySizing);

    // Check for sizing problem
    if (size > atomicQueue->getBufferSize()) {
        return QueueInterface::Status::SIZE_MISMATCH;
    }

    // Send message using AtomicQueue
    bool success;
    if (blockType == QueueInterface::BlockingType::BLOCKING) {
        success = atomicQueue->enqueueBlocking(buffer, size, true);
    } else {
        success = atomicQueue->enqueue(buffer, size);
    }

    if (!success) {
        return QueueInterface::Status::FULL;
    }

    // Update per-priority high water mark (use array index, not priority value)
    I8 index = this->m_handle.getPriorityIndex(priority);
    FW_ASSERT(index >= 0, this->m_handle.m_id, priority);
    U32 currentDepth = static_cast<U32>(atomicQueue->getSize());
    updateHighWaterMark(this->m_handle.m_highWaterMarks, static_cast<FwSizeType>(index), currentDepth,
                        this->m_handle.m_id);

    // Post semaphore to wake up receiver (if any)
    FW_ASSERT(this->m_handle.m_notEmptySem != nullptr, this->m_handle.m_id, priority);
    Os::CountingSemaphoreInterface::Status semStatus = this->m_handle.m_notEmptySem->post();
    FW_ASSERT(semStatus == Os::CountingSemaphoreInterface::Status::OP_OK, static_cast<FwAssertArgType>(semStatus));

    return QueueInterface::Status::OP_OK;
}

QueueInterface::Status PriorityMemQueue::receive(U8* destination,
                                                 FwSizeType capacity,
                                                 QueueInterface::BlockingType blockType,
                                                 FwSizeType& actualSize,
                                                 FwQueuePriorityType& priority) {
    // Validate input parameters
    FW_ASSERT(destination != nullptr, blockType, this->m_handle.m_id);

    // Check if the queue is initialized
    if (this->m_handle.m_atomicQueues == nullptr) {
        return QueueInterface::Status::UNINITIALIZED;
    }

    // Check if the blocking type is valid
    FW_ASSERT(
        blockType == QueueInterface::BlockingType::BLOCKING || blockType == QueueInterface::BlockingType::NONBLOCKING,
        blockType, this->m_handle.m_id);

    // RECEIVE FLOW: Scan all enabled priorities from highest to lowest.
    // For each enabled priority, use getSize() as a cheap pre-filter (2 relaxed loads).
    // Attempt dequeue only when getSize() > 0; dequeue CAS is the authoritative check.
    // If getSize() is transiently stale and dequeue() fails, continue to next priority.
    // Liveness is guaranteed by the semaphore — spurious wakes just re-scan.
    //
    // MEMORY ORDERING: acquire on priority mask ensures visibility of queue state.

    // Bounded loop with compile-time limit
    for (U32 reps = 0; reps < LOOP_GUARD_LIMIT; ++reps) {
        U32 enabledPriorities = this->m_handle.m_priorityMask.load(std::memory_order_acquire);

        for (I32 p = this->m_handle.m_maxPriority; p >= 0; --p) {
            FwQueuePriorityType testPriority = static_cast<FwQueuePriorityType>(p);
            if ((enabledPriorities & priorityBitMask(testPriority)) == 0) {
                continue;
            }
            // Look up priority in sparse map
            I8 index = this->m_handle.getPriorityIndex(testPriority);
            FW_ASSERT(index >= 0, index, testPriority, this->m_handle.m_id);
            FW_ASSERT(this->m_handle.m_atomicQueues != nullptr, this->m_handle.m_id, testPriority);
            Types::AtomicQueue* aq = &this->m_handle.m_atomicQueues[index];
            FW_ASSERT(aq->isCreated(), this->m_handle.m_id, testPriority);
            if (aq->getSize() == 0) {
                continue;
            }
            const bool dequeued = aq->dequeue(destination, capacity, actualSize);
            if (dequeued) {
                priority = testPriority;
                return QueueInterface::Status::OP_OK;
            }
        }

        // No message found
        if (blockType == QueueInterface::BlockingType::BLOCKING) {
            FW_ASSERT(this->m_handle.m_notEmptySem != nullptr, this->m_handle.m_id);
            Os::CountingSemaphoreInterface::Status semStatus = this->m_handle.m_notEmptySem->wait();
            FW_ASSERT(semStatus == Os::CountingSemaphoreInterface::Status::OP_OK,
                      static_cast<FwAssertArgType>(semStatus));
        } else {
            return QueueInterface::Status::EMPTY;
        }
    }

    // Should never reach here - loop guard prevents infinite loop
    FW_ASSERT(false, this->m_handle.m_id, LOOP_GUARD_LIMIT);
    return QueueInterface::Status::UNKNOWN_ERROR;
}

FwSizeType PriorityMemQueue::getMessagesAvailable() const {
    FwSizeType total = 0;

    if (this->m_handle.m_atomicQueues != nullptr) {
        for (FwSizeType i = 0; i < this->m_handle.m_numActivePriorities; ++i) {
            const Types::AtomicQueue* atomicQueue = &this->m_handle.m_atomicQueues[i];
            if (atomicQueue->isCreated()) {
                total += atomicQueue->getSize();
            }
        }
    }

    return total;
}

FwSizeType PriorityMemQueue::getMessageHighWaterMark() const {
    // Return the maximum high water mark across all priorities
    // MEMORY ORDERING: Use acquire to ensure visibility of latest HWM updates
    U32 maxHwm = 0;
    if (this->m_handle.m_highWaterMarks != nullptr) {
        for (FwSizeType i = 0; i < this->m_handle.m_numActivePriorities; ++i) {
            U32 hwm = this->m_handle.m_highWaterMarks[i].load(std::memory_order_acquire);
            if (hwm > maxHwm) {
                maxHwm = hwm;
            }
        }
    }
    return static_cast<FwSizeType>(maxHwm);
}

QueueHandle* PriorityMemQueue::getHandle() {
    return &this->m_handle;
}

}  // namespace Generic
}  // namespace Os
