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
        const FwSizeType containerSize = this->m_packetsPerContainer * SIZE_OF_ComBufferRecord_RECORD;

        // Get a container buffer
        const Fw::Success status = this->dpGet_ComBuffContainer(containerSize, this->m_container);

        // Check for allocation failure
        if (status != Fw::Success::SUCCESS) {
            this->log_WARNING_HI_DpBufferError(static_cast<U32>(containerSize));
            return;
        }
    }

    // Serialize the ComBuffer into the container as a ComBufferRecord
    ComLoggerDp_ComBufferArray comArray{};

    // Copy data from ComBuffer to array
    const FwSizeType dataSize = data.getSize();
    const U8* dataPtr = data.getBuffAddr();

    // Copy only up to the array size
    const FwSizeType copySize = (dataSize <= static_cast<FwSizeType>(FW_COM_BUFFER_MAX_SIZE)) ? dataSize : static_cast<FwSizeType>(FW_COM_BUFFER_MAX_SIZE);
    for (FwSizeType i = 0; i < copySize; i++) {
        comArray[i] = dataPtr[i];
    }

    // Serialize the record into the container
    this->m_container.serializeRecord_ComBufferRecord(comArray);

    // Increment packet count
    ++this->m_currentPacketCount;

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

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void ComLoggerDp ::StartComDp_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 packetsPerContainer, U32 priority) {
    // Validate packetsPerContainer is non-zero
    if (packetsPerContainer == 0) {
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::VALIDATION_ERROR);
        return;
    }

    // Store configuration
    this->m_packetsPerContainer = packetsPerContainer;
    this->m_currentPacketCount = 0;

    // Update the container priority if logging is already active
    if (this->m_enabled) {
        this->m_container.setPriority(static_cast<FwDpPriorityType>(priority));
    }

    // Enable logging
    this->m_enabled = true;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void ComLoggerDp ::UpdatePriority_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, U32 priority) {
    // Only update priority if logging is enabled and there's an active container
    if (this->m_enabled && (this->m_currentPacketCount > 0)) {
        this->m_container.setPriority(static_cast<FwDpPriorityType>(priority));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        // No active container, command has no effect
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    }
}

void ComLoggerDp ::StopComDp_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // If there's a partial container, send it before stopping
    if (this->m_enabled && (this->m_currentPacketCount > 0)) {
        this->dpSend(this->m_container);
        this->m_currentPacketCount = 0;
    }

    // Disable logging
    this->m_enabled = false;

    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

}  // namespace Svc
