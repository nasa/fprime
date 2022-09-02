// ======================================================================
// \title  ComQueue.cpp
// \author vbai
// \brief  cpp file for ComQueue component implementation class
// 
// Purpose: Catches start up events and telemetry
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

ComQueue ::ComQueue(const char* const compName) : ComQueueComponentBase(compName), m_state(WAITING), m_allocator(nullptr), m_allocation(nullptr) {
    for (NATIVE_UINT_TYPE i = 0; i < totalSize; i++) {
        m_throttle[i] = false;
    }
}

ComQueue ::~ComQueue() {}

void ComQueue ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    ComQueueComponentBase::init(queueDepth, instance);
}

void ComQueue ::cleanup() {
    if (m_allocator != nullptr && m_allocation != nullptr) {
        m_allocator->deallocate(m_allocationId, m_allocation);
    }
}

// Setup of metadata of the queues
void ComQueue::configure(QueueConfigurationTable queueConfig, NATIVE_UINT_TYPE allocationId, Fw::MemAllocator& allocator) {
    NATIVE_UINT_TYPE currentIndex = 0;
    NATIVE_UINT_TYPE totalAllocation = 0;
    m_allocator = &allocator;
    m_allocationId = allocationId;

    // Note: Priority should range from 0 to totalSize
    for (NATIVE_UINT_TYPE currentPriority = 0; currentPriority < totalSize; currentPriority++) {
        for (NATIVE_UINT_TYPE j = 0; j < FW_NUM_ARRAY_ELEMENTS(queueConfig.entries); j++) {
            FW_ASSERT(queueConfig.entries[j].depth > 0, queueConfig.entries[j].depth, j); // Valid depth
            // ensure that priority has been set properly
            FW_ASSERT(queueConfig.entries[j].priority < totalSize, queueConfig.entries[j].priority, totalSize, j);

            if (currentPriority == queueConfig.entries[j].priority) {
                // Writing to the memory location of prioritized list
                // entry points to prioritized list at i
                QueueData& entry = m_prioritizedList[currentIndex];
                entry.priority = queueConfig.entries[j].priority;
                entry.depth = queueConfig.entries[j].depth;
                entry.index = j;
                entry.msgSize = (j < ComQueueComSize) ? sizeof(Fw::ComBuffer) : sizeof(Fw::Buffer);
                totalAllocation += entry.depth * entry.msgSize;
                currentIndex++;
            }
        }
    }
    // Allocate a single chunk of memory from the memory allocator, without caring about memory recovery
    bool recoverable = false;
    m_allocation = m_allocator->allocate(m_allocationId, totalAllocation, recoverable);
    NATIVE_UINT_TYPE allocationOffset = 0;

    // After loop: Prioritized list should contain the all the data we want to work with
    // Ensure that every entry in the prioritized list is initialized
    for (NATIVE_UINT_TYPE i = 0; i < totalSize; i++) {
        NATIVE_UINT_TYPE allocationSize = m_prioritizedList[i].depth * m_prioritizedList[i].msgSize;
        //  ensures that we have enough memory to work with
        FW_ASSERT(allocationSize >= (m_prioritizedList[i].depth * m_prioritizedList[i].msgSize));
        m_queues[m_prioritizedList[i].index].setup(reinterpret_cast<U8*>(m_allocation) + allocationOffset,
                                                   allocationSize,
                                                   m_prioritizedList[i].depth,
                                                   m_prioritizedList[i].msgSize);
        allocationOffset += allocationSize;
    }
    FW_ASSERT(allocationOffset == totalAllocation, allocationOffset, totalAllocation);
}
// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ComQueue::comQueueIn_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    // Enqueues the items in the combuffer onto queue set
    FW_ASSERT(portNum >= 0 && portNum < ComQueueComSize, portNum);
    Fw::SerializeStatus status =
        m_queues[portNum].enqueue(reinterpret_cast<const U8*>(&data), sizeof(Fw::ComBuffer));
    if (status == Fw::FW_SERIALIZE_NO_ROOM_LEFT && !m_throttle[portNum]) {
        this->log_WARNING_HI_QueueFull(QueueType::comQueue, portNum);
        m_throttle[portNum] = true;
    }
    // If already ready, processes for the purposes of sending
    if (m_state == READY) {
        processQueue();
    }
}

