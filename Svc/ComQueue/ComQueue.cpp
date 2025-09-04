// ======================================================================
// \title  ComQueue.cpp
// \author vbai
// \brief  cpp file for ComQueue component implementation class
// ======================================================================

#include <Fw/Com/ComPacket.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/ComQueue/ComQueue.hpp>
#include <type_traits>
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

using FwUnsignedIndexType = std::make_unsigned<FwIndexType>::type;

ComQueue ::QueueConfigurationTable ::QueueConfigurationTable() {
    static_assert(static_cast<FwUnsignedIndexType>(std::numeric_limits<FwIndexType>::max()) >=
                      FW_NUM_ARRAY_ELEMENTS(this->entries),
                  "Number of entries must fit into FwIndexType");
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(this->entries)); i++) {
        this->entries[i].priority = 0;
        this->entries[i].depth = 0;
    }
}

ComQueue ::ComQueue(const char* const compName)
    : ComQueueComponentBase(compName),
      m_state(WAITING),
      m_allocationId(static_cast<FwEnumStoreType>(-1)),
      m_allocator(nullptr),
      m_allocation(nullptr) {
    // Initialize throttles to "off"
    for (FwIndexType i = 0; i < TOTAL_PORT_COUNT; i++) {
        this->m_throttle[i] = false;
    }
}

ComQueue ::~ComQueue() {}

void ComQueue ::cleanup() {
    // Deallocate memory ignoring error conditions
    if ((this->m_allocator != nullptr) && (this->m_allocation != nullptr)) {
        this->m_allocator->deallocate(this->m_allocationId, this->m_allocation);
    }
}

void ComQueue::configure(QueueConfigurationTable queueConfig,
                         FwEnumStoreType allocationId,
                         Fw::MemAllocator& allocator) {
    FwIndexType currentPriorityIndex = 0;
    FwSizeType totalAllocation = 0;

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
        for (FwIndexType entryIndex = 0;
             entryIndex < static_cast<FwIndexType>(FW_NUM_ARRAY_ELEMENTS(queueConfig.entries)); entryIndex++) {
            // Check for valid configuration entry
            FW_ASSERT(queueConfig.entries[entryIndex].priority < TOTAL_PORT_COUNT,
                      static_cast<FwAssertArgType>(queueConfig.entries[entryIndex].priority),
                      static_cast<FwAssertArgType>(TOTAL_PORT_COUNT), static_cast<FwAssertArgType>(entryIndex));

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
                // Overflow checks
                FW_ASSERT((std::numeric_limits<FwSizeType>::max() / entry.depth) >= entry.msgSize,
                          static_cast<FwAssertArgType>(entry.depth), static_cast<FwAssertArgType>(entry.msgSize));
                FW_ASSERT(std::numeric_limits<FwSizeType>::max() - (entry.depth * entry.msgSize) >= totalAllocation);
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
                  static_cast<FwAssertArgType>(this->m_prioritizedList[i].index));
        FW_ASSERT((allocationSize + allocationOffset) <= totalAllocation, static_cast<FwAssertArgType>(allocationSize),
                  static_cast<FwAssertArgType>(allocationOffset), static_cast<FwAssertArgType>(totalAllocation));

        // Setup queue's memory allocation, depth, and message size. Setup is skipped for a depth 0 queue
        if (allocationSize > 0) {
            this->m_queues[this->m_prioritizedList[i].index].setup(
                reinterpret_cast<U8*>(this->m_allocation) + allocationOffset, allocationSize,
                this->m_prioritizedList[i].depth, this->m_prioritizedList[i].msgSize);
        }
        allocationOffset += allocationSize;
    }
    // Safety check that all memory was used as expected
    FW_ASSERT(allocationOffset == totalAllocation, static_cast<FwAssertArgType>(allocationOffset),
              static_cast<FwAssertArgType>(totalAllocation));
}
// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ComQueue::comPacketQueueIn_handler(const FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    // Ensure that the port number of comPacketQueueIn is consistent with the expectation
    FW_ASSERT(portNum >= 0 && portNum < COM_PORT_COUNT, static_cast<FwAssertArgType>(portNum));
    (void)this->enqueue(portNum, QueueType::COM_QUEUE, reinterpret_cast<const U8*>(&data), sizeof(Fw::ComBuffer));
}

