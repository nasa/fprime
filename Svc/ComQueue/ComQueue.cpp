// ======================================================================
// \title  ComQueue.cpp
// \author vbai
// \brief  cpp file for ComQueue component implementation class
// ======================================================================

#include <Fw/Types/Assert.hpp>
#include <Svc/ComQueue/ComQueue.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ComQueue ::QueueConfigurationTable ::QueueConfigurationTable() {
    for (NATIVE_UINT_TYPE i = 0; i < FW_NUM_ARRAY_ELEMENTS(this->entries); i++) {
        this->entries[i].priority = 0;
        this->entries[i].depth = 0;
    }
}

ComQueue ::ComQueue(const char* const compName)
    : ComQueueComponentBase(compName),
      m_state(WAITING),
      m_allocationId(-1),
      m_allocator(nullptr),
      m_allocation(nullptr) {
    // Initialize throttles to "off"
    for (NATIVE_UINT_TYPE i = 0; i < TOTAL_PORT_COUNT; i++) {
        this->m_throttle[i] = false;
    }
}

ComQueue ::~ComQueue() {}

void ComQueue ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    ComQueueComponentBase::init(queueDepth, instance);
}

void ComQueue ::cleanup() {
    // Deallocate memory ignoring error conditions
    if ((this->m_allocator != nullptr) && (this->m_allocation != nullptr)) {
        this->m_allocator->deallocate(this->m_allocationId, this->m_allocation);
    }
}

void ComQueue::configure(QueueConfigurationTable queueConfig,
                         NATIVE_UINT_TYPE allocationId,
                         Fw::MemAllocator& allocator) {
    FwIndexType currentPriorityIndex = 0;
    NATIVE_UINT_TYPE totalAllocation = 0;

    // Store/initialize allocator members
    this->m_allocator = &allocator;
    this->m_allocationId = allocationId;
    this->m_allocation = nullptr;

    // Initializes the sorted queue metadata list in priority (sorted) order. This is accomplished by walking the
    // priority values in priority order from 0 to TOTAL_PORT_COUNT. At each priory value, the supplied queue
    // configuration table is walked and any entry matching the current priority values is used to add queue metadata to
    // the prioritized list. This results in priority-sorted queue metadata objects that index back into the unsorted
    // queue data structures.
    //
    // The total allocation size is tracked for passing to the allocation call and is a summation of
    // (depth * message size)  for each prioritized metadata object of (depth * message size)
    for (FwIndexType currentPriority = 0; currentPriority < TOTAL_PORT_COUNT; currentPriority++) {
        // Walk each queue configuration entry and add them into the prioritized metadata list when matching the current
        // priority value
        for (NATIVE_UINT_TYPE entryIndex = 0; entryIndex < FW_NUM_ARRAY_ELEMENTS(queueConfig.entries); entryIndex++) {
            // Check for valid configuration entry
            FW_ASSERT(queueConfig.entries[entryIndex].priority < TOTAL_PORT_COUNT,
                      queueConfig.entries[entryIndex].priority, TOTAL_PORT_COUNT, entryIndex);

            if (currentPriority == queueConfig.entries[entryIndex].priority) {
                // Set up the queue metadata object in order to track priority, depth, index into the queue list of the
                // backing queue object, and message size. Both index and message size are calculated where priority and
                // depth are copied from the configuration object.
                QueueMetadata& entry = this->m_prioritizedList[currentPriorityIndex];
                entry.priority = queueConfig.entries[entryIndex].priority;
                entry.depth = queueConfig.entries[entryIndex].depth;
                entry.index = entryIndex;
                // Message size is determined by the type of object being stored, which in turn is determined by the
                // index of the entry. Those lower than COM_PORT_COUNT are Fw::ComBuffers and those larger Fw::Buffer.
                entry.msgSize = (entryIndex < COM_PORT_COUNT) ? sizeof(Fw::ComBuffer) : sizeof(Fw::Buffer);
                totalAllocation += entry.depth * entry.msgSize;
                currentPriorityIndex++;
            }
        }
    }
    // Allocate a single chunk of memory from the memory allocator. Memory recover is neither needed nor used.
    bool recoverable = false;
    this->m_allocation = this->m_allocator->allocate(this->m_allocationId, totalAllocation, recoverable);

    // Each of the backing queue objects must be supplied memory to store the queued messages. These data regions are
    // sub-portions of the total allocated data. This memory is passed out by looping through each queue in prioritized
    // order and passing out the memory to each queue's setup method.
    FwSizeType allocationOffset = 0;
    for (FwIndexType i = 0; i < TOTAL_PORT_COUNT; i++) {
        // Get current queue's allocation size and safety check the values
        FwSizeType allocationSize = this->m_prioritizedList[i].depth * this->m_prioritizedList[i].msgSize;
        FW_ASSERT(this->m_prioritizedList[i].index < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(this->m_queues)),
                  this->m_prioritizedList[i].index);
        FW_ASSERT((allocationSize + allocationOffset) <= totalAllocation, allocationSize, allocationOffset,
                  totalAllocation);

        // Setup queue's memory allocation, depth, and message size. Setup is skipped for a depth 0 queue
        if (allocationSize > 0) {
            this->m_queues[this->m_prioritizedList[i].index].setup(
                reinterpret_cast<U8*>(this->m_allocation) + allocationOffset, allocationSize,
                this->m_prioritizedList[i].depth, this->m_prioritizedList[i].msgSize);
        }
        allocationOffset += allocationSize;
    }
    // Safety check that all memory was used as expected
    FW_ASSERT(allocationOffset == totalAllocation, allocationOffset, totalAllocation);
}
// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ComQueue::comQueueIn_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    // Ensure that the port number of comQueueIn is consistent with the expectation
    FW_ASSERT(portNum >= 0 && portNum < COM_PORT_COUNT, portNum);
    this->enqueue(portNum, QueueType::COM_QUEUE, reinterpret_cast<const U8*>(&data), sizeof(Fw::ComBuffer));
}

