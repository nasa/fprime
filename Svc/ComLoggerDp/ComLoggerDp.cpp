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

void ComLoggerDp ::configure(bool enabled) {
    this->m_enabled = enabled;
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

    // Allocate container if needed
    if (this->m_currentPacketCount == 0) {
        // Calculate size needed for the requested number of packets
        // Each record holds up to FW_COM_BUFFER_MAX_SIZE bytes
        const FwSizeType containerSize = this->m_packetsPerContainer * SIZE_OF_ComBufferRecord_RECORD(FW_COM_BUFFER_MAX_SIZE);

        // Get a container buffer
        const Fw::Success status = this->dpGet_ComBuffContainer(containerSize, this->m_container);

        // Check for allocation failure
        if (status != Fw::Success::SUCCESS) {
            this->handleBufferDrop(static_cast<U32>(containerSize));
            return;
        }

        // Set the priority on the newly allocated container
        this->m_container.setPriority(this->m_priority);
    }

    // Get data from ComBuffer
    const FwSizeType dataSize = data.getSize();
    const U8* dataPtr = data.getBuffAddr();

    // Serialize the ComBuffer data directly into the container as a ComBufferRecord
    // The size is determined by the actual ComBuffer size
    Fw::SerializeStatus serStatus = this->m_container.serializeRecord_ComBufferRecord(dataPtr, dataSize);

    // Check for serialization failure - might be due to container being full
    if (serStatus != Fw::FW_SERIALIZE_OK) {
        // Send the current partial container
        if (this->m_currentPacketCount > 0) {
            this->dpSend(this->m_container);
        }
        this->m_currentPacketCount = 0;

        // Try to allocate a new container and retry serialization
        const FwSizeType containerSize = this->m_packetsPerContainer * SIZE_OF_ComBufferRecord_RECORD(FW_COM_BUFFER_MAX_SIZE);
        const Fw::Success status = this->dpGet_ComBuffContainer(containerSize, this->m_container);

        // If allocation fails, drop the buffer
        if (status != Fw::Success::SUCCESS) {
            this->handleBufferDrop(static_cast<U32>(containerSize));
            return;
        }

        // Set priority on the new container
        this->m_container.setPriority(this->m_priority);

        // Retry serialization with the new container
        serStatus = this->m_container.serializeRecord_ComBufferRecord(dataPtr, dataSize);

        // If serialization still fails, drop the buffer
        if (serStatus != Fw::FW_SERIALIZE_OK) {
            this->handleBufferDrop(static_cast<U32>(dataSize));
            return;
        }
    }

    // Increment counters
    ++this->m_currentPacketCount;
    ++this->m_numBuffersLogged;

    // Check if container has reached the limit
    if (this->m_currentPacketCount >= this->m_packetsPerContainer) {
        // Send the container
        this->dpSend(this->m_container);

        // Reset counter for next container
        this->m_currentPacketCount = 0;
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

    // Write telemetry
    this->tlmWrite_LoggingEnabled(this->m_enabled);
    this->tlmWrite_NumBuffersLogged(this->m_numBuffersLogged);
    this->tlmWrite_NumBuffersDropped(this->m_numBuffersDropped);
}

void ComLoggerDp ::startRecordingIn_handler(FwIndexType portNum, U32 packetsPerContainer, FwDpPriorityType priority) {
    // portNum is unused
    (void)portNum;

    // Call internal helper function
    this->startRecordingInternal(packetsPerContainer, priority);
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
    // Validate packetsPerContainer is non-zero
    if (packetsPerContainer == 0) {
        return false;
    }

    // If recording is already active and there's a partial container, send it before reconfiguring
    if (this->m_enabled && (this->m_currentPacketCount > 0)) {
        this->dpSend(this->m_container);
    }

    // Store configuration
    this->m_packetsPerContainer = packetsPerContainer;
    this->m_currentPacketCount = 0;
    this->m_priority = priority;

    // Enable logging
    this->m_enabled = true;

    // Log event
    this->log_ACTIVITY_HI_ComDpStarted(packetsPerContainer);

    return true;
}

U32 ComLoggerDp ::stopRecordingInternal() {
    U32 numSent = 0;

    // If there's a partial container, send it before stopping
    if (this->m_enabled && (this->m_currentPacketCount > 0)) {
        this->dpSend(this->m_container);
        numSent = 1;
        this->m_currentPacketCount = 0;
    }

    // Disable logging
    this->m_enabled = false;

    // Log event
    this->log_ACTIVITY_HI_ComDpStopped(numSent);

    return numSent;
}

void ComLoggerDp ::handleBufferDrop(U32 size) {
    this->log_WARNING_HI_DpBufferError(size);
    ++this->m_numBuffersDropped;
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void ComLoggerDp ::StartComDp_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 packetsPerContainer, U32 priority) {
    // Call internal helper function
    bool success = this->startRecordingInternal(packetsPerContainer, priority);

    // Send command response
    if (success) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
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
