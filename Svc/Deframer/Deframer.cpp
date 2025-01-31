// ======================================================================
// \title  Deframer.cpp
// \author chammard
// \brief  cpp file for Deframer component implementation class
// ======================================================================

#include "Svc/Deframer/Deframer.hpp"
#include "FpConfig.hpp"
#include "Fw/Types/Assert.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

Deframer ::Deframer(const char* const compName) : DeframerComponentBase(compName) {}

Deframer ::~Deframer() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void Deframer ::framedIn_handler(FwIndexType portNum, Fw::Buffer& data, Fw::Buffer& context) {

    // Add checks to ensure that the data is a valid F' frame ??
    // Seems like this component should be able to do that on its own without relying
    // on an upstream component (the F´ accumulator)
    FW_ASSERT(data.getSize() >= FrameConfig::HEADER_SIZE + FrameConfig::CHECKSUM_SIZE);

    data.setData(data.getData() + FrameConfig::HEADER_SIZE);
    data.setSize(data.getSize() - FrameConfig::HEADER_SIZE - FrameConfig::CHECKSUM_SIZE);

    this->deframedOut_out(0, data, context);
}

}  // namespace Svc
