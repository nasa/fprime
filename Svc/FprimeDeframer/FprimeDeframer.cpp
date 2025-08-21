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

void FprimeDeframer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    if (data.getSize() < FprimeProtocol::FrameHeader::SERIALIZED_SIZE + FprimeProtocol::FrameTrailer::SERIALIZED_SIZE) {
        // Incoming buffer is not long enough to contain a valid frame (header+trailer)
        this->log_WARNING_HI_InvalidBufferReceived();
        this->dataReturnOut_out(0, data, context);  // drop the frame
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
    if (header.get_startWord() != defaultValue.get_startWord()) {
        this->log_WARNING_HI_InvalidStartWord();
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }
    // We expect the frame size to be size of header + body (of size specified in header) + trailer
    const FwSizeType expectedFrameSize = FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.get_lengthField() +
                                         FprimeProtocol::FrameTrailer::SERIALIZED_SIZE;
    if (data.getSize() < expectedFrameSize) {
        this->log_WARNING_HI_InvalidLengthReceived();
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }
    // -------- Attempt to extract APID from Payload --------
    // If PacketDescriptor translates to an invalid APID, let it default to FW_PACKET_UNKNOWN
    // and let downstream components (e.g. custom router) handle it
    FwPacketDescriptorType packetDescriptor;
    status = deserializer.deserialize(packetDescriptor);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK, status);
    ComCfg::FrameContext contextCopy = context;
    // If a valid descriptor is deserialized, set it in the context
    if (packetDescriptor < ComCfg::APID::INVALID_UNINITIALIZED) {
        contextCopy.set_apid(static_cast<ComCfg::APID::T>(packetDescriptor));
    }

    // ---------------- Validate Frame Trailer ----------------
    // Deserialize transmitted trailer: trailer is at offset = len(header) + len(body)
    status = deserializer.moveDeserToOffset(FprimeProtocol::FrameHeader::SERIALIZED_SIZE + header.get_lengthField());
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK, status);
    status = trailer.deserialize(deserializer);
    FW_ASSERT(status == Fw::SerializeStatus::FW_SERIALIZE_OK, status);
    // Compute CRC over the transmitted data (header + body)
    Utils::Hash hash;
    Utils::HashBuffer computedCrc;
    FwSizeType fieldToHashSize = header.get_lengthField() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE;
    hash.init();
    // Add byte by byte to the hash
    for (FwSizeType i = 0; i < fieldToHashSize; i++) {
        hash.update(data.getData() + i, 1);
    }
    hash.final(computedCrc);
    // Check that the CRC in the trailer of the frame matches the computed CRC
    if (trailer.get_crcField() != computedCrc.asBigEndianU32()) {
        this->log_WARNING_HI_InvalidChecksum();
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }

    // ---------------- Extract payload from frame ----------------
    // Shift data pointer to effectively remove the header
    data.setData(data.getData() + FprimeProtocol::FrameHeader::SERIALIZED_SIZE);
    // Shrink size to effectively remove the trailer (also removes the header)
    data.setSize(data.getSize() - FprimeProtocol::FrameHeader::SERIALIZED_SIZE -
                 FprimeProtocol::FrameTrailer::SERIALIZED_SIZE);
    // Emit the deframed data
    this->dataOut_out(0, data, contextCopy);
}

void FprimeDeframer ::dataReturnIn_handler(FwIndexType portNum,
                                           Fw::Buffer& fwBuffer,
                                           const ComCfg::FrameContext& context) {
    this->dataReturnOut_out(0, fwBuffer, context);
}

}  // namespace Svc