void ComQueue::buffQueueIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    const NATIVE_INT_TYPE queueNum = portNum + ComQueueComSize;
    // Enqueues the items in buff onto the queue set
    FW_ASSERT(portNum >= 0 && portNum < ComQueueBuffSize, portNum);
    FW_ASSERT(queueNum < totalSize);
    // use as constant
    Fw::SerializeStatus status =
        m_queues[queueNum].enqueue(reinterpret_cast<const U8*>(&fwBuffer), sizeof(Fw::Buffer));
    if (status == Fw::FW_SERIALIZE_NO_ROOM_LEFT && !m_throttle[queueNum]) {
        this->log_WARNING_HI_QueueFull(QueueType::buffQueue, portNum);
        m_throttle[queueNum] = true;
    }
    // If already ready, processes for the purposes of sending
    if (m_state == READY) {
        processQueue();
    }
}

void ComQueue::comStatusIn_handler(const NATIVE_INT_TYPE portNum, Svc::ComSendStatus& ComStatus) {
    switch (ComStatus.e) {
        case ComSendStatus::READY:
            if (m_state == RETRY) {
                retryQueue();
            } else {
                processQueue();
            }
            break;
        case ComSendStatus::FAIL:
            m_state = RETRY;
            break;
        default:
            FW_ASSERT(0, ComStatus.e);
            break;
    }
}

void ComQueue::run_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
    ComQueueDepth comQueueDepth;
    BuffQueueDepth buffQueueDepth;
    for (NATIVE_INT_TYPE i = 0; i < comQueueDepth.SIZE; i++) {
        comQueueDepth[i] = m_queues[i].get_high_water_mark();
        m_queues[i].clear_high_water_mark();
    }
    this->tlmWrite_comQueueDepth(comQueueDepth);

    for (NATIVE_INT_TYPE i = 0; i < buffQueueDepth.SIZE; i++) {
        buffQueueDepth[i] = m_queues[i + ComQueueComSize].get_high_water_mark();
        m_queues[i + ComQueueComSize].clear_high_water_mark();
    }
    this->tlmWrite_buffQueueDepth(buffQueueDepth);
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------
void ComQueue::sendComBuffer(Fw::ComBuffer& comBuffer) {
    FW_ASSERT(m_state != WAITING);
    m_comBufferMessage = comBuffer;
    this->comQueueSend_out(0, comBuffer, 0);
    m_state = WAITING;
}

void ComQueue::sendBuffer(Fw::Buffer& buffer) {
    FW_ASSERT(m_state != WAITING);
    m_bufferMessage = buffer;
    this->buffQueueSend_out(0, buffer);
    m_state = WAITING;
}
void ComQueue::retryQueue() {
    if (m_lastEntry.index < ComQueueComSize) {
        sendComBuffer(m_comBufferMessage);
    } else {
        sendBuffer(m_bufferMessage);
    }
}
// We work under the assumption that the metadata in prioritized list
// being fed into the component is already sorted
void ComQueue::processQueue() {
    NATIVE_UINT_TYPE i = 0;
    NATIVE_UINT_TYPE sendPriority = 0;
    m_state = READY;

    for (i = 0; i < totalSize; i++) {
        QueueData& entry = m_prioritizedList[i];
        Types::Queue& queue = m_queues[entry.index];

        // Continue onto next iteration if there is no items in the queue
        if (queue.getQueueSize() == 0) {
            continue;
        }

        sendPriority = entry.priority;

        // Store entry in order to differentiate which buffer to send out during retry
        m_lastEntry = entry;

        if (entry.index < ComQueueComSize) {
            Fw::ComBuffer comBuffer;
            queue.dequeue(reinterpret_cast<U8*>(&comBuffer), sizeof(comBuffer));
            sendComBuffer(comBuffer);
        } else {
            Fw::Buffer buffer;
            queue.dequeue(reinterpret_cast<U8*>(&buffer), sizeof(buffer));
            sendBuffer(buffer);
        }
        m_throttle[entry.index] = false;  // sent message, throttle is now clear
        break;
    }

    // Onto the next entry after we sent the current entry
    // No need for initialization continuing from when the previous loop completed
    // Round robin of priority bucket as long as we have sent something
    for (i++; i < totalSize && (m_prioritizedList[i].priority == sendPriority); i++) {
        QueueData temp = m_prioritizedList[i];
        m_prioritizedList[i] = m_prioritizedList[i - 1];
        m_prioritizedList[i - 1] = temp;
    }
}
}  // end namespace Svc
