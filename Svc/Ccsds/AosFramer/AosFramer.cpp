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

    this->m_fecf = frameErrorControlField;

    // For each vc, init the buffer objects
    for (AosVc currentVc& : this->m_vcs) {
        currentVc.frameBuffer = {currentVc.frameBufferBacker, fixedFixedSize};
    }
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AosFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // Ensure rest of the com stack is complying with the [communications adapter
    // interface](docs/reference/communication-adapter-interface.md)
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED, static_cast<FwAssertArgType>(this->m_bufferState));

    // Get the VC Struct for this buffer
    AosVc currentVc& = this->m_vcs[this->get_vc_index(context.vdId)];
    FW_ASSERT(currentVc.virtualChannelId == context.get_vcId());

    // AOS Header & M_PDU Header
    // If this is the first fresh packet, set the M_PDU firstHeaderPointer
    // dataOffset is non-zero for packets that are continuing
    if (!currentVc.m_past_first_fresh_packet) {
        // TODO: setup a new header if we have a packet + context ready to go
        setup_header(context);
        setup_m_pdu_header(context);

        // We now will only work with fresh packets for the rest of this frame
        currentVc.m_past_first_fresh_packet = true;
    }

    // Pack this packet
    this->pack_packet(data, context);

    if (context.get_sendNow()) {
        // TODO: Decide when to pack w/ idle
        // Potentially add a configurable time/remaining bytes threshold
        // As per TM Standard 4.2.2.5, fill the rest of the data field with an Idle Packet
        this->fill_with_idle_packet(currentVc);
    }

    // TODO: Check if offset == end && call into compute_trailer
    if (currentVc.current_payload_offset ==) {
    } else if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        // We got more room, so ask for my bytes
        Fw::Success cesondition = Fw::Success::SUCCESS;
        this->comStatusOut_out(portNum, condition);
    }
}
// -------------------------------------------------
// Trailer (CRC)
// -------------------------------------------------
TMTrailer trailer;
// Compute CRC over the entire frame buffer minus the FECF trailer (Standard 4.1.6)
U16 crc = Ccsds::Utils::CRC16::compute(frameBuffer.getData(), sizeof(this->m_frameBuffer) - TMTrailer::SERIALIZED_SIZE);
// Set the Frame Error Control Field (FECF)
trailer.set_fecf(crc);
// Move the serializer pointer to the end of the location where the trailer will be
// serialized
frameSerializer.moveSerToOffset(ComCfg::TmFrameFixedSize - TMTrailer::SERIALIZED_SIZE);
status = frameSerializer.serializeFrom(trailer);
FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

this->m_bufferState = BufferOwnershipState::NOT_OWNED;
this->dataOut_out(0, frameBuffer, context);
}

void AosFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    // Ensure rest of the com stack is complying with the [communications adapter
    // interface](docs/reference/communication-adapter-interface.md)
    // ComStatus must come in after the dataReturnIn
    FW_ASSERT(this->m_bufferState == BufferOwnershipState::OWNED, static_cast<FwAssertArgType>(this->m_bufferState));

    // If we have a packet ready to go, write it to output
    // TODO: Check for packet that's ready
    if (condition == Fw::Success::SUCCESS && false) {
    } else if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        // Otherwise, forward the comStatus upstream
        this->comStatusOut_outfj(portNum, condition);
    }
}

void AosFramer ::dataReturnIn_handler(FwIndexType portNum,
                                      Fw::Buffer& frameBuffer,
                                      const ComCfg::FrameContext& context) {
    // Get the VC Struct for this buffer
    AosVc currentVc& = this->m_vcs[this->get_vc_index(context.vdId)];
    FW_ASSERT(currentVc.virtualChannelId == context.get_vcId());

    // Assert that the returned buffer is the member, and set ownership state
    FW_ASSERT(frameBuffer.getData() >= &currentVc.frameBufferBacker[0]);
    FW_ASSERT(frameBuffer.getData() < &currentVc.frameBufferBacker[0] + sizeof(currentVc.frameBufferBacker));
    currentVc.bufferState = BufferOwnershipState::OWNED;

    // Clean up our per frame vc values
    currentVc.m_current_payload_offset = 0;
    currentVc.m_past_first_fresh_packet = false;

    // If we have an outstanding packet from the prior frame, pack it
    // TODO: Is this enough
    if (currentVc.outstanding_packet.isValid()) {
        this->pack_packet(currentVc.outstanding_packet, currentVc.outstanding_context, currentVc.outstanding_offset);
    }
}

void AosFramer ::get_vc_index(ComCfg::FrameContext& context) {
    // Always returns 0 for now.
    // Eventually setup a table per AOS Framer and map context VC IDs into indicies in the Array of Structs
    return 0;
}