void ComQueue::buffQueueIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    const NATIVE_INT_TYPE queueNum = portNum + COM_PORT_COUNT;
    // Ensure that the port number of buffQueueIn is consistent with the expectation
    FW_ASSERT(portNum >= 0 && portNum < BUFFER_PORT_COUNT, portNum);
    FW_ASSERT(queueNum < TOTAL_PORT_COUNT);
    this->enqueue(queueNum, QueueType::BUFFER_QUEUE, reinterpret_cast<const U8*>(&fwBuffer), sizeof(Fw::Buffer));
}

void ComQueue::comStatusIn_handler(const NATIVE_INT_TYPE portNum, Fw::Success& condition) {
    switch (this->m_state) {
        // On success, the queue should be processed. On failure, the component should still wait.
        case WAITING:
            if (condition.e == Fw::Success::SUCCESS) {
                this->m_state = READY;
                this->processQueue();
                // A message may or may not be sent. Thus, READY or WAITING are acceptable final states.
                FW_ASSERT((this->m_state == WAITING || this->m_state == READY), this->m_state);
            } else {
                this->m_state = WAITING;
            }
            break;
        // Both READY and unknown states should not be possible at this point. To receive a status message we must be
        // one of the WAITING or RETRY states.
        default:
            FW_ASSERT(0, this->m_state);
            break;
    }
}

void ComQueue::run_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
    // Downlink the high-water marks for the Fw::ComBuffer array types
    ComQueueDepth comQueueDepth;
    for (FwSizeType i = 0; i < comQueueDepth.SIZE; i++) {
        comQueueDepth[i] = this->m_queues[i].get_high_water_mark();
        this->m_queues[i].clear_high_water_mark();
    }
    this->tlmWrite_comQueueDepth(comQueueDepth);

    // Downlink the high-water marks for the Fw::Buffer array types
    BuffQueueDepth buffQueueDepth;
    for (FwSizeType i = 0; i < buffQueueDepth.SIZE; i++) {
        buffQueueDepth[i] = this->m_queues[i + COM_PORT_COUNT].get_high_water_mark();
        this->m_queues[i + COM_PORT_COUNT].clear_high_water_mark();
    }
    this->tlmWrite_buffQueueDepth(buffQueueDepth);
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

