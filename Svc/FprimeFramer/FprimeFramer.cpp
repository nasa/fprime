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

void FprimeFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, Fw::Buffer& context) {
    FprimeProtocol::FrameHeader header;
    FprimeProtocol::FrameTrailer trailer;

    FwSizeType frameSize = FprimeProtocol::FrameHeader::SERIALIZED_SIZE + data.getSize() + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
    // check frame size can be held into Fw::Buffer::Size
    header.setlengthField(data.getSize());

    Fw::Buffer frameBuffer = this->bufferAllocate_out(0, static_cast<Fw::Buffer::SizeType>(frameSize));
    Fw::SerializeBufferBase& bufferSerializer = frameBuffer.getSerializeRepr();

    Fw::SerializeStatus status;
    status = bufferSerializer.serialize(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    status = bufferSerializer.serialize(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Calculate and add transmission hash
    Utils::HashBuffer hash;
    Utils::Hash::hash(frameBuffer.getData(), frameSize - HASH_DIGEST_LENGTH, hash);
    trailer.setcrcField(hash.asBigEndianU32());
    status = bufferSerializer.serialize(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    this->framedOut_helper(frameBuffer, context);
}

void FprimeFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void FprimeFramer ::framedOut_helper(Fw::Buffer& frameBuffer, Fw::Buffer& context) {
    if (this->isConnected_framedStreamOut_OutputPort(0)) {
        this->framedStreamOut_out(0, frameBuffer);
    }
    if (this->isConnected_framedDataOut_OutputPort(0)) {
        this->framedDataOut_out(0, frameBuffer, context);
    }
}

}  // namespace Svc
