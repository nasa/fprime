// ======================================================================
// \title  AosDeframer.cpp
// \author Will MacCormack
// \brief  cpp file for AosDeframer component implementation class
//
// Deframer for the AOS Space Data Link Protocol per CCSDS 732.0-B-5.
// Supports M_PDU data field service with:
// - Frame Error Control Field (FECF) validation (Section 4.1.6)
// - Space Packet Protocol (SPP) extraction (CCSDS 133.0-B-2)
// - Encapsulation Packet Protocol (EPP) extraction (CCSDS 133.1-B-3)
// ======================================================================

#include "Svc/Ccsds/AosDeframer/AosDeframer.hpp"
#include "Svc/Ccsds/Types/EppPacketTypeEnumAc.hpp"
#include "Svc/Ccsds/Types/SpacePacketHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "config/FppConstantsAc.hpp"

namespace Svc {
namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

AosDeframer::AosDeframer(const char* const compName)
    : AosDeframerComponentBase(compName),
      m_fixedFrameSize(0),
      m_fecfEnabled(true),
      m_spacecraftId(ComCfg::SpacecraftId),
      m_crcErrorCount(0) {
    // Initialize VC struct
    for (U8 vcInd = 0; vcInd < AosDeframer_NumVcs; vcInd++) {
        m_vcs[vcInd].vcStructIndex = 0;
        m_vcs[vcInd].virtualChannelId = 0;
        m_vcs[vcInd].pvnMask = PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK;
    }
}

AosDeframer::~AosDeframer() {}

void AosDeframer::configure(U32 fixedFrameSize, bool frameErrorControlField, U16 spacecraftId, U8 vcId, U8 pvnMask) {
    // Validate frame size is within bounds
    FW_ASSERT(fixedFrameSize <= ComCfg::AosMaxFrameFixedSize, static_cast<FwAssertArgType>(fixedFrameSize));

    // Frame must be large enough for header + M_PDU header + optional trailer
    const FwSizeType minSize = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE +
                               (frameErrorControlField ? AOSTrailer::SERIALIZED_SIZE : 0);
    FW_ASSERT(fixedFrameSize > minSize, static_cast<FwAssertArgType>(fixedFrameSize));

    // Spacecraft ID is 10 bits (per CCSDS 732.0-B-5 Section 4.1.2.2)
    FW_ASSERT((spacecraftId & 0xFC00) == 0, static_cast<FwAssertArgType>(spacecraftId));

    // Virtual Channel ID is 6 bits (per CCSDS 732.0-B-5 Section 4.1.2.3)
    FW_ASSERT((vcId & 0xC0) == 0, static_cast<FwAssertArgType>(vcId));

    // pvnMask must only contain valid PVN bits and at least one must be set
    FW_ASSERT((pvnMask & PvnBitfield::VALID_MASK) != 0, static_cast<FwAssertArgType>(pvnMask));
    FW_ASSERT((pvnMask & ~PvnBitfield::VALID_MASK) == 0, static_cast<FwAssertArgType>(pvnMask));

    m_fixedFrameSize = fixedFrameSize;
    m_fecfEnabled = frameErrorControlField;
    m_spacecraftId = spacecraftId;

    // Zero out FECF error counter on (re)configure
    m_crcErrorCount = 0;

    // Populate the (single) VC struct
    m_vcs[0].virtualChannelId = vcId;
    m_vcs[0].pvnMask = pvnMask;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void AosDeframer::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // Per CCSDS 732.0-B-5, AOS frames are fixed-size
    // Verify we have received a complete frame
    FW_ASSERT(m_fixedFrameSize > 0, static_cast<FwAssertArgType>(m_fixedFrameSize));

    if (data.getSize() < m_fixedFrameSize) {
        this->log_WARNING_HI_InvalidFrameLength(data.getSize(), m_fixedFrameSize);
        if (this->isConnected_errorNotify_OutputPort(0)) {
            this->errorNotify_out(0, Ccsds::FrameError::AOS_INVALID_LENGTH);
        }
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Create a mutable context for extracted packet info
    ComCfg::FrameContext packetContext = context;

    // Parse and validate the AOS Primary Header (Section 4.1.2)
    // Note: parseAndValidateHeader handles warning events and errorNotify for header failures.
    if (!this->parseAndValidateHeader(data, packetContext)) {
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Look up the VC struct for this frame's VCID
    AosDeframerVc& vc = this->getVcStruct(packetContext);

    // Validate FECF if enabled (Section 4.1.6)
    if (m_fecfEnabled && !this->validateFecf(data)) {
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Update telemetry
    this->tlmWrite_FrameCount(++vc.frameCount);

    // Extract packets from the M_PDU data zone
    this->extractPackets(vc, data, packetContext);

    // Return the frame buffer
    this->dataReturnOut_out(0, data, context);
}

void AosDeframer::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    // Forward the buffer return to the upstream component
    this->dataReturnOut_out(0, fwBuffer, context);
}

// ----------------------------------------------------------------------
// Private helper methods
// ----------------------------------------------------------------------

AosDeframer::AosDeframerVc& AosDeframer::getVcStruct(const ComCfg::FrameContext& context) {
    // TODO: Implement multi-VC support by mapping context.vcId to the correct VC struct
    (void)context;
    return m_vcs[0];
}

bool AosDeframer::parseAndValidateHeader(Fw::Buffer& data, ComCfg::FrameContext& context) {
    // Deserialize the AOS Primary Header (per CCSDS 732.0-B-5 Section 4.1.2)
    AOSHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserializeTo(header);
    if (status != Fw::FW_SERIALIZE_OK) {
        // Buffer too small to contain a valid AOS primary header
        this->log_WARNING_HI_InvalidFrameLength(data.getSize(), m_fixedFrameSize);
        if (this->isConnected_errorNotify_OutputPort(0)) {
            this->errorNotify_out(0, Ccsds::FrameError::AOS_INVALID_LENGTH);
        }
        return false;
    }

    // Extract Transfer Frame Version Number (Section 4.1.2.2.2)
    // AOS uses Tfvn::AOS = 0x1 ('01' binary)
    U8 tfvn = static_cast<U8>((header.get_globalVcId() & AOSHeaderSubfields::frameVersionMask) >>
                              AOSHeaderSubfields::frameVersionOffset);
    if (tfvn != static_cast<U8>(Tfvn::AOS)) {
        this->log_WARNING_HI_InvalidTfvn(tfvn, static_cast<U8>(Tfvn::AOS));
        if (this->isConnected_errorNotify_OutputPort(0)) {
            this->errorNotify_out(0, Ccsds::FrameError::AOS_INVALID_VERSION);
        }
        return false;
    }

    // Extract Spacecraft ID (Section 4.1.2.2)
    // SCID is split: 8 LS bits in globalVcId, 2 MS bits in signaling field
    U8 signalingByte = static_cast<U8>(header.get_frameCountAndSignaling() & 0xFF);
    U16 spacecraftId = static_cast<U16>(((header.get_globalVcId() & AOSHeaderSubfields::spacecraftIdLsbMask) >>
                                         AOSHeaderSubfields::spacecraftIdLsbOffset) |
                                        (static_cast<U16>((signalingByte & AOSHeaderSubfields::spacecraftIdMsbMask) >>
                                                          AOSHeaderSubfields::spacecraftIdMsbOffset)
                                         << 8));

    if (spacecraftId != m_spacecraftId) {
        this->log_WARNING_LO_InvalidSpacecraftId(spacecraftId, m_spacecraftId);
        if (this->isConnected_errorNotify_OutputPort(0)) {
            this->errorNotify_out(0, Ccsds::FrameError::AOS_INVALID_SCID);
        }
        return false;
    }

    // Extract Virtual Channel ID (Section 4.1.2.3)
    U8 vcId = static_cast<U8>(header.get_globalVcId() & AOSHeaderSubfields::virtualChannelIdMask);
    if (vcId != m_vcs[0].virtualChannelId) {
        this->log_ACTIVITY_LO_InvalidVcId(vcId, m_vcs[0].virtualChannelId);
        if (this->isConnected_errorNotify_OutputPort(0)) {
            this->errorNotify_out(0, Ccsds::FrameError::AOS_INVALID_VCID);
        }
        return false;
    }

    // Extract Virtual Channel Frame Count (Section 4.1.2.4)
    // 24 bits in the upper 3 bytes of frameCountAndSignaling
    U32 vcFrameCount = (header.get_frameCountAndSignaling() >> AOSHeaderSubfields::vcFrameCountOffset) &
                       AOSHeaderSubfields::vcFrameCountMask;

    // Extract VC Frame Count Cycle if in use (Section 4.1.2.5.3)
    if ((signalingByte & AOSHeaderSubfields::cycleCountFlagMask) != 0) {
        U8 vcFrameCountCycle = signalingByte & AOSHeaderSubfields::vcFrameCountCycleMask;
        // Extend the 24-bit frame count with the 4-bit cycle count
        vcFrameCount |= static_cast<U32>(vcFrameCountCycle) << 24;
    }

    // Store VC frame count in the VC struct for reference (e.g., gap detection)
    m_vcs[0].vcFrameCount = vcFrameCount;

    // Update context with extracted values
    context.set_vcId(vcId);

    return true;
}

bool AosDeframer::validateFecf(Fw::Buffer& data) {
    // Per CCSDS 732.0-B-5 Section 4.1.6, FECF is a 16-bit CRC
    // computed over all preceding bits in the frame

    const FwSizeType crcDataLen = m_fixedFrameSize - AOSTrailer::SERIALIZED_SIZE;
    U16 computedCrc = Ccsds::Utils::CRC16::compute(data.getData(), static_cast<U32>(crcDataLen));

    // Deserialize the trailer
    AOSTrailer trailer;
    auto deserializer = data.getDeserializer();
    deserializer.moveDeserToOffset(crcDataLen);
    Fw::SerializeStatus status = deserializer.deserializeTo(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 transmittedCrc = trailer.get_fecf();
    if (transmittedCrc != computedCrc) {
        this->log_WARNING_HI_InvalidFecf(transmittedCrc, computedCrc);
        if (this->isConnected_errorNotify_OutputPort(0)) {
            this->errorNotify_out(0, Ccsds::FrameError::AOS_INVALID_CRC);
        }
        this->tlmWrite_CrcErrorCount(++m_crcErrorCount);
        return false;
    }

    return true;
}

bool AosDeframer::appendToSpanningPacket(AosDeframerVc& vc, U8* data, FwSizeType size, ComCfg::FrameContext& context) {
    // Clamp copy to buffer capacity to avoid overflow
    FwSizeType bytesToCopy = FW_MIN(size, sizeof(vc.spanningPacket.buffer) - vc.spanningPacket.bytesReceived);
    ::memcpy(vc.spanningPacket.buffer + vc.spanningPacket.bytesReceived, data, bytesToCopy);
    vc.spanningPacket.bytesReceived += bytesToCopy;

    // Determine expected size once we have enough SPP header bytes
    if (vc.spanningPacket.expectedSize == 0 &&
        vc.spanningPacket.pvn == static_cast<U8>(ComCfg::Pvn::SPACE_PACKET_PROTOCOL) &&
        (vc.pvnMask & PvnBitfield::SPP_MASK) && vc.spanningPacket.bytesReceived >= SpacePacketHeader::SERIALIZED_SIZE) {
        U16 dataLength = static_cast<U16>((vc.spanningPacket.buffer[4] << 8) | vc.spanningPacket.buffer[5]);
        vc.spanningPacket.expectedSize = SpacePacketHeader::SERIALIZED_SIZE + dataLength + 1;
    }

    // Check if the spanning packet is now complete
    if (vc.spanningPacket.expectedSize > 0 && vc.spanningPacket.bytesReceived >= vc.spanningPacket.expectedSize) {
        // Spanning packet data is owned by vc.spanningPacket.buffer (persistent member)
        Fw::Buffer packetBuffer(vc.spanningPacket.buffer, vc.spanningPacket.expectedSize);

        ComCfg::FrameContext packetContext = context;
        if (vc.spanningPacket.pvn == static_cast<U8>(ComCfg::Pvn::SPACE_PACKET_PROTOCOL)) {
            packetContext.set_pvn(ComCfg::Pvn::SPACE_PACKET_PROTOCOL);
        } else {
            packetContext.set_pvn(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
        }

        this->dataOut_out(0, packetBuffer, packetContext);
        this->tlmWrite_PacketCount(++vc.packetCount);

        vc.spanningPacket.active = false;
        vc.spanningPacket.bytesReceived = 0;
        vc.spanningPacket.expectedSize = 0;
        return true;
    }

    return false;
}

void AosDeframer::extractPackets(AosDeframerVc& vc, Fw::Buffer& data, ComCfg::FrameContext& context) {
    // Parse M_PDU header (per CCSDS 732.0-B-5 Section 4.1.4.2.2)
    M_PDUHeader mpduHeader;
    auto deserializer = data.getDeserializer();
    deserializer.moveDeserToOffset(AOSHeader::SERIALIZED_SIZE);
    Fw::SerializeStatus status = deserializer.deserializeTo(mpduHeader);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 firstHeaderPointer = mpduHeader.get_firstHeaderPointer();

    // Calculate data zone boundaries
    const FwSizeType dataZoneStart = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE;
    const FwSizeType dataZoneEnd = m_fixedFrameSize - (m_fecfEnabled ? AOSTrailer::SERIALIZED_SIZE : 0);
    const FwSizeType dataZoneSize = dataZoneEnd - dataZoneStart;
    U8* dataZone = data.getData() + dataZoneStart;

    // Handle special First Header Pointer values (Section 4.1.4.2.2.4)
    if (firstHeaderPointer == M_PDUSubfields::FHP_IDLE_DATA_ONLY) {
        // Frame contains only idle data - reset any in-progress spanning packet
        this->log_ACTIVITY_LO_IdleFrame(vc.virtualChannelId);
        vc.spanningPacket.active = false;
        vc.spanningPacket.bytesReceived = 0;
        return;
    }

    // Handle continuation data (data before First Header Pointer)
    if (firstHeaderPointer == M_PDUSubfields::FHP_NO_PACKET_START) {
        // Entire data zone is continuation of previous packet
        if (vc.spanningPacket.active) {
            (void)this->appendToSpanningPacket(vc, dataZone, dataZoneSize, context);
        }
        // If no spanning packet active, this continuation data cannot be used
        return;
    }

    // There is continuation data before the first packet header
    if (firstHeaderPointer > 0 && vc.spanningPacket.active) {
        (void)this->appendToSpanningPacket(vc, dataZone, static_cast<FwSizeType>(firstHeaderPointer), context);
        vc.spanningPacket.active = false;
        vc.spanningPacket.bytesReceived = 0;
        vc.spanningPacket.expectedSize = 0;
    }

    // Move to first packet header
    FwSizeType currentOffset = firstHeaderPointer;

    // Extract packets starting at First Header Pointer
    for (; currentOffset < dataZoneSize;) {
        U8* packetStart = dataZone + currentOffset;
        FwSizeType remainingBytes = dataZoneSize - currentOffset;

        // Determine packet type from PVN (upper 3 bits of first byte)
        U8 pvn = getPacketVersion(packetStart[0]);

        FwSizeType packetSize = 0;

        if (pvn == static_cast<U8>(ComCfg::Pvn::SPACE_PACKET_PROTOCOL) && (vc.pvnMask & PvnBitfield::SPP_MASK)) {
            // Space Packet Protocol
            packetSize = extractSppPacket(vc, packetStart, remainingBytes, context);
        } else if (pvn == static_cast<U8>(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL) &&
                   (vc.pvnMask & PvnBitfield::EPP_MASK)) {
            // Encapsulation Packet Protocol
            packetSize = extractEppPacket(vc, packetStart, remainingBytes, context);
        } else {
            // Unknown or disabled packet type - stop processing this frame
            break;
        }

        if (packetSize == 0) {
            // Packet spans to next frame - save state
            vc.spanningPacket.active = true;
            vc.spanningPacket.pvn = pvn;
            vc.spanningPacket.bytesReceived = remainingBytes;
            vc.spanningPacket.expectedSize = 0;
            ::memcpy(vc.spanningPacket.buffer, packetStart, remainingBytes);
            break;
        }

        currentOffset += packetSize;
    }
}

FwSizeType AosDeframer::extractSppPacket(AosDeframerVc& vc,
                                         U8* payloadStart,
                                         FwSizeType payloadSize,
                                         ComCfg::FrameContext& context) {
    // Per CCSDS 133.0-B-2, Space Packet Header is 6 bytes
    if (payloadSize < SpacePacketHeader::SERIALIZED_SIZE) {
        return 0;  // Incomplete header - spans to next frame
    }

    // Parse packet data length from header (bytes 4-5)
    // Per CCSDS 133.0-B-2 Section 4.1.3.5.2, packet data length = (actual length - 1)
    U16 dataLength = static_cast<U16>((payloadStart[4] << 8) | payloadStart[5]);
    FwSizeType totalPacketSize = SpacePacketHeader::SERIALIZED_SIZE + dataLength + 1;

    if (payloadSize < totalPacketSize) {
        return 0;  // Incomplete packet - spans to next frame
    }

    // Check for idle packet (APID = 0x7FF per CCSDS 133.0-B-2)
    U16 apid = static_cast<U16>(((payloadStart[0] & 0x07) << 8) | payloadStart[1]);
    if (apid == static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET)) {
        // Skip idle packets, don't output
        return totalPacketSize;
    }

    // Create buffer pointing to the packet data within the incoming frame buffer
    Fw::Buffer packetBuffer(payloadStart, totalPacketSize);

    // Update context
    ComCfg::FrameContext packetContext = context;
    packetContext.set_pvn(ComCfg::Pvn::SPACE_PACKET_PROTOCOL);
    packetContext.set_apid(static_cast<ComCfg::Apid::T>(apid));

    // Extract sequence count from header (bytes 2-3, lower 14 bits)
    U16 seqControl = static_cast<U16>((payloadStart[2] << 8) | payloadStart[3]);
    U16 seqCount = seqControl & SpacePacketSubfields::SeqCountMask;
    packetContext.set_sequenceCount(seqCount);

    // Output the packet
    this->dataOut_out(0, packetBuffer, packetContext);
    this->tlmWrite_PacketCount(++vc.packetCount);

    return totalPacketSize;
}

FwSizeType AosDeframer::extractEppPacket(AosDeframerVc& vc,
                                         U8* payloadStart,
                                         FwSizeType payloadSize,
                                         ComCfg::FrameContext& context) {
    // Per CCSDS 133.1-B-3 Section 4.1, EPP minimum header is 1 byte
    if (payloadSize < 1) {
        return 0;
    }

    // Parse first byte
    U8 firstByte = payloadStart[0];
    U8 packetType = static_cast<U8>((firstByte & EPPSubfields::packetTypeMask) >> EPPSubfields::packetTypeOffset);

    FwSizeType totalPacketSize = 0;

    if (packetType == static_cast<U8>(EppPacketType::EncapsulationIdle)) {
        // Encapsulation Idle Packet per CCSDS 133.1-B-3 Section 4.1.3.2
        U8 lengthOfLength = firstByte & EPPSubfields::lengthOfLengthMask;

        if (lengthOfLength == 0) {
            // Fill packet - consumes rest of data zone
            return payloadSize;
        }

        // Validate and read length field
        if (payloadSize < static_cast<FwSizeType>(1 + lengthOfLength)) {
            return 0;  // Incomplete
        }

        // Read length field (big-endian)
        FwSizeType packetDataLength = 0;
        for (U8 i = 0; i < lengthOfLength; i++) {
            packetDataLength = (packetDataLength << 8) | payloadStart[1 + i];
        }

        totalPacketSize = 1 + lengthOfLength + packetDataLength;

        if (payloadSize < totalPacketSize) {
            return 0;  // Incomplete
        }

        // Idle packets are not output
        return totalPacketSize;

    } else {
        // Encapsulation Packet (data) per CCSDS 133.1-B-3 Section 4.1.3.1
        U8 protocolId = firstByte & EPPSubfields::protocolIdMask;

        if (protocolId <= 0x07) {
            // Standard encapsulation - next 2 bytes are length
            if (payloadSize < 3) {
                return 0;  // Incomplete header
            }

            U16 packetDataLength = static_cast<U16>((payloadStart[1] << 8) | payloadStart[2]);
            totalPacketSize = 3 + packetDataLength;
        } else {
            // Extended protocol ID (0x8-0xF) - variable structure
            // Treat as 2-byte length following
            if (payloadSize < 3) {
                return 0;
            }

            U16 packetDataLength = static_cast<U16>((payloadStart[1] << 8) | payloadStart[2]);
            totalPacketSize = 3 + packetDataLength;
        }

        if (payloadSize < totalPacketSize) {
            return 0;  // Incomplete packet
        }

        // Create buffer pointing to the packet data within the incoming frame buffer
        Fw::Buffer packetBuffer(payloadStart, totalPacketSize);

        // Update context
        ComCfg::FrameContext packetContext = context;
        packetContext.set_pvn(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);

        // Output the packet
        this->dataOut_out(0, packetBuffer, packetContext);
        this->tlmWrite_PacketCount(++vc.packetCount);

        return totalPacketSize;
    }
}

U8 AosDeframer::getPacketVersion(U8 firstByte) {
    // PVN is the upper 3 bits per both CCSDS 133.0-B-2 and 133.1-B-3
    return (firstByte >> 5) & 0x07;
}

}  // namespace Ccsds
}  // namespace Svc