void ComQueue::enqueue(const FwIndexType queueNum, QueueType queueType, const U8* data, const FwSizeType size) {
    // Enqueue the given message onto the matching queue. When no space is available then emit the queue overflow event,
    // set the appropriate throttle, and move on. Will assert if passed a message for a depth 0 queue.
    const FwSizeType expectedSize = (queueType == QueueType::COM_QUEUE) ? sizeof(Fw::ComBuffer) : sizeof(Fw::Buffer);
    const FwIndexType portNum = queueNum - ((queueType == QueueType::COM_QUEUE) ? 0 : COM_PORT_COUNT);
    FW_ASSERT(expectedSize == size, size, expectedSize);
    FW_ASSERT(portNum >= 0, portNum);
    Fw::SerializeStatus status = this->m_queues[queueNum].enqueue(data, size);
    if (status == Fw::FW_SERIALIZE_NO_ROOM_LEFT && !this->m_throttle[queueNum]) {
        this->log_WARNING_HI_QueueOverflow(queueType, portNum);
        this->m_throttle[queueNum] = true;
    }
    // When the component is already in READY state process the queue to send out the next available message immediately
    if (this->m_state == READY) {
        this->processQueue();
    }
}

void ComQueue::sendComBuffer(Fw::ComBuffer& comBuffer) {
    FW_ASSERT(this->m_state == READY);
    this->comQueueSend_out(0, comBuffer, 0);
    this->m_state = WAITING;
}

void ComQueue::sendBuffer(Fw::Buffer& buffer) {
    // Retry buffer expected to be cleared as we are either transferring ownership or have already deallocated it.
    FW_ASSERT(this->m_state == READY);
    this->buffQueueSend_out(0, buffer);
    this->m_state = WAITING;
}

void ComQueue::processQueue() {
    FwIndexType priorityIndex = 0;
    FwIndexType sendPriority = 0;
    // Check that we are in the appropriate state
    FW_ASSERT(this->m_state == READY);

    // Walk all the queues in priority order. Send the first message that is available in priority order. No balancing
    // is done within this loop.
    for (priorityIndex = 0; priorityIndex < TOTAL_PORT_COUNT; priorityIndex++) {
        QueueMetadata& entry = this->m_prioritizedList[priorityIndex];
        Types::Queue& queue = this->m_queues[entry.index];

        // Continue onto next prioritized queue if there is no items in the current queue
        if (queue.getQueueSize() == 0) {
            continue;
        }

        // Send out the message based on the type
        if (entry.index < COM_PORT_COUNT) {
            Fw::ComBuffer comBuffer;
            queue.dequeue(reinterpret_cast<U8*>(&comBuffer), sizeof(comBuffer));
            this->sendComBuffer(comBuffer);
        } else {
            Fw::Buffer buffer;
            queue.dequeue(reinterpret_cast<U8*>(&buffer), sizeof(buffer));
            this->sendBuffer(buffer);
        }

        // Update the throttle and the index that was just sent
        this->m_throttle[entry.index] = false;

        // Priority used in the next loop
        sendPriority = entry.priority;
        break;
    }

    // Starting on the priority entry after the one dispatched and continuing through the end of the set of entries that
    // share the same priority, rotate those entries such that the currently dispatched queue is last and the rest are
    // shifted up by one. This effectively round-robins the queues of the same priority.
    for (priorityIndex++;
         priorityIndex < TOTAL_PORT_COUNT && (this->m_prioritizedList[priorityIndex].priority == sendPriority);
         priorityIndex++) {
        // Swap the previous entry with this one.
        QueueMetadata temp = this->m_prioritizedList[priorityIndex];
        this->m_prioritizedList[priorityIndex] = this->m_prioritizedList[priorityIndex - 1];
        this->m_prioritizedList[priorityIndex - 1] = temp;
    }
}
}  // end namespace Svc
