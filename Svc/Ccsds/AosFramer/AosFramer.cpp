// ======================================================================
// \title  AosFramer.cpp
// \author Will MacCormack (Aos Modifications)
// \brief  cpp file for AosFramer component implementation class
// \details modified from thomas-bc's TmFramer
// ======================================================================

#include "Svc/Ccsds/AosFramer/AosFramer.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {

namespace Ccsds {
// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AosFramer ::AosFramer(const char* const compName) : AosFramerComponentBase(compName) {
    // Default to FECF on, Max Sized if you don't override w/ another configure call
    configure(ComCfg::AosMaxFrameFixedSize, true);
}

AosFramer ::~AosFramer() {}

void AosFramer::configure(const U32 fixedFrameSize, const bool frameErrorControlField, const U8 idlePvns) {
    // fixedFrameSize must be less than or equal to the maximum defined in ComCfg.fpp
    FW_ASSERT(fixedFrameSize <= ComCfg::AosMaxFrameFixedSize, static_cast<FwAssertArgType>(fixedFrameSize));

    // AOS Frame Fixed Size must be at least large enough to hold header, trailer and data
    FW_ASSERT(fixedFrameSize > AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE +
                                   (frameErrorControlField ? AOSTrailer::SERIALIZED_SIZE : 0),
              static_cast<FwAssertArgType>(fixedFrameSize));

    // AOS Framer must be provided with a protocol to use for Idle Packets
    // Currently, only SPP idle packing is supported
    // EPP is more optimal, however
    FW_ASSERT(idlePvns & PvnBitfield::SPP_MASK, static_cast<FwAssertArgType>(idlePvns));

    // FECF is constant for a given Physical Channel during a Mission Phase (4.1.6.1.3)
    this->m_fecf = frameErrorControlField;

    // For each vc, init the buffer objects
    for (U8 ind = 0; ind < sizeof(this->m_vcs) / sizeof(AosVc); ind++) {
        AosVc& currentVc = this->m_vcs[ind];

        // Write the index for mapping a vc struct onto an output port
        currentVc.vc_struct_index = ind;
        currentVc.frame.buffer = {currentVc.frame.backer, fixedFrameSize};
        // Set the bitmask of PVNs to use for idle packets
        currentVc.idle_packet_types = idlePvns;
    }
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void AosFramer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // Get the VC Struct for this buffer
    const U8 vcId = context.get_vcId();
    AosVc& currentVc = this->get_vc_struct(context);
    FW_ASSERT(currentVc.virtualChannelId == vcId);

    // Ensure rest of the com stack is complying with the [communications adapter
    // interface](docs/reference/communication-adapter-interface.md)
    FW_ASSERT(currentVc.frame.state == BufferOwnershipState::OWNED,
              static_cast<FwAssertArgType>(currentVc.frame.state));

    // AOS Header & M_PDU Header
    // If this is the first fresh packet, set the M_PDU firstHeaderPointer
    if (!currentVc.past_first_fresh_packet) {
        // setup our headers if we have a packet + context ready to go
        setup_header(context);
        setup_m_pdu_header(context);

        // We now will only work with fresh packets for the rest of this frame
        currentVc.past_first_fresh_packet = true;
    }

    // Pack this packet
    pack_pad_send(data, context);
}

