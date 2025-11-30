// ======================================================================
// \title  AosFramer.cpp
// \author thomas-bc
// \brief  cpp file for AosFramer component implementation class
// ======================================================================

#include "Svc/Ccsds/AosFramer/AosFramer.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AosFramer ::AosFramer(const char* const compName)
    : AosFramerComponentBase(compName), m_masterFrameCount(0), m_virtualFrameCount(0) {}

AosFramer ::~AosFramer() {}

void AosFramer::configure(U32 fixedFixedSize, bool frameErrorControlField) {
    static_assert(fixedFixedSize < ComCfg::AosMaxFrameFixedSize,
                  "fixedFrameSize must be less than the maximum defined in ComCfg.fpp");

    static_assert(fixedFrameSize > AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE +
                                       (frameErrorControlField ? AOSTrailer::SERIALIZED_SIZE : 0),
                  "AOS Frame Fixed Size must be at least large enough to hold header, trailer and data");

    this->m_fixedFrameSize = fixedFixedSize;
    this->m_fecf = frameErrorControlField;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AosFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // Ensure rest of the com stack is complying with the [communications adapter
    // interface](docs/reference/communication-adapter-interface.md)
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED, static_cast<FwAssertArgType>(this->m_bufferState));

    // -----------------------------------------------
    // Header
    // -----------------------------------------------
    AosHeader header;

    // GVCID (Global Virtual Channel ID) (Standard 4.1.2.2 and 4.1.2.3)
    U16 globalVcId = static_cast<U16>(context.get_vcId() << AOSHeaderSubfields::virtualChannelIdOffset);
    globalVcId |= static_cast<U16>(ComCfg::SpacecraftId & 0x00FF << AOSHeaderSubfields::spacecraftIdLsbOffset);
    globalVcId |= static_cast<U16>(Ccsds::Tfvn::AOS & 0x3 << AOSHeaderSubfields::frameVersionOffset);

    // Virtual Channel Frame Count (4.1.2.4)
    U32 frameCountAndSignaling =
        static_cast<U32>(this->m_virtualFrameCount & 0x00FF_FFFF << AOSHeaderSubfields::vcFrameCountOffset);

    // Replay Flag (4.1.2.5.2)
    frameCountAndSignaling |= static_cast<U32>(replayFlag << AOSHeaderSubfields::replayFlagOffset);

    // Virtual Channel Frame Count Cycle Use Flag (4.1.2.5.3)
    frameCountAndSignaling |= static_cast<U32>(1 << AOSHeaderSubfields::cycleCountFlagOffset);

    // Spacecraft ID MSB (4.1.2.5.4)
    frameCountAndSignaling |=
        static_cast<U32>(ComCfg::SpacecraftId & 0x0300 >> (8 - AOSHeaderSubfields::spacecraftIdMsbOffset));

    // Virtual Channel Frame Cycle Count (4.1.2.5.5)
    frameCountAndSignaling |=
        static_cast<U32>(this->m_virtualFrameCount & 0x0F00_0000 >> (24 - AOSHeaderSubfields::spacecraftIdMsbOffset));

    header.set_globalVcId(globalVcId);
    header.set_frameCountAndSignaling(frameCountAndSignaling);

    // We use only a single Virtual Channel for now, so increment the one counter
    // Perform the modulo at serialization time we we can add vc cycle count
    this->m_virtualFrameCount++;  // U24 intended to wrap around (modulo 16,777,216)

    // -------------------------------------------------
    // Data field
    // -------------------------------------------------
    // Payload packet
    Fw::SerializeStatus status;
    // Create frame Fw::Buffer using member data field
    Fw::Buffer frameBuffer = Fw::Buffer(this->m_frameBuffer, sizeof(this->m_frameBuffer));
    auto frameSerializer = frameBuffer.getSerializer();
    status = frameSerializer.serializeFrom(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = frameSerializer.serializeFrom(data.getData(), data.getSize(), Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // As per TM Standard 4.2.2.5, fill the rest of the data field with an Idle Packet
    this->fill_with_idle_packet(frameSerializer);

    // -------------------------------------------------
    // Trailer (CRC)
    // -------------------------------------------------
    TMTrailer trailer;
    // Compute CRC over the entire frame buffer minus the FECF trailer (Standard 4.1.6)
    U16 crc =
        Ccsds::Utils::CRC16::compute(frameBuffer.getData(), sizeof(this->m_frameBuffer) - TMTrailer::SERIALIZED_SIZE);
    // Set the Frame Error Control Field (FECF)
    trailer.set_fecf(crc);
    // Move the serializer pointer to the end of the location where the trailer will be
    // serialized
    frameSerializer.moveSerToOffset(ComCfg::TmFrameFixedSize - TMTrailer::SERIALIZED_SIZE);
    status = frameSerializer.serializeFrom(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    this->m_bufferState = BufferOwnershipState::NOT_OWNED;
    this->dataOut_out(0, frameBuffer, context);
    this->dataReturnOut_out(0, data,
                            context);  // return ownership of the original data buffer
}

void AosFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        this->comStatusOut_out(portNum, condition);
    }
}

void AosFramer ::dataReturnIn_handler(FwIndexType portNum,
                                      Fw::Buffer& frameBuffer,
                                      const ComCfg::FrameContext& context) {
    // Assert that the returned buffer is the member, and set ownership state
    FW_ASSERT(frameBuffer.getData() >= &this->m_frameBuffer[0]);
    FW_ASSERT(frameBuffer.getData() < &this->m_frameBuffer[0] + sizeof(this->m_frameBuffer));
    this->m_bufferState = BufferOwnershipState::OWNED;
}

void AosFramer ::fill_with_idle_packet(Fw::SerializeBufferBase& serializer) {
    constexpr U16 endIndex = ComCfg::TmFrameFixedSize - TMTrailer::SERIALIZED_SIZE;
    constexpr U16 idleApid = static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET);
    const U16 startIndex = static_cast<U16>(serializer.getSize());
    const U16 idlePacketSize = static_cast<U16>(endIndex - startIndex);
    // Length token is defined as the number of bytes of payload data minus 1
    const U16 lengthToken = static_cast<U16>(idlePacketSize - SpacePacketHeader::SERIALIZED_SIZE - 1);

    FW_ASSERT(idlePacketSize >= 7, static_cast<FwAssertArgType>(idlePacketSize));  // 7 bytes minimum for idle packet
    FW_ASSERT(idlePacketSize <= ComCfg::TmFrameFixedSize, static_cast<FwAssertArgType>(idlePacketSize));

    SpacePacketHeader header;
    header.set_packetIdentification(idleApid);
    header.set_packetSequenceControl(
        0x3 << SpacePacketSubfields::SeqFlagsOffset);  // Sequence Flags = 0b11 (unsegmented) & unused Seq count
    header.set_packetDataLength(lengthToken);
    // Serialize header and idle data into the frame
    serializer.serializeFrom(header);
    for (U16 i = static_cast<U16>(startIndex + SpacePacketHeader::SERIALIZED_SIZE); i < endIndex; i++) {
        serializer.serializeFrom(IDLE_DATA_PATTERN);  // Idle data
    }
}
}  // namespace Ccsds
}  // namespace Svc
