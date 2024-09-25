// ======================================================================
// \title  Deframer.cpp
// \author chammard
// \brief  cpp file for Deframer component implementation class
// ======================================================================

#include "Svc/Deframer/Deframer.hpp"
#include "FpConfig.hpp"

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

    // TODO: add asserts?
    data.setData(data.getData() + FrameConfig::HEADER_SIZE);
    data.setSize(data.getSize() - FrameConfig::HEADER_SIZE - FrameConfig::CHECKSUM_SIZE);

    this->deframedOut_out(0, data, context);
}

}  // namespace Svc