void ComQueue::bufferQueueIn_handler(const FwIndexType portNum, Fw::Buffer& fwBuffer) {
    FW_ASSERT(std::numeric_limits<FwIndexType>::max() - COM_PORT_COUNT > portNum);
    const FwIndexType queueNum = static_cast<FwIndexType>(portNum + COM_PORT_COUNT);
    // Ensure that the port number of bufferQueueIn is consistent with the expectation
    FW_ASSERT(portNum >= 0 && portNum < BUFFER_PORT_COUNT, static_cast<FwAssertArgType>(portNum));
    FW_ASSERT(queueNum < TOTAL_PORT_COUNT);
    bool success =
        this->enqueue(queueNum, QueueType::BUFFER_QUEUE, reinterpret_cast<const U8*>(&fwBuffer), sizeof(Fw::Buffer));
    if (!success) {
        this->bufferReturnOut_out(portNum, fwBuffer);
    }
}

void ComQueue::comStatusIn_handler(const FwIndexType portNum, Fw::Success& condition) {
    switch (this->m_state) {
        // On success, the queue should be processed. On failure, the component should still wait.
        case WAITING:
            if (condition.e == Fw::Success::SUCCESS) {
                this->m_state = READY;
                this->processQueue();
                // A message may or may not be sent. Thus, READY or WAITING are acceptable final states.
                FW_ASSERT((this->m_state == WAITING || this->m_state == READY),
                          static_cast<FwAssertArgType>(this->m_state));
            } else {
                this->m_state = WAITING;
            }
            break;
        // Both READY and unknown states should not be possible at this point. To receive a status message we must be
        // one of the WAITING or RETRY states.
        default:
            FW_ASSERT(0, static_cast<FwAssertArgType>(this->m_state));
            break;
    }
}

void ComQueue::run_handler(const FwIndexType portNum, U32 context) {
    // Downlink the high-water marks for the Fw::ComBuffer array types
    ComQueueDepth comQueueDepth;
    for (U32 i = 0; i < comQueueDepth.SIZE; i++) {
        comQueueDepth[i] = static_cast<U32>(this->m_queues[i].get_high_water_mark());
        this->m_queues[i].clear_high_water_mark();
    }
    this->tlmWrite_comQueueDepth(comQueueDepth);

    // Downlink the high-water marks for the Fw::Buffer array types
    BuffQueueDepth buffQueueDepth;
    for (U32 i = 0; i < buffQueueDepth.SIZE; i++) {
        buffQueueDepth[i] = static_cast<U32>(this->m_queues[i + COM_PORT_COUNT].get_high_water_mark());
        this->m_queues[i + COM_PORT_COUNT].clear_high_water_mark();
    }
    this->tlmWrite_buffQueueDepth(buffQueueDepth);
}

void ComQueue ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    static_assert(std::numeric_limits<FwIndexType>::is_signed, "FwIndexType must be signed");
    // For the buffer queues, the index of the queue is portNum offset by COM_PORT_COUNT since
    // the first COM_PORT_COUNT queues are for ComBuffer. So we have for buffer queues:
    // queueNum = portNum + COM_PORT_COUNT
    // Since queueNum is used as APID, we can retrieve the original portNum like such:
    FwIndexType bufferReturnPortNum = static_cast<FwIndexType>(context.get_comQueueIndex() - ComQueue::COM_PORT_COUNT);
    // Failing this assert means that context.apid was modified since ComQueue set it, which should not happen
    FW_ASSERT(bufferReturnPortNum < BUFFER_PORT_COUNT, static_cast<FwAssertArgType>(bufferReturnPortNum));
    if (bufferReturnPortNum >= 0) {
        // It is a coding error not to connect the associated bufferReturnOut port for each dataReturnIn port
        FW_ASSERT(this->isConnected_bufferReturnOut_OutputPort(bufferReturnPortNum),
                  static_cast<FwAssertArgType>(bufferReturnPortNum));
        // If this is a buffer port, return the buffer to the BufferDownlink
        this->bufferReturnOut_out(bufferReturnPortNum, data);
    }
}

