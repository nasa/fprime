// ======================================================================
// \title  FprimeDeframer.cpp
// \author thomas-bc
// \brief  cpp file for FprimeDeframer component implementation class
// ======================================================================

#include "Svc/FprimeDeframer/FprimeDeframer.hpp"
#include "FpConfig.hpp"
#include "Fw/Types/Assert.hpp"

#include "config/FrameHeaderSerializableAc.hpp"
#include "config/FrameTrailerSerializableAc.hpp"


namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FprimeDeframer ::FprimeDeframer(const char* const compName) : FprimeDeframerComponentBase(compName) {}

FprimeDeframer ::~FprimeDeframer() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void FprimeDeframer ::framedIn_handler(FwIndexType portNum, Fw::Buffer& data, Fw::Buffer& context) {

    FW_ASSERT(data.getSize() >= FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);

    data.setData(data.getData() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE);
    data.setSize(data.getSize() - FprimeProtocol::FrameHeader::SERIALIZED_SIZE - FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);

    this->deframedOut_out(0, data, context);
}

}  // namespace Svc
