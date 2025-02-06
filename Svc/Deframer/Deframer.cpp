// ======================================================================
// \title  Deframer.cpp
// \author thomas-bc
// \brief  cpp file for Deframer component implementation class
// ======================================================================

#include "Svc/Deframer/Deframer.hpp"
#include "FpConfig.hpp"
#include "Fw/Types/Assert.hpp"

// REVIEW NOTE: Rename this to FprimeDeframer
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

    FW_ASSERT(data.getSize() >= FrameConfig::HEADER_SIZE + FrameConfig::CHECKSUM_SIZE);

    data.setData(data.getData() + FrameConfig::HEADER_SIZE);
    data.setSize(data.getSize() - FrameConfig::HEADER_SIZE - FrameConfig::CHECKSUM_SIZE);

    this->deframedOut_out(0, data, context);
}

}  // namespace Svc