// ----------------------------------------------------------------------
// Hook implementations for typed async input ports
// ----------------------------------------------------------------------

void ComQueue::bufferQueueIn_overflowHook(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    FW_ASSERT(portNum >= 0 && portNum < BUFFER_PORT_COUNT, static_cast<FwAssertArgType>(portNum));
    this->bufferReturnOut_out(portNum, fwBuffer);
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

bool ComQueue::enqueue(const FwIndexType queueNum, QueueType queueType, const U8* data, const FwSizeType size) {
    // Enqueue the given message onto the matching queue. When no space is available then emit the queue overflow event,
    // set the appropriate throttle, and move on. Will assert if passed a message for a depth 0 queue.
    const FwSizeType expectedSize = (queueType == QueueType::COM_QUEUE) ? sizeof(Fw::ComBuffer) : sizeof(Fw::Buffer);
    FW_ASSERT((queueType == QueueType::COM_QUEUE) || (queueNum >= COM_PORT_COUNT),
              static_cast<FwAssertArgType>(queueType), static_cast<FwAssertArgType>(queueNum));
    const FwIndexType portNum =
        static_cast<FwIndexType>(queueNum - ((queueType == QueueType::COM_QUEUE) ? 0 : COM_PORT_COUNT));
    bool rvStatus = true;
    FW_ASSERT(expectedSize == size, static_cast<FwAssertArgType>(size), static_cast<FwAssertArgType>(expectedSize));
    FW_ASSERT(portNum >= 0, static_cast<FwAssertArgType>(portNum));
    Fw::SerializeStatus status = this->m_queues[queueNum].enqueue(data, size);
    if (status == Fw::FW_SERIALIZE_NO_ROOM_LEFT) {
        if (!this->m_throttle[queueNum]) {
            this->log_WARNING_HI_QueueOverflow(queueType, static_cast<U32>(portNum));
            this->m_throttle[queueNum] = true;
        }

        rvStatus = false;
    }
    // When the component is already in READY state process the queue to send out the next available message immediately
    if (this->m_state == READY) {
        this->processQueue();
    }

    return rvStatus;
}

void ComQueue::sendComBuffer(Fw::ComBuffer& comBuffer, FwIndexType queueIndex) {
    FW_ASSERT(this->m_state == READY);
    Fw::Buffer outBuffer(comBuffer.getBuffAddr(), static_cast<Fw::Buffer::SizeType>(comBuffer.getBuffLength()));

    // Context value is used to determine what to do when the buffer returns on the dataReturnIn port
    ComCfg::FrameContext context;
    FwPacketDescriptorType descriptor;
    Fw::SerializeStatus status = comBuffer.deserializeTo(descriptor);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    context.set_apid(static_cast<ComCfg::APID::T>(descriptor));
    context.set_comQueueIndex(queueIndex);

    this->dataOut_out(0, outBuffer, context);
    // Set state to WAITING for the status to come back
    this->m_state = WAITING;
}

void ComQueue::sendBuffer(Fw::Buffer& buffer, FwIndexType queueIndex) {
    // Retry buffer expected to be cleared as we are either transferring ownership or have already deallocated it.
    FW_ASSERT(this->m_state == READY);

    // Context value is used to determine what to do when the buffer returns on the dataReturnIn port
    ComCfg::FrameContext context;
    FwPacketDescriptorType descriptor;
    Fw::SerializeStatus status = buffer.getDeserializer().deserializeTo(descriptor);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));
    context.set_apid(static_cast<ComCfg::APID::T>(descriptor));
    context.set_comQueueIndex(queueIndex);

    this->dataOut_out(0, buffer, context);
    // Set state to WAITING for the status to come back
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
            this->sendComBuffer(comBuffer, entry.index);
        } else {
            Fw::Buffer buffer;
            queue.dequeue(reinterpret_cast<U8*>(&buffer), sizeof(buffer));
            this->sendBuffer(buffer, entry.index);
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
