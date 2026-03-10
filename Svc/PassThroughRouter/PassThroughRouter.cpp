// ======================================================================
// \title  PassThroughRouter.cpp
// \author kessler
// \brief  cpp file for PassThroughRouter component implementation class
// ======================================================================

#include "Svc/PassThroughRouter/PassThroughRouter.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

PassThroughRouter::PassThroughRouter(const char* const compName) : PassThroughRouterComponentBase(compName) {}

PassThroughRouter::~PassThroughRouter() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void PassThroughRouter::allPacketsReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    this->bufferDeallocate_out(0, fwBuffer);
}

void PassThroughRouter::dataIn_handler(FwIndexType portNum,
                                       Fw::Buffer& packetBuffer,
                                       const ComCfg::FrameContext& context) {
    if (this->isConnected_allPacketsOut_OutputPort(0)) {
        // Copy buffer into a new allocated buffer. This lets us return the original buffer with dataReturnOut,
        auto packetBufferCopy = this->bufferAllocate_out(0, packetBuffer.getSize());
        if (packetBufferCopy.isValid()) {
            auto copySerializer = packetBufferCopy.getSerializer();
            auto status = copySerializer.serializeFrom(packetBuffer.getData(), packetBuffer.getSize(),
                                                       Fw::Serialization::OMIT_LENGTH);
            FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
            // Send the copied buffer out. It will come back on allPacketsReturnIn once the receiver is done with it
            this->allPacketsOut_out(0, packetBufferCopy);
        } else {
            this->log_WARNING_HI_AllocationError();
        }
    }

    // Return ownership of the incoming packetBuffer
    this->dataReturnOut_out(0, packetBuffer, context);
}

}  // namespace Svc