void AosFramer ::setup_header(ComCfg::FrameContext& context) {
    FwIndexType vcIndex = this->get_vc_index(context.get_vcId());

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

    // -----------------------------------------------
    // Write Header
    // -----------------------------------------------
    Fw::SerializeStatus status;
    // Use our member Fw::Buffer
    auto frameSerializer = this->m_frameBuffer.getSerializer();
    status = frameSerializer.serializeFrom(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
}

void AosFramer ::setup_m_pdu_header(ComCfg::FrameContext& context) {
    // Get the VC Struct for this vc
    AosVc currentVc& = this->m_vcs[this->get_vc_index(context.vdId)];
    FW_ASSERT(currentVc.virtualChannelId == context.get_vcId());

    M_PDUHeader muxedPdu;

    muxedPdu.set_firstHeaderPointer(this->m_current_payload_offset);

    status = frameSerializer.moveSerToOffset(AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    frameSerializer.serializeFrom(muxedPdu);
}

void AosFramer ::pack_packet(Fw::Buffer& data, ComCfg::FrameContext& context, FwSizeType dataOffset) {
    // Ensure the packet is starting within the frame
    const FwSizeType min_size =
        AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE + (m_fecf ? AOSTrailer::SERIALIZED_SIZE : 0);

    // Get the VC Struct for this vc
    AosVc currentVc& = this->m_vcs[this->get_vc_index(context.get_vdId())];
    FW_ASSERT(currentVc.virtualChannelId == context.get_vcId());

    const FwSizeType bytesAvailable = currentVc.frameBuffer.getSize() - (min_size + currentVc.current_payload_offset);
    FW_ASSERT(bytesAvailable < currentVc.frameBuffer.getSize(),
              static_cast<FwAssertArgType>(min_size + currentVc.current_payload_offset));

    // -------------------------------------------------
    // Payload Packet
    // -------------------------------------------------
    Fw::SerializeStatus status;
    // Use our member Fw::Buffer
    auto frameSerializer = currentVc.frameBuffer.getSerializer();

    frameSerializer.moveSerToOffset(AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE +
                                    currentVc.current_payload_offset);

    const U8* dataStart = data.getData() + currentVc.outstanding_offset;
    // min of (remaining bytes in buffer and available bytes in frame)
    U8* dataSize = data.getSize() - currentVc.outstanding_offset;

    // Determine if we can write the whole packet or not
    if (dataSize <= bytesAvailable) {
        dataSize = bytesAvailable;
        // Clear this out; we're done w/ this packet
        currentVc.outstanding_offset += dataSize;

    } else {
        // Clear out the outstanding; we're done w/ this packet after we serialize
        currentVc.outstanding_offset = 0;
    }

    status = frameSerializer.serializeFrom(dataStart, dataSize, Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    currentVc.m_current_payload_offset += dataSize;

    // Return the buffer if no bytes are outstanding
    if (currentVc.outstanding_offset == 0) {
        currentVc.outstanding_packet = {};
        currentVc.outstanding_context = {};

        this->dataReturnOut_out(0, data,
                                context);  // return ownership of the original data buffer
    }
}

void AosFramer ::serialize_idle_spp_packet(Fw::SerializeBufferBase& serializer, FwSizeType length) {
    // Length token is defined as the number of bytes of payload data minus 1
    const U16 lengthToken = static_cast<U16>(idlePacketSize - SpacePacketHeader::SERIALIZED_SIZE - 1);

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

    status = frameSerializer.serializeFrom(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
}

void AosFramer ::fill_with_idle_packet() {
    constexpr U16 endIndex = ComCfg::TmFrameFixedSize - TMTrailer::SERIALIZED_SIZE;
    constexpr U16 idleApid = static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET);
    const U16 startIndex = static_cast<U16>(serializer.getSize());
    U16 idlePacketSize = static_cast<U16>(endIndex - startIndex);

    // While we are using only SPP, we have to comply w/ the min SPP packet size
    // We'll stripe this packet onto the next frame of this VC if we have to
    if (idlePacketSize < 7) {
        idlePacketSize = 7;
        // Serialize the Idle packet into the spp_idle_backer
        // TODO: Call into pack_packet w/ the idle packet
    } else if (false) {
        // Switch to an EPP Idle packet to fill the small gap
        // TODO: Add a configurable on EPP based idle packing option
    } else {
        // Serialize an idle packet right into the frame

        Fw::SerializeStatus status;
        // Use our member Fw::Buffer
        Fw::SerialBufferBase frameSerializer = this->m_frameBuffer.getSerializer();
    }
}

}  // namespace Ccsds
}  // namespace Svc