void AosFramer::compute_and_inject_fecf(AosVc& currentVc) {
    // -------------------------------------------------
    // Trailer (CRC)
    // -------------------------------------------------
    AOSTrailer trailer;

    const FwSizeType fecfStart = currentVc.frame.buffer.getSize() - AOSTrailer::SERIALIZED_SIZE;
    // Compute CRC over the entire frame buffer minus the FECF trailer (Standard 4.1.6)
    U16 crc = Ccsds::Utils::CRC16::compute(currentVc.frame.buffer.getData(), static_cast<U32>(fecfStart));
    // Set the Frame Error Control Field (FECF)
    trailer.set_fecf(crc);
    // Move the serializer pointer to the end of the location where the trailer will be
    // serialized
    auto frameSerializer = currentVc.frame.buffer.getSerializer();
    Fw::SerializeStatus status = frameSerializer.moveSerToOffset(fecfStart);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    status = frameSerializer.serializeFrom(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
}

void AosFramer ::comStatusIn_handler(FwIndexType portNum, Fw::Success& condition) {
    // We just ask upstream for more packets
    // comQueue decides to which VCs to allocate comStatus success
    if (this->isConnected_comStatusOut_OutputPort(portNum)) {
        // Forward the comStatus upstream
        this->comStatusOut_out(portNum, condition);
    }
}

void AosFramer ::dataReturnIn_handler(FwIndexType portNum,
                                      Fw::Buffer& frameBuffer,
                                      const ComCfg::FrameContext& context) {
    // Get the VC Struct for this buffer
    AosVc& currentVc = this->get_vc_struct(context);

    // Assert that the returned buffer is the member, and set ownership state
    FW_ASSERT(buffer_belongs(frameBuffer, currentVc.frame.backer, sizeof(currentVc.frame.backer)));
    currentVc.frame.state = BufferOwnershipState::OWNED;

    // If we have an outstanding packet from the prior frame, pack it
    if (currentVc.outstanding.packet.isValid()) {
        this->pack_pad_send(currentVc.outstanding.packet, currentVc.outstanding.context, currentVc.outstanding.offset);
    }
}

AosFramer::AosVc& AosFramer ::get_vc_struct(const ComCfg::FrameContext& context) {
    // MultiVc support would require looking up a vc struct ind given the vcIndex
    // (unless you force them to be continuous and start at zero)
    const U8 ind = 0;
    AosVc& currentVc = this->m_vcs[ind];

    // Ensure configure was called
    FW_ASSERT(currentVc.vc_struct_index == ind);
    FW_ASSERT(currentVc.virtualChannelId == context.get_vcId());
    return currentVc;
}

void AosFramer ::setup_header(const ComCfg::FrameContext& context) {
    // Get the VC Struct for this vc
    AosVc& currentVc = this->get_vc_struct(context);

    // -----------------------------------------------
    // Header
    // -----------------------------------------------
    AOSHeader header;

    // GVCID (Global Virtual Channel ID) (Standard 4.1.2.2 and 4.1.2.3)
    U16 globalVcId = static_cast<U16>(context.get_vcId() << AOSHeaderSubfields::virtualChannelIdOffset);
    globalVcId |= static_cast<U16>((ComCfg::SpacecraftId & 0x00FF) << AOSHeaderSubfields::spacecraftIdLsbOffset);
    globalVcId |= static_cast<U16>((Tfvn::AOS & 0x3) << AOSHeaderSubfields::frameVersionOffset);

    // Virtual Channel Frame Count (4.1.2.4)
    U32 frameCountAndSignaling = static_cast<U32>((currentVc.virtualFrameCount & 0x00FFFFFFU)
                                                  << static_cast<FwSizeType>(AOSHeaderSubfields::vcFrameCountOffset));

    // Virtual Channel Frame Count Cycle Use Flag (4.1.2.5.3)
    frameCountAndSignaling |= static_cast<U32>(1 << AOSHeaderSubfields::cycleCountFlagOffset);

    // Spacecraft ID MSB (4.1.2.5.4)
    frameCountAndSignaling |=
        static_cast<U32>((ComCfg::SpacecraftId & 0x0300) >> (8 - AOSHeaderSubfields::spacecraftIdMsbOffset));

    // Virtual Channel Frame Cycle Count (4.1.2.5.5)
    frameCountAndSignaling |= static_cast<U32>((currentVc.virtualFrameCount & 0x0F000000) >> 24);

    header.set_globalVcId(globalVcId);
    header.set_frameCountAndSignaling(frameCountAndSignaling);

    // Perform the modulo at serialization time makes vc cycle count easier to make optional
    currentVc.virtualFrameCount++;  // U24 base Frame Count
                                    // Extended by 4 bit VC Cycle Count
                                    // intended to wrap around (modulo 268,435,456)

    // -----------------------------------------------
    // Write Header
    // -----------------------------------------------
    Fw::SerializeStatus status;
    // Use our member Fw::Buffer
    auto frameSerializer = currentVc.frame.buffer.getSerializer();

    status = frameSerializer.moveSerToOffset(0);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    status = frameSerializer.serializeFrom(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
}

void AosFramer ::setup_m_pdu_header(const ComCfg::FrameContext& context, bool no_fresh) {
    // Get the VC Struct for this vc
    AosVc& currentVc = this->get_vc_struct(context);

    M_PDUHeader muxedPdu;
    if (no_fresh) {
        // All Ones since no packets start in this frame (no fresh packets)
        muxedPdu.set_firstHeaderPointer(0xFFFF);
    } else {
        // Called at first fresh packet (not a packet tail)
        // So current payload offset is the pointer to first packet header
        muxedPdu.set_firstHeaderPointer(currentVc.current_payload_offset);
    }

    auto frameSerializer = currentVc.frame.buffer.getSerializer();
    Fw::SerializeStatus status = frameSerializer.moveSerToOffset(AOSHeader::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    frameSerializer.serializeFrom(muxedPdu);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
}

FwSizeType AosFramer::get_min_size() {
    return AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE + (this->m_fecf ? AOSTrailer::SERIALIZED_SIZE : 0);
}

bool AosFramer::buffer_belongs(Fw::Buffer& buffer, U8 const* start, FwSizeType size) {
    return (buffer.getData() >= start && buffer.getData() < start + size);
}

void AosFramer::check_and_send_vc(AosFramer::AosVc& currentVc) {
    const FwSizeType maxPayload = currentVc.frame.buffer.getSize() - get_min_size();

    // Check if we've filled up the M_PDU payload
    if (currentVc.current_payload_offset == maxPayload) {
        // Write the headers if we haven't already (continuing packet only)
        if (!currentVc.past_first_fresh_packet) {
            // Setup the AOS header right before we send with the context cached in outstanding
            setup_header(currentVc.outstanding.context);

            // Setup the M_PDU header w/ all zero (only continuing packets)
            setup_m_pdu_header(currentVc.outstanding.context, true);
        }

        // Compute Trailer
        if (this->m_fecf) {
            compute_and_inject_fecf(currentVc);
        }

        // Ensure we aren't double sending
        FW_ASSERT(currentVc.frame.state == BufferOwnershipState::OWNED);
        currentVc.frame.state = BufferOwnershipState::NOT_OWNED;

        // Clean up our per frame vc values
        currentVc.current_payload_offset = 0;
        currentVc.past_first_fresh_packet = false;

        // Write the completed frame
        this->dataOut_out(0, currentVc.frame.buffer, currentVc.outstanding.context);
    } else if (this->isConnected_comStatusOut_OutputPort(currentVc.vc_struct_index)) {
        // We got more room, so ask for more bytes
        Fw::Success condition = Fw::Success::SUCCESS;
        this->comStatusOut_out(currentVc.vc_struct_index, condition);
    }
}

void AosFramer ::pack_pad_send(Fw::Buffer& data, const ComCfg::FrameContext& context, FwSizeType dataOffset) {
    // Pack this packet into the M_PDU
    pack_packet(data, context, dataOffset);

    // Get the VC Struct for this buffer
    AosVc& currentVc = this->get_vc_struct(context);

    // Pack with idle packets if sendNow and not full already
    // TODO: Add configurable time elapsed & buffer remaining based idle packing
    if (context.get_sendNow()) {
        // Compute some sizes to check if we've got space left to pad
        const FwSizeType min_size = get_min_size();
        const FwSizeType maxPayload = currentVc.frame.buffer.getSize() - min_size;

        if (currentVc.current_payload_offset < maxPayload) {
            // As per TM Standard 4.2.2.5, fill the rest of the data field with an Idle Packet
            fill_with_idle_packet(currentVc, context);
        }
    }

    // Send the frame if we've filled it
    check_and_send_vc(currentVc);
}

void AosFramer ::pack_packet(Fw::Buffer& data, const ComCfg::FrameContext& context, FwSizeType dataOffset) {
    // Ensure the packet is starting within the frame
    const FwSizeType min_size = get_min_size();

    // Get the VC Struct for this buffer
    AosVc& currentVc = this->get_vc_struct(context);

    const FwSizeType maxPayload = currentVc.frame.buffer.getSize() - min_size;
    const FwSizeType bytesAvailable = maxPayload - currentVc.current_payload_offset;
    FW_ASSERT(bytesAvailable < currentVc.frame.buffer.getSize(),
              static_cast<FwAssertArgType>(min_size + currentVc.current_payload_offset));

    // -------------------------------------------------
    // Payload Packet
    // -------------------------------------------------
    Fw::SerializeStatus status;
    // Use our member Fw::Buffer
    auto frameSerializer = currentVc.frame.buffer.getSerializer();
    status = frameSerializer.moveSerToOffset(START_OF_PAYLOAD + currentVc.current_payload_offset);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    const U8* dataStart = data.getData() + dataOffset;
    // min of (remaining bytes in buffer and available bytes in frame)
    FwSizeType dataSize = data.getSize() - dataOffset;

    // Determine if we can write the whole packet or not
    if (dataSize <= bytesAvailable) {
        // Whole packet fits, no need to store to outstanding
        currentVc.outstanding.offset = 0;
    } else {
        // We'll only write a subset (clamp our write to the available size)
        dataSize = bytesAvailable;

        // We'll pick up serialization from here later
        currentVc.outstanding.offset = dataOffset + dataSize;
        currentVc.outstanding.packet = data;
    }

    status = frameSerializer.serializeFrom(dataStart, dataSize, Fw::Serialization::OMIT_LENGTH);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Shift our offset into the M_PDU payload region by how many bytes we wrote
    currentVc.current_payload_offset = static_cast<U16>(currentVc.current_payload_offset + dataSize);

    // Context must always be present once any packet payload bytes are written
    currentVc.outstanding.context = context;

    // Return the buffer if no bytes are outstanding
    if (currentVc.outstanding.offset == 0) {
        // Return the buffer if this isn't the SPP Idle buff
        if (!buffer_belongs(data, currentVc.spp_idle.backer, sizeof(currentVc.spp_idle.backer))) {
            this->dataReturnOut_out(0, data,
                                    context);  // return ownership of the original data buffer
        }

        currentVc.outstanding.packet = {};
        currentVc.outstanding.offset = 0;
    }
}

void AosFramer ::serialize_idle_spp_packet(Fw::SerializeBufferBase& serializer, U16 length) {
    // APID to use for this Idle Packet
    constexpr U16 idleApid = static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET);

    // Length token is defined as the number of bytes of payload data minus 1
    const U16 lengthToken = static_cast<U16>(length - SpacePacketHeader::SERIALIZED_SIZE - 1);

    SpacePacketHeader header;
    header.set_packetIdentification(idleApid);
    header.set_packetSequenceControl(
        0x3 << SpacePacketSubfields::SeqFlagsOffset);  // Sequence Flags = 0b11 (unsegmented) & unused Seq count
    header.set_packetDataLength(lengthToken);
    // Serialize header into frame
    Fw::SerializeStatus status = serializer.serializeFrom(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // Fill with idle pattern
    for (U16 i = static_cast<U16>(SpacePacketHeader::SERIALIZED_SIZE); i < length; i++) {
        status = serializer.serializeFrom(SPP_IDLE_DATA_PATTERN);  // Idle data
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    }
}

void AosFramer ::fill_with_idle_packet(AosVc& vc, const ComCfg::FrameContext& context) {
    // Bytes that aren't actual PDUs
    const U16 overhead = START_OF_PAYLOAD + (m_fecf ? AOSTrailer::SERIALIZED_SIZE : 0);
    // Bytes for all the PDUs in this VC Frame
    FW_ASSERT(vc.frame.buffer.getSize() > 0, static_cast<FwAssertArgType>(vc.frame.buffer.getSize()));
    FW_ASSERT(vc.frame.buffer.getSize() < std::numeric_limits<U16>::max(),
              static_cast<FwAssertArgType>(vc.frame.buffer.getSize()));
    const U16 pduSize = static_cast<U16>(vc.frame.buffer.getSize() - overhead);

    // How many bytes are left over
    const U16 idlePacketSize = static_cast<U16>(pduSize - vc.current_payload_offset);

    // Grab a serializer @ the current offset
    auto frameSerializer = vc.frame.buffer.getSerializer();
    Fw::SerializeStatus status = frameSerializer.moveSerToOffset(START_OF_PAYLOAD + vc.current_payload_offset);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    // If there wasn't a fresh packet before this
    // set the M_PDU firstHeaderPointer
    if (!vc.past_first_fresh_packet) {
        // setup our headers if we have a packet + context ready to go
        setup_header(context);
        setup_m_pdu_header(context);

        // This idle packet is the fresh packet
        vc.past_first_fresh_packet = true;
    }

    // Use EPP if we can (solves for all sizes)
    if (vc.idle_packet_types & PvnBitfield::EPP_MASK) {
        // TODO: Serialize an EPP of the right size
        FW_ASSERT(1);
    }
    // While we are using only SPP, we have to comply w/ the min SPP packet size
    // We'll stripe this packet onto the next frame of this VC if we have to
    else if (idlePacketSize < 7) {
        // Serialize the Idle packet into the spp_idle_backer

        // Make sure we aren't overwriting a packet fragment
        FW_ASSERT(!vc.outstanding.packet.isValid());

        // Setup the PDU to point at our idle buffer
        vc.outstanding.packet = {vc.spp_idle.backer, MIN_SPP_LENGTH};

        // Write the SPP Idle to the outstanding packet
        auto pduSerializer = vc.outstanding.packet.getSerializer();
        serialize_idle_spp_packet(pduSerializer, MIN_SPP_LENGTH);

        // Remove the sendNow param
        // The idle fragment that spills onto the next doesn't need to be sentNow
        ComCfg::FrameContext filtered_context = context;
        filtered_context.set_sendNow(false);

        // Use the normal pack command since we will have leftovers
        // There won't be an outstanding packet since we'd use those bytes instead
        pack_packet(vc.outstanding.packet, filtered_context);
    } else {
        // Serialize an idle packet right into the frame
        serialize_idle_spp_packet(frameSerializer, idlePacketSize);

        // Increment the offset since we serialized directly into the frame
        vc.current_payload_offset = static_cast<U16>(vc.current_payload_offset + idlePacketSize);
    }
}

}  // namespace Ccsds
}  // namespace Svc
