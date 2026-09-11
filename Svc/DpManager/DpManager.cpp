// ======================================================================
// \title  DpManager.cpp
// \author bocchino
// \brief  cpp file for DpManager component implementation class
// ======================================================================

#include "Svc/DpManager/DpManager.hpp"
#include "Fw/FPrimeBasicTypes.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

DpManager::DpManager(const char* const compName)
    : DpManagerComponentBase(compName),
      numSuccessfulAllocations(0),
      numFailedAllocations(0),
      numDataProducts(0),
      numBytes(0) {}

DpManager::~DpManager() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

Fw::Success DpManager::productGetIn_handler(const FwIndexType portNum,
                                            FwDpIdType id,
                                            FwSizeType size,
                                            Fw::Buffer& buffer) {
    return this->getBuffer(portNum, id, size, buffer);
}

void DpManager::productRequestIn_handler(const FwIndexType portNum, FwDpIdType id, FwSizeType size) {
    // Get a buffer
    Fw::Buffer buffer;
    const Fw::Success status = this->getBuffer(portNum, id, size, buffer);
    // Send buffer on productResponseOut
    this->productResponseOut_out(portNum, id, buffer, status);
}

void DpManager::productSendIn_handler(const FwIndexType portNum, FwDpIdType id, const Fw::Buffer& buffer) {
    // id is unused
    (void)id;
    // Update state variables
    ++this->numDataProducts;
    this->numBytes += buffer.getSize();
    // Send the buffer on productSendOut
    Fw::Buffer sendBuffer = buffer;
    this->productSendOut_out(portNum, sendBuffer);
}

void DpManager::schedIn_handler(const FwIndexType portNum, U32 context) {
    // Emit telemetry
    this->tlmWrite_NumSuccessfulAllocations(this->numSuccessfulAllocations);
    this->tlmWrite_NumFailedAllocations(this->numFailedAllocations);
    this->tlmWrite_NumDataProducts(this->numDataProducts);
    this->tlmWrite_NumBytes(this->numBytes);
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void DpManager ::CLEAR_EVENT_THROTTLE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->log_WARNING_HI_BufferAllocationFailed_ThrottleClear();
    this->log_WARNING_HI_BufferDropped_ThrottleClear();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

// ----------------------------------------------------------------------
// Hook implementations for typed async input ports
// ----------------------------------------------------------------------

void DpManager::productSendIn_overflowHook(FwIndexType portNum, FwDpIdType id, const Fw::Buffer& buffer) {
    // Queue full: drop the DP, record it, and return the buffer to its pool so it never leaks.
    // Runs on the caller's thread; bufferReturnOut targets the guarded BufferManager.
    (void)portNum;
    this->log_WARNING_HI_BufferDropped(id, buffer.getSize());
    if (buffer.isValid()) {
        Fw::Buffer returnBuffer = buffer;
        this->bufferReturnOut_out(0, returnBuffer);
    }
}

void DpManager::productRequestIn_overflowHook(FwIndexType portNum, FwDpIdType id, FwSizeType size) {
    // Queue full: no buffer was allocated (would happen in the handler), so nothing leaks.
    // Honor the request/response contract with an invalid buffer + FAILURE so the client
    // treats it as an allocation failure and moves on instead of waiting forever.
    (void)size;
    Fw::Buffer invalidBuffer;
    this->productResponseOut_out(portNum, id, invalidBuffer, Fw::Success::FAILURE);
}

// ----------------------------------------------------------------------
// Private helper functions
// ----------------------------------------------------------------------

Fw::Success DpManager::getBuffer(FwIndexType portNum, FwDpIdType id, FwSizeType size, Fw::Buffer& buffer) {
    // Set status
    Fw::Success status(Fw::Success::FAILURE);
    // Get a buffer
    buffer = this->bufferGetOut_out(portNum, static_cast<U32>(size));
    if (buffer.isValid()) {
        // Buffer is valid
        ++this->numSuccessfulAllocations;
        status = Fw::Success::SUCCESS;
    } else {
        // Buffer is invalid
        ++this->numFailedAllocations;
        this->log_WARNING_HI_BufferAllocationFailed(id);
    }
    return status;
}

}  // end namespace Svc
