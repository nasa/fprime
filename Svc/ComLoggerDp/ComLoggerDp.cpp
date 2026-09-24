// ======================================================================
// \title  ComLoggerDp.cpp
// \author tcanham
// \brief  cpp file for ComLoggerDp component implementation class
// ======================================================================

#include "Svc/ComLoggerDp/ComLoggerDp.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ComLoggerDp ::ComLoggerDp(const char* const compName) : ComLoggerDpComponentBase(compName) {}

ComLoggerDp ::~ComLoggerDp() {}

// ----------------------------------------------------------------------
// Public interface
// ----------------------------------------------------------------------

void ComLoggerDp ::configure(bool enabled, U32 packetsPerContainer, FwDpPriorityType priority, U32 flushTimeout) {
    // Store flush timeout setting
    this->m_flushTimeout = flushTimeout;

    // If enabling, use the internal start function which validates parameters
    if (enabled) {
        // This will validate packetsPerContainer and set m_enabled
        const bool started = this->startRecordingInternal(packetsPerContainer, priority);
        FW_ASSERT(started, static_cast<FwAssertArgType>(packetsPerContainer));
    } else {
        // Just disable if not enabling
        this->m_enabled = false;
    }
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ComLoggerDp ::comIn_handler(FwIndexType portNum, Fw::ComBuffer& data, U32 context) {
    // portNum and context are unused
    (void)portNum;
    (void)context;

    // If logging is not enabled, return immediately
    if (!this->m_enabled) {
        return;
    }

    // Reset inactivity counter - we received a packet
    this->m_schedCallsSinceLastPacket = 0;

    // Allocate container if needed
    if (this->m_currentPacketCount == 0) {
        if (!this->allocateAndSetupContainer()) {
            return;
        }
    }

    // Get data from ComBuffer
    const FwSizeType dataSize = data.getSize();
    const U8* dataPtr = data.getBuffAddr();

    // Serialize the packet
    this->serializePacket(dataPtr, dataSize);

    // Increment counters
    ++this->m_currentPacketCount;
    ++this->m_numBuffersLogged;

    // Check if container has reached the limit
    if (this->m_currentPacketCount >= this->m_packetsPerContainer) {
        this->finalizeContainer();
    }
}

void ComLoggerDp ::pingIn_handler(FwIndexType portNum, U32 key) {
    // Return the key via pingOut port
    this->pingOut_out(portNum, key);
}

void ComLoggerDp ::schedIn_handler(FwIndexType portNum, U32 context) {
    // portNum and context are unused
    (void)portNum;
    (void)context;

    // Check for auto-flush condition
    // Note: m_flushTimeout > 0 check ensures auto-flush is disabled when timeout is 0
    // Without this check, flushTimeout=0 would cause immediate flush (0 >= 0 is true)
    if (this->m_enabled && this->m_currentPacketCount > 0 && this->m_flushTimeout > 0) {
        ++this->m_schedCallsSinceLastPacket;

        if (this->m_schedCallsSinceLastPacket >= this->m_flushTimeout) {
            // Flush the partial container
            this->finalizeContainer();
            this->m_schedCallsSinceLastPacket = 0;
        }
    }

    // Write telemetry
    this->tlmWrite_LoggingEnabled(this->m_enabled);
    this->tlmWrite_NumBuffersLogged(this->m_numBuffersLogged);
    this->tlmWrite_NumBuffersDropped(this->m_numBuffersDropped);
    this->tlmWrite_NumQueueDrops(static_cast<U32>(this->getNumMsgsDropped()));
}

void ComLoggerDp ::startRecordingIn_handler(FwIndexType portNum, U32 packetsPerContainer, FwDpPriorityType priority) {
    // portNum is unused
    (void)portNum;

    // Call internal helper function and check for validation failure
    bool success = this->startRecordingInternal(packetsPerContainer, priority);

    // Log warning if validation failed
    if (!success) {
        this->log_WARNING_LO_StartRecordingFailed(packetsPerContainer);
    }
}

void ComLoggerDp ::stopRecordingIn_handler(FwIndexType portNum) {
    // portNum is unused
    (void)portNum;

    // Call internal helper function
    this->stopRecordingInternal();
}

// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

bool ComLoggerDp ::startRecordingInternal(U32 packetsPerContainer, FwDpPriorityType priority) {
    // Validate packetsPerContainer is non-zero and doesn't exceed the max size that
    // DP creation allows
    constexpr U32 MAX_PACKETS_PER_CONTAINER =
        static_cast<U32>((std::numeric_limits<U32>::max() - Fw::DpContainer::MIN_PACKET_SIZE) / RECORD_SIZE);
    if ((packetsPerContainer == 0) || (packetsPerContainer > MAX_PACKETS_PER_CONTAINER)) {
        // Disable logging on validation failure, flushing any partial container first
        if (this->m_enabled) {
            this->sendContainerIfNonEmpty();
        }
        this->m_enabled = false;
        return false;
    }

    // If recording is already active and there's a partial container, send it before reconfiguring
    if (this->m_enabled) {
        this->sendContainerIfNonEmpty();
    }

    // Store configuration
    this->m_packetsPerContainer = packetsPerContainer;
    this->m_priority = priority;

    // Reset inactivity counter
    this->m_schedCallsSinceLastPacket = 0;

    // Enable logging
    this->m_enabled = true;

    // Log event
    this->log_ACTIVITY_HI_ComDpStarted(packetsPerContainer);

    return true;
}

U32 ComLoggerDp ::stopRecordingInternal() {
    // 1 if a partial container is about to be sent, 0 otherwise (event arg and return value)
    const U32 numSent = (this->m_currentPacketCount > 0) ? 1 : 0;

    // If there's a partial container, send it before stopping
    if (this->m_enabled) {
        this->sendContainerIfNonEmpty();
    }

    // Disable logging
    this->m_enabled = false;

    // Reset inactivity counter
    this->m_schedCallsSinceLastPacket = 0;

    // Log event
    this->log_ACTIVITY_HI_ComDpStopped(numSent);

    return numSent;
}

void ComLoggerDp ::handleBufferDrop(U32 size) {
    this->log_WARNING_HI_DpBufferError(size);
    ++this->m_numBuffersDropped;
}

bool ComLoggerDp ::allocateAndSetupContainer() {
    // Calculate data size needed for the requested number of packets
    // Each record holds a sentry plus up to FW_COM_BUFFER_MAX_SIZE bytes
    // Note: DpManager adds the container header overhead, so we only request the data size
    const FwSizeType containerSize = this->m_packetsPerContainer * RECORD_SIZE;

    // Get a container buffer
    const Fw::Success status = this->dpGet_ComBuffContainer(containerSize, this->m_container);

    // Check for allocation failure
    if (status != Fw::Success::SUCCESS) {
        this->handleBufferDrop(static_cast<U32>(containerSize));
        return false;
    }

    // Set the priority on the newly allocated container
    this->m_container.setPriority(this->m_priority);
    return true;
}

void ComLoggerDp ::serializePacket(const U8* dataPtr, FwSizeType dataSize) {
    // Create buffer with sentry followed by ComBuffer data
    // Use sizeof to get sentry size (supports user changing the constant type)
    const FwSizeType sentrySize = sizeof(ComLoggerDpSentry);
    const FwSizeType totalSize = sentrySize + dataSize;

    // Use F Prime serialization to insert sentry value (handles endianness automatically)
    Fw::ExternalSerializeBuffer serBuf(this->m_recordBuffer, sizeof(this->m_recordBuffer));
    Fw::SerializeStatus serStatus = serBuf.serializeFrom(ComLoggerDpSentry);
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

    // Copy ComBuffer data after sentry
    serStatus = serBuf.serializeFrom(dataPtr, dataSize, Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

    // Serialize the complete record into the container
    serStatus = this->m_container.serializeRecord_ComBufferRecord(this->m_recordBuffer, totalSize);
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);
}

void ComLoggerDp ::sendContainerIfNonEmpty() {
    // Send container if it has any packets
    // Handles both full and partial containers
    if (this->m_currentPacketCount > 0) {
        this->dpSend(this->m_container);
        // Note: dpSend() invalidates the container; will allocate new one on next packet
        this->m_currentPacketCount = 0;
    }
}

void ComLoggerDp ::finalizeContainer() {
    // Send the container (full or partial) and reset the packet count
    this->sendContainerIfNonEmpty();
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void ComLoggerDp ::StartComDp_cmdHandler(FwOpcodeType opCode,
                                         U32 cmdSeq,
                                         U32 packetsPerContainer,
                                         FwDpPriorityType priority) {
    // Call internal helper function
    bool success = this->startRecordingInternal(packetsPerContainer, priority);

    // Send command response
    if (success) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        // Log warning event for validation failure (consistent with port path)
        this->log_WARNING_LO_StartRecordingFailed(packetsPerContainer);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
    }
}

void ComLoggerDp ::UpdatePriority_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, FwDpPriorityType priority) {
    // Store the new priority
    this->m_priority = priority;

    // Update priority if logging is enabled and there's an active container
    if (this->m_enabled && (this->m_currentPacketCount > 0)) {
        this->m_container.setPriority(this->m_priority);
    }

    // Log event
    this->log_ACTIVITY_LO_PriorityUpdated(priority);

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void ComLoggerDp ::StopComDp_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Call internal helper function
    this->stopRecordingInternal();

    // Send command response
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void ComLoggerDp ::CLEAR_COUNTERS_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Clear the NumBuffersLogged counter
    this->m_numBuffersLogged = 0;

    // Clear the NumBuffersDropped counter
    this->m_numBuffersDropped = 0;

    // Clear the DpBufferError event throttle
    this->log_WARNING_HI_DpBufferError_ThrottleClear();

    // Log event
    this->log_ACTIVITY_LO_CountersCleared();

    // Send command response
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
