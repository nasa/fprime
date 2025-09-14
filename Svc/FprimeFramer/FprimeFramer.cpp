// ======================================================================
// \title  FprimeFramer.cpp
// \author thomas-bc
// \brief  cpp file for FprimeFramer component implementation class
// ======================================================================

#include "Svc/FprimeFramer/FprimeFramer.hpp"
#include "Svc/FprimeProtocol/FrameHeaderSerializableAc.hpp"
#include "Svc/FprimeProtocol/FrameTrailerSerializableAc.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FprimeFramer ::FprimeFramer(const char* const compName) : FprimeFramerComponentBase(compName) {}

FprimeFramer ::~FprimeFramer() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void FprimeFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    FprimeProtocol::FrameHeader header;
    FprimeProtocol::FrameTrailer trailer;

    // Full size of the frame will be size of header + data + trailer
    FwSizeType frameSize =
        FprimeProtocol::FrameHeader::SERIALIZED_SIZE + data.getSize() + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
    FW_ASSERT(data.getSize() <= std::numeric_limits<FprimeProtocol::TokenType>::max(),
              static_cast<FwAssertArgType>(frameSize));
    FW_ASSERT(frameSize <= std::numeric_limits<Fw::Buffer::SizeType>::max(), static_cast<FwAssertArgType>(frameSize));

    // Allocate frame buffer
    Fw::Buffer frameBuffer = this->bufferAllocate_out(0, frameSize);
    auto frameSerializer = frameBuffer.getSerializer();
    Fw::SerializeStatus status;

    // Serialize the header
    // 0xDEADBEEF is already set as the default value for the header startWord field in the FPP type definition
    header.set_lengthField(static_cast<FprimeProtocol::TokenType>(data.getSize()));
    status = frameSerializer.serialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Serialize the data
    status = frameSerializer.serialize(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Serialize the trailer (with CRC computation)
    Utils::HashBuffer hashBuffer;
    Utils::Hash::hash(frameBuffer.getData(), frameSize - HASH_DIGEST_LENGTH, hashBuffer);
    trailer.set_crcField(hashBuffer.asBigEndianU32());
    status = frameSerializer.serialize(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Send the full frame out - this port shall always be connected
    this->dataOut_out(0, frameBuffer, context);
    // Return original (unframed) data buffer ownership back to its sender - always connected
    this->dataReturnOut_out(0, data, context);
}

void FprimeFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void FprimeFramer ::dataReturnIn_handler(FwIndexType portNum,
                                         Fw::Buffer& frameBuffer,
                                         const ComCfg::FrameContext& context) {
    // dataReturnIn is the allocated buffer coming back from the ComManager (e.g. ComStub) component
    this->bufferDeallocate_out(0, frameBuffer);
}

}  // namespace Svc
