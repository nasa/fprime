// ======================================================================
// \title  AosDeframer.cpp
// \author Auto-generated
// \brief  cpp file for AosDeframer component implementation class
//
// Deframer for the AOS Space Data Link Protocol per CCSDS 732.0-B-5.
// Supports M_PDU data field service with:
// - Frame Error Control Field (FECF) validation (Section 4.1.6)
// - Space Packet Protocol (SPP) extraction (CCSDS 133.0-B-2)
// - Encapsulation Packet Protocol (EPP) extraction (CCSDS 133.1-B-3)
// ======================================================================

#include "Svc/Ccsds/AosDeframer/AosDeframer.hpp"
#include "Fw/Types/String.hpp"
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
      m_vcId(0),
      m_acceptAllVcid(true),
      m_pvnMask(PvnBitfield::SPP_MASK | PvnBitfield::EPP_MASK),
      m_frameCount(0),
      m_packetCount(0),
      m_crcErrorCount(0) {
    // Initialize spanning packet state
    m_spanningPacket.bytesReceived = 0;
    m_spanningPacket.expectedSize = 0;
    m_spanningPacket.pvn = 0;
    m_spanningPacket.active = false;
}

AosDeframer::~AosDeframer() {}

void AosDeframer::configure(U32 fixedFrameSize,
                            bool frameErrorControlField,
                            U16 spacecraftId,
                            U8 vcId,
                            bool acceptAllVcid,
                            U8 pvnMask) {
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

    // At least one packet type must be enabled
    FW_ASSERT((pvnMask & PvnBitfield::VALID_MASK) != 0, static_cast<FwAssertArgType>(pvnMask));

    m_fixedFrameSize = fixedFrameSize;
    m_fecfEnabled = frameErrorControlField;
    m_spacecraftId = spacecraftId;
    m_vcId = vcId;
    m_acceptAllVcid = acceptAllVcid;
    m_pvnMask = pvnMask;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void AosDeframer::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // Per CCSDS 732.0-B-5, AOS frames are fixed-size
    // Verify we have received a complete frame
    FW_ASSERT(m_fixedFrameSize > 0, static_cast<FwAssertArgType>(m_fixedFrameSize));

    if (data.getSize() < m_fixedFrameSize) {
        this->log_WARNING_HI_InvalidFrameLength(m_fixedFrameSize, data.getSize());
        this->errorNotifyHelper(Ccsds::FrameError::AOS_INVALID_LENGTH);
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Create a mutable context for extracted packet info
    ComCfg::FrameContext packetContext = context;

    // Parse and validate the AOS Primary Header (Section 4.1.2)
    if (!this->parseAndValidateHeader(data, packetContext)) {
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Validate FECF if enabled (Section 4.1.6)
    if (m_fecfEnabled && !this->validateFecf(data)) {
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Update telemetry
    m_frameCount++;
    this->tlmWrite_FrameCount(m_frameCount);

    // Extract packets from the M_PDU data zone
    this->extractPackets(data, packetContext);

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

void AosDeframer::errorNotifyHelper(Ccsds::FrameError error) {
    if (this->isConnected_errorNotify_OutputPort(0)) {
        this->errorNotify_out(0, error);
    }
}

bool AosDeframer::parseAndValidateHeader(Fw::Buffer& data, ComCfg::FrameContext& context) {
    // Deserialize the AOS Primary Header (per CCSDS 732.0-B-5 Section 4.1.2)
    AOSHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserializeTo(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 globalVcId = header.get_globalVcId();
    U32 frameCountAndSignaling = header.get_frameCountAndSignaling();

    // Extract Transfer Frame Version Number (Section 4.1.2.2.2)
    // AOS uses version '01' binary = 1
    U8 tfvn = static_cast<U8>((globalVcId & AOSHeaderSubfields::frameVersionMask) >> AOSHeaderSubfields::frameVersionOffset);
    if (tfvn != AOSHeaderSubfields::expectedFrameVersion) {
        this->log_WARNING_HI_InvalidTfvn(tfvn, static_cast<U8>(AOSHeaderSubfields::expectedFrameVersion));
        this->errorNotifyHelper(Ccsds::FrameError::AOS_INVALID_VERSION);
        return false;
    }

    // Extract Spacecraft ID (Section 4.1.2.2)
    // SCID is split: 8 LSBs in globalVcId, 2 MSBs in signaling field
    U16 scidLsb = static_cast<U16>((globalVcId & AOSHeaderSubfields::spacecraftIdLsbMask) >> AOSHeaderSubfields::spacecraftIdLsbOffset);
    U8 signalingByte = static_cast<U8>(frameCountAndSignaling & 0xFF);
    U16 scidMsb = static_cast<U16>((signalingByte & AOSHeaderSubfields::spacecraftIdMsbMask) >> AOSHeaderSubfields::spacecraftIdMsbOffset);
    U16 spacecraftId = static_cast<U16>(scidLsb | (scidMsb << 8));

    if (spacecraftId != m_spacecraftId) {
        this->log_WARNING_LO_InvalidSpacecraftId(spacecraftId, m_spacecraftId);
        this->errorNotifyHelper(Ccsds::FrameError::AOS_INVALID_SCID);
        return false;
    }

    // Extract Virtual Channel ID (Section 4.1.2.3)
    U8 vcId = static_cast<U8>(globalVcId & AOSHeaderSubfields::virtualChannelIdMask);
    if (!m_acceptAllVcid && vcId != m_vcId) {
        this->log_ACTIVITY_LO_InvalidVcId(vcId, m_vcId);
        this->errorNotifyHelper(Ccsds::FrameError::AOS_INVALID_VCID);
        return false;
    }

    // Extract Virtual Channel Frame Count (Section 4.1.2.4)
    // 24 bits in the upper 3 bytes of frameCountAndSignaling
    U32 vcFrameCount = (frameCountAndSignaling >> AOSHeaderSubfields::vcFrameCountOffset) & AOSHeaderSubfields::vcFrameCountMask;

    // Extract Replay Flag (Section 4.1.2.5.2)
    bool replayFlag = (signalingByte & AOSHeaderSubfields::replayFlagMask) != 0;

    // Extract VC Frame Count Cycle if in use (Section 4.1.2.5.3)
    bool cycleCountInUse = (signalingByte & AOSHeaderSubfields::cycleCountFlagMask) != 0;
    if (cycleCountInUse) {
        U8 vcFrameCountCycle = signalingByte & AOSHeaderSubfields::vcFrameCountCycleMask;
        // Extend the 24-bit frame count with 4-bit cycle
        vcFrameCount |= static_cast<U32>(vcFrameCountCycle) << 24;
    }

    // Update context with extracted values
    context.set_vcId(vcId);
    context.set_replayFlag(replayFlag);

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
        this->log_WARNING_HI_InvalidCrc(transmittedCrc, computedCrc);
        this->errorNotifyHelper(Ccsds::FrameError::AOS_INVALID_CRC);
        m_crcErrorCount++;
        this->tlmWrite_CrcErrorCount(m_crcErrorCount);
        return false;
    }

    return true;
}

void AosDeframer::extractPackets(Fw::Buffer& data, ComCfg::FrameContext& context) {
    // Parse M_PDU header (per CCSDS 732.0-B-5 Section 4.1.4.2.2)
    M_PDUHeader mpduHeader;
    auto deserializer = data.getDeserializer();
    deserializer.moveDeserToOffset(AOSHeader::SERIALIZED_SIZE);
    Fw::SerializeStatus status = deserializer.deserializeTo(mpduHeader);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 firstHeaderPointer = mpduHeader.get_firstHeaderPointer() & MPDUSubfields::firstHeaderPointerMask;

    // Calculate data zone boundaries
    const FwSizeType dataZoneStart = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE;
    const FwSizeType dataZoneEnd = m_fixedFrameSize - (m_fecfEnabled ? AOSTrailer::SERIALIZED_SIZE : 0);
    const FwSizeType dataZoneSize = dataZoneEnd - dataZoneStart;
    U8* dataZone = data.getData() + dataZoneStart;

    // Handle special First Header Pointer values (Section 4.1.4.2.2.4)
    if (firstHeaderPointer == MPDUSubfields::FHP_IDLE_DATA_ONLY) {
        // Frame contains only idle data
        this->log_ACTIVITY_LO_IdleFrame();
        // If there's a spanning packet in progress, this is an error condition
        // Per the standard, idle-only frames shouldn't occur mid-packet
        m_spanningPacket.active = false;
        m_spanningPacket.bytesReceived = 0;
        return;
    }

    FwSizeType currentOffset = 0;

    // Handle continuing packet data (data before First Header Pointer)
    if (firstHeaderPointer == MPDUSubfields::FHP_NO_PACKET_START) {
        // Entire data zone is continuation of previous packet
        if (m_spanningPacket.active) {
            // Add all data to spanning packet buffer
            FwSizeType bytesToCopy = FW_MIN(dataZoneSize,
                                            sizeof(m_spanningPacket.buffer) - m_spanningPacket.bytesReceived);
            ::memcpy(m_spanningPacket.buffer + m_spanningPacket.bytesReceived, dataZone, bytesToCopy);
            m_spanningPacket.bytesReceived += bytesToCopy;

            // Check if we now have enough for the header to determine packet size
            if (m_spanningPacket.expectedSize == 0 && m_spanningPacket.bytesReceived >= 6) {
                // For SPP, packet length is in bytes 4-5 (0-indexed)
                if ((m_spanningPacket.pvn == 0) && (m_pvnMask & PvnBitfield::SPP_MASK)) {
                    U16 dataLength = static_cast<U16>((m_spanningPacket.buffer[4] << 8) | m_spanningPacket.buffer[5]);
                    m_spanningPacket.expectedSize = SpacePacketHeader::SERIALIZED_SIZE + dataLength + 1;
                }
            }

            // Check if packet is complete
            if (m_spanningPacket.expectedSize > 0 &&
                m_spanningPacket.bytesReceived >= m_spanningPacket.expectedSize) {
                // Output the complete packet
                Fw::Buffer packetBuffer(m_spanningPacket.buffer, m_spanningPacket.expectedSize);

                // Update context with PVN
                ComCfg::FrameContext packetContext = context;
                if (m_spanningPacket.pvn == 0) {
                    packetContext.set_pvn(ComCfg::Pvn::SPACE_PACKET_PROTOCOL);
                } else {
                    packetContext.set_pvn(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
                }

                this->dataOut_out(0, packetBuffer, packetContext);
                m_packetCount++;
                this->tlmWrite_PacketCount(m_packetCount);

                m_spanningPacket.active = false;
                m_spanningPacket.bytesReceived = 0;
                m_spanningPacket.expectedSize = 0;
            }
        }
        // If no spanning packet active, this is continuation data we can't use
        return;
    }

    // Handle continuation data before first packet header
    if (firstHeaderPointer > 0 && m_spanningPacket.active) {
        FwSizeType continuationBytes = FW_MIN(static_cast<FwSizeType>(firstHeaderPointer),
                                              sizeof(m_spanningPacket.buffer) - m_spanningPacket.bytesReceived);
        ::memcpy(m_spanningPacket.buffer + m_spanningPacket.bytesReceived, dataZone, continuationBytes);
        m_spanningPacket.bytesReceived += continuationBytes;

        // Determine expected size if we have enough header bytes
        if (m_spanningPacket.expectedSize == 0 && m_spanningPacket.bytesReceived >= 6) {
            if ((m_spanningPacket.pvn == 0) && (m_pvnMask & PvnBitfield::SPP_MASK)) {
                U16 dataLength = static_cast<U16>((m_spanningPacket.buffer[4] << 8) | m_spanningPacket.buffer[5]);
                m_spanningPacket.expectedSize = SpacePacketHeader::SERIALIZED_SIZE + dataLength + 1;
            }
        }

        // Check if the spanning packet is now complete
        if (m_spanningPacket.expectedSize > 0 &&
            m_spanningPacket.bytesReceived >= m_spanningPacket.expectedSize) {
            // Output the complete packet
            Fw::Buffer packetBuffer(m_spanningPacket.buffer, m_spanningPacket.expectedSize);

            ComCfg::FrameContext packetContext = context;
            if (m_spanningPacket.pvn == 0) {
                packetContext.set_pvn(ComCfg::Pvn::SPACE_PACKET_PROTOCOL);
            } else {
                packetContext.set_pvn(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);
            }

            this->dataOut_out(0, packetBuffer, packetContext);
            m_packetCount++;
            this->tlmWrite_PacketCount(m_packetCount);
        }

        m_spanningPacket.active = false;
        m_spanningPacket.bytesReceived = 0;
        m_spanningPacket.expectedSize = 0;
    }

    // Move to first packet header
    currentOffset = firstHeaderPointer;

    // Extract packets starting at First Header Pointer
    while (currentOffset < dataZoneSize) {
        U8* packetStart = dataZone + currentOffset;
        FwSizeType remainingBytes = dataZoneSize - currentOffset;

        // Determine packet type from PVN
        U8 pvn = getPacketVersion(packetStart[0]);

        FwSizeType packetSize = 0;

        if (pvn == 0 && (m_pvnMask & PvnBitfield::SPP_MASK)) {
            // Space Packet Protocol
            packetSize = extractSppPacket(packetStart, remainingBytes, context);
        } else if (pvn == 7 && (m_pvnMask & PvnBitfield::EPP_MASK)) {
            // Encapsulation Packet Protocol
            packetSize = extractEppPacket(packetStart, remainingBytes, context);
        } else {
            // Unknown or disabled packet type - skip to next frame
            break;
        }

        if (packetSize == 0) {
            // Packet spans to next frame - save state
            m_spanningPacket.active = true;
            m_spanningPacket.pvn = pvn;
            m_spanningPacket.bytesReceived = remainingBytes;
            m_spanningPacket.expectedSize = 0;
            ::memcpy(m_spanningPacket.buffer, packetStart, remainingBytes);
            break;
        }

        currentOffset += packetSize;
    }
}

FwSizeType AosDeframer::extractSppPacket(U8* payloadStart, FwSizeType payloadSize, ComCfg::FrameContext& context) {
    // Per CCSDS 133.0-B-2, Space Packet Header is 6 bytes
    if (payloadSize < SpacePacketHeader::SERIALIZED_SIZE) {
        return 0;  // Incomplete header - spans to next frame
    }

    // Parse packet data length from header (bytes 4-5)
    U16 dataLength = static_cast<U16>((payloadStart[4] << 8) | payloadStart[5]);
    // Per CCSDS 133.0-B-2 Section 4.1.3.5.2, packet data length = (actual length - 1)
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

    // Create buffer pointing to the packet data
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
    m_packetCount++;
    this->tlmWrite_PacketCount(m_packetCount);

    return totalPacketSize;
}

FwSizeType AosDeframer::extractEppPacket(U8* payloadStart, FwSizeType payloadSize, ComCfg::FrameContext& context) {
    // Per CCSDS 133.1-B-3 Section 4.1, EPP minimum header is 1 byte
    if (payloadSize < 1) {
        return 0;
    }

    // Parse first byte
    U8 firstByte = payloadStart[0];
    U8 packetVersion = static_cast<U8>((firstByte & EPPSubfields::packetVersionMask) >> EPPSubfields::packetVersionOffset);

    // Validate packet version
    if (packetVersion != EPPSubfields::expectedPacketVersion) {
        Fw::String reason("Invalid packet version number");
        this->log_WARNING_HI_InvalidEppPacket(packetVersion, reason);
        this->errorNotifyHelper(Ccsds::FrameError::AOS_INVALID_EPP);
        return payloadSize;  // Skip rest of data zone
    }

    U8 packetType = static_cast<U8>((firstByte & EPPSubfields::packetTypeMask) >> EPPSubfields::packetTypeOffset);

    FwSizeType totalPacketSize = 0;

    if (packetType == EPPSubfields::typeEncapsulationIdle) {
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

        // For now, support basic protocol IDs with defined structure
        // Protocol ID determines the header structure

        if (protocolId <= 0x07) {
            // Standard encapsulation - next 2 bytes are length
            if (payloadSize < 3) {
                return 0;  // Incomplete header
            }

            U16 packetDataLength = static_cast<U16>((payloadStart[1] << 8) | payloadStart[2]);
            totalPacketSize = 3 + packetDataLength;
        } else {
            // Extended protocol ID (0x8-0xF) - variable structure
            // For simplicity, treat as 2-byte length following
            if (payloadSize < 3) {
                return 0;
            }

            U16 packetDataLength = static_cast<U16>((payloadStart[1] << 8) | payloadStart[2]);
            totalPacketSize = 3 + packetDataLength;
        }

        if (payloadSize < totalPacketSize) {
            return 0;  // Incomplete packet
        }

        // Create buffer for packet
        Fw::Buffer packetBuffer(payloadStart, totalPacketSize);

        // Update context
        ComCfg::FrameContext packetContext = context;
        packetContext.set_pvn(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL);

        // Output the packet
        this->dataOut_out(0, packetBuffer, packetContext);
        m_packetCount++;
        this->tlmWrite_PacketCount(m_packetCount);

        return totalPacketSize;
    }
}

U8 AosDeframer::getPacketVersion(U8 firstByte) {
    // PVN is the upper 3 bits per both CCSDS 133.0-B-2 and 133.1-B-3
    return (firstByte >> 5) & 0x07;
}

bool AosDeframer::isPacketTypeEnabled(U8 pvn) const {
    if (pvn == 0) {
        return (m_pvnMask & PvnBitfield::SPP_MASK) != 0;
    } else if (pvn == 7) {
        return (m_pvnMask & PvnBitfield::EPP_MASK) != 0;
    }
    return false;
}

}  // namespace Ccsds
}  // namespace Svc
