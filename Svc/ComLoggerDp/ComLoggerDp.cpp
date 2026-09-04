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
            this->log_WARNING_HI_DpBufferError(static_cast<U32>(containerSize));
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
    this->m_container.serializeRecord_ComBufferRecord(dataPtr, dataSize);

    // Increment counters
    ++this->m_currentPacketCount;
    ++this->m_numBuffersLogged;

    // Check if container is full
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
}

void ComLoggerDp ::startRecordingIn_handler(FwIndexType portNum, U32 config) {
    // portNum is unused
    (void)portNum;

    // Decode configuration from packed U32
    // Upper 16 bits: packetsPerContainer, lower 16 bits: priority
    const U32 packetsPerContainer = (config >> 16) & 0xFFFF;
    const U32 priority = config & 0xFFFF;

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

bool ComLoggerDp ::startRecordingInternal(U32 packetsPerContainer, U32 priority) {
    // Validate packetsPerContainer is non-zero
    if (packetsPerContainer == 0) {
        return false;
    }

    // Store configuration
    this->m_packetsPerContainer = packetsPerContainer;
    this->m_currentPacketCount = 0;
    this->m_priority = static_cast<FwDpPriorityType>(priority);

    // Update the container priority if logging is already active and there's an active container
    if (this->m_enabled && (this->m_currentPacketCount > 0)) {
        this->m_container.setPriority(this->m_priority);
    }

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

void ComLoggerDp ::UpdatePriority_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 priority) {
    // Store the new priority
    this->m_priority = static_cast<FwDpPriorityType>(priority);

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

    // Clear the DpBufferError event throttle
    this->log_WARNING_HI_DpBufferError_ThrottleClear();

    // Log event
    this->log_ACTIVITY_LO_CountersCleared();

    // Send command response
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
