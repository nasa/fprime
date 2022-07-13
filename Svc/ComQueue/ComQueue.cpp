// ======================================================================
// \title  ComQueue.cpp
// \author vbai
// \brief  cpp file for ComQueue component implementation class
// ======================================================================

// Purpose: Catches start up events and telemetry

#include <Fw/Types/Assert.hpp>
#include <Svc/ComQueue/ComQueue.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ComQueue ::ComQueue() : ComQueueComponentBase(nullptr) {}

ComQueue ::ComQueue(const char* const compName) : ComQueueComponentBase(compName) {}

void ComQueue ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    ComQueueComponentBase::init(queueDepth, instance);
}

ComQueue::~ComQueue() {}

// Setup of metadata of the queues
void ComQueue::configure(QueueConfiguration queueConfig[], NATIVE_UINT_TYPE configSize, Fw::MemAllocator allocator){
    FW_ASSERT(configSize == totalSize, configSize, totalSize);

    NATIVE_UINT_TYPE currentIndex = 0;

    // Note: Priority should range from 0 to totalsize
    for (NATIVE_UINT_TYPE currentPriority = 0; currentPriority < totalSize; currentPriority++){
        for (NATIVE_UINT_TYPE j = 0; j < configSize; j++){

            // ensure that priority has been set properly
            FW_ASSERT(queueConfig[j].priority < totalSize, queueConfig[j].priority, totalSize);

            if (currentPriority == queueConfig[j].priority){

                // Writing to the memory location of prioritzed list
                // entry points to prioritized list at i
                QueueData& entry = m_prioritizedList[currentIndex];
                entry.priority = queueConfig[j].priority;
                entry.depth = queueConfig[j].depth;
                entry.index = j;
                entry.msgSize = (j < ComQueueComSize) ? sizeof(Fw::ComBuffer) : sizeof(Fw::Buffer);
                currentIndex++;
            }
        }
    }

    // After loop: Prioritized list should contain the all the data we want to work with
    // Ensure that every entry in the prioritized list is initialized
    for(NATIVE_UINT_TYPE i = 0; i < totalSize; i++) {
        NATIVE_UINT_TYPE allocationSize = m_prioritizedList[i].depth*m_prioritizedList[i].msgSize;

        // We do not care if it is recoverable since the buffers are user provided
        bool recoverable = false;
        void* ptr = allocator.allocate(i, allocationSize, recoverable);

        //  ensures that we have enough memory to work with
        FW_ASSERT(allocationSize >= (m_prioritizedList[i].depth*m_prioritizedList[i].msgSize));
        m_queues[i].setup(reinterpret_cast<U8*> (ptr), allocationSize,
                        m_prioritizedList[i].depth, m_prioritizedList[i].msgSize);
    }
}
// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ComQueue::comQueueIn_handler(const NATIVE_INT_TYPE portNum, Fw::ComBuffer& data, U32 context) {
    // Enqueues the items in the combuffer onto queue set
    FW_ASSERT(portNum >= 0 && portNum < ComQueueComSize, portNum);
    Fw::SerializeStatus status = m_queues[portNum].enqueue(reinterpret_cast<const U8*>(&data), ComQueueComSize);
    if (status == Fw::FW_SERIALIZE_NO_ROOM_LEFT && !m_throttle[portNum]) {
        this->log_WARNING_HI_QueueFull(QueueType::comQueue, portNum);
        m_throttle[portNum] = true;
    }
}

void ComQueue::buffQueueIn_handler(const NATIVE_INT_TYPE portNum, Fw::Buffer& fwBuffer) {
    // Enqueues the items in buff onto the queue set
    FW_ASSERT(portNum >= 0 && portNum < ComQueueBuffSize, portNum);

    // set is offset
    FW_ASSERT((portNum + ComQueueComSize) < totalSize);
    // use as constant
    Fw::SerializeStatus status =
        m_queues[portNum + ComQueueComSize].enqueue(reinterpret_cast<const U8*>(&fwBuffer), ComQueueBuffSize);

    if (status == Fw::FW_SERIALIZE_NO_ROOM_LEFT && !m_throttle[portNum]) {
        this->log_WARNING_HI_QueueFull(QueueType::buffQueue, portNum);
        m_throttle[portNum] = true;
    }
}

void ComQueue::comStatusIn_handler(const NATIVE_INT_TYPE portNum, Svc::ComSendStatus& ComStatus) {
    switch (ComStatus.e) {
        case ComSendStatus::READY:
            if (m_needRetry) {
                retryQueue();
                m_needRetry = false;
            } else {
                processQueue();
            }
            break;
        case ComSendStatus::FAIL:
            m_needRetry = true;
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
        buffQueueDepth[i] = m_queues[i].get_high_water_mark();
        m_queues[i].clear_high_water_mark();
    }
    this->tlmWrite_buffQueueDepth(buffQueueDepth);
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------
void ComQueue::retryQueue() {

    if (m_lastEntry.index < ComQueueComSize){
        this->comQueueSend_out(0,m_comBufferMessage, 0);
    } else {
        this->buffQueueSend_out(0,m_bufferMessage);
    }
}
// We work under the assumption that the metadata in prioritized list
// being fed into the component is already sorted
void ComQueue::processQueue() {
    NATIVE_UINT_TYPE i = 0;
    NATIVE_UINT_TYPE sendPriority = 0;

    for (i = 0; i < totalSize; i++){
        QueueData& entry = m_prioritizedList[i];
        Types::Queue& queue =  m_queues[entry.index];

        // Continue onto next iteration if there is no items in the queue
        if (queue.getQueueSize() == 0) {
            continue;
        }

        sendPriority = entry.priority;

        // Store entry in order to differentiate which buffer to send out during retry
        m_lastEntry = entry;

        if (entry.index < ComQueueComSize){
            queue.dequeue(reinterpret_cast<U8*>(&m_comBuffer), sizeof(m_comBuffer));
            m_comBufferMessage = m_comBuffer;
            this->comQueueSend_out(0,m_comBuffer, 0);
        } else {
            queue.dequeue(reinterpret_cast<U8*>(&m_buffer), sizeof(m_buffer));
            m_bufferMessage = m_buffer;
            this->buffQueueSend_out(0,m_buffer);
        }
        m_throttle[entry.index] = false; // sent message, throttle is now clear
        break;
    }

    // Onto the next entry after we sent the current entry
    // No need for initialization continuing from when the previous loop completed
    // Round robin of priority bucket as long as we have sent something
    for (i++; (m_prioritizedList[i].priority == sendPriority) && i < totalSize; i++){
        QueueData temp = m_prioritizedList[i];
        m_prioritizedList[i] = m_prioritizedList[i-1];
        m_prioritizedList[i-1] = temp;
    }
}
}  // end namespace Svc
