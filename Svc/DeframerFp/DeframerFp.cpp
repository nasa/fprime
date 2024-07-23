// ======================================================================
// \title  DeframerFp.cpp
// \author chammard
// \brief  cpp file for DeframerFp component implementation class
// ======================================================================

#include "Svc/DeframerFp/DeframerFp.hpp"
#include "FpConfig.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

DeframerFp ::DeframerFp(const char* const compName) : DeframerFpComponentBase(compName) {}

DeframerFp ::~DeframerFp() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void DeframerFp ::framedIn_handler(FwIndexType portNum, Fw::Buffer& data, Fw::Buffer& context) {

    // TODO: add asserts?
    data.setData(data.getData() + FrameConfig::HEADER_SIZE);
    data.setSize(data.getSize() - FrameConfig::HEADER_SIZE - FrameConfig::CHECKSUM_SIZE);

    this->deframedOut_out(0, data, context);
}

}  // namespace Svc
