// ======================================================================
// \title  FprimeDeframer.cpp
// \author thomas-bc
// \brief  cpp file for FprimeDeframer component implementation class
// ======================================================================

#include "Svc/FprimeDeframer/FprimeDeframer.hpp"
#include "Fw/FPrimeBasicTypes.hpp"
#include "Fw/Types/Assert.hpp"

#include "Svc/FprimeProtocol/FrameHeaderSerializableAc.hpp"
#include "Svc/FprimeProtocol/FrameTrailerSerializableAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

FprimeDeframer ::FprimeDeframer(const char* const compName) : FprimeDeframerComponentBase(compName) {}

FprimeDeframer ::~FprimeDeframer() {}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void FprimeDeframer ::framedIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    if (data.getSize() < FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE) {
        // Incoming buffer is not long enough to contain a valid frame (header+trailer)
        this->log_WARNING_HI_InvalidBufferReceived();
        this->bufferDeallocate_out(0, data); // drop the frame
        return;
    }

    // Header and Trailer objects to hold the deserialized data (types are autocoded by FPP)
    FprimeProtocol::FrameHeader header;
    FprimeProtocol::FrameTrailer trailer;

    // ---------------- Validate Frame Header ----------------
    // Deserialize transmitted header into the header object
    auto deserializer = data.getDeserializer();
    Fw::SerializeStatus status = header.deserialize(deserializer);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK, status);
    // Check that deserialized start_word token matches expected value (default start_word value in the FPP object)
    const FprimeProtocol::FrameHeader defaultValue;
    if (header.getstartWord() != defaultValue.getstartWord()) {
        this->log_WARNING_HI_InvalidStartWord();
        this->bufferDeallocate_out(0, data);
        return;
    }
    // We expect the frame size to be size of header + body (of size specified in header) + trailer
    const FwSizeType expectedFrameSize = FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.getlengthField() +
                                         FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
    if (data.getSize() < expectedFrameSize) {
        this->log_WARNING_HI_InvalidLengthReceived();
        this->bufferDeallocate_out(0, data);
        return;
    }

    // ---------------- Validate Frame Trailer ----------------
    // Deserialize transmitted trailer: trailer is at offset = len(header) + len(body)
    status = deserializer.moveDeserToOffset(FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.getlengthField());
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK, status);
    status = trailer.deserialize(deserializer);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK, status);
    // Compute CRC over the transmitted data (header + body)
    Utils::Hash hash;
    Utils::HashBuffer computedCrc;
    FwSizeType fieldToHashSize = header.getlengthField() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE;
    hash.init();
    // Add byte by byte to the hash
    for (FwSizeType i = 0; i < fieldToHashSize; i++) {
        hash.update(data.getData() + i, 1);
    }
    hash.final(computedCrc);
    // Check that the CRC in the trailer of the frame matches the computed CRC
    if (trailer.getcrcField() != computedCrc.asBigEndianU32()) {
        this->log_WARNING_HI_InvalidChecksum();
        this->bufferDeallocate_out(0, data);
        return;
    }

    // ---------------- Extract payload from frame ----------------
    // Shift data pointer to effectively remove the header
    data.setData(data.getData() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE);
    // Shrink size to effectively remove the trailer (also removes the header)
    data.setSize(data.getSize() - FprimeProtocol::FrameHeader::SERIALIZED_SIZE -
                 FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    // Emit the deframed data
    this->deframedOut_out(0, data, context);
}

}  // namespace Svc
