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

    // Spanning packet reassembly requires dynamic backing via allocator ports
    FW_ASSERT(this->isConnected_allocate_OutputPort(0));
    FW_ASSERT(this->isConnected_deallocate_OutputPort(0));

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
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_LENGTH);
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Validate FECF if enabled (Section 4.1.6)
    // FrameDetector + FrameAccumulator or Lower Protocol Layer should enforce whole AOS Frames
    if (m_fecfEnabled && !this->validateFecf(data)) {
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Create a mutable context for extracted packet info
    ComCfg::FrameContext packetContext = context;
    // Start null, and is set by the
    AosDeframerVc* vc;

    // Parse and validate the AOS Primary Header (Section 4.1.2)
    // Note: parseAndValidateHeader handles warning events and errorNotify for header failures.
    if ((vc = this->parseAndValidateHeader(data, packetContext)) == nullptr) {
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Update telemetry
    this->tlmWrite_FramesProcessed(++vc->framesProcessed);

    // Extract packets from the M_PDU data zone
    this->extractPackets(*vc, data, packetContext);

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

void AosDeframer::notifyErrorIfConnected(Ccsds::FrameError error) {
    if (this->isConnected_errorNotify_OutputPort(0)) {
        this->errorNotify_out(0, error);
    }
}

void AosDeframer::abandonSpanningPacket(AosDeframerVc& vc) {
    if (vc.spanningPacket.buffer.isValid()) {
        this->deallocate_out(0, vc.spanningPacket.buffer);
    }
    vc.spanningPacket.buffer = Fw::Buffer();
    vc.spanningPacket.active = false;
    vc.spanningPacket.bytesReceived = 0;
    vc.spanningPacket.expectedSize = 0;
    vc.spanningPacket.pvn = ComCfg::Pvn::INVALID_UNINITIALIZED;
}

AosDeframer::AosDeframerVc* AosDeframer::getVcStruct(const U8 vcId) {
    for (U8 vcInd = 0; vcInd < AosDeframer_NumVcs; vcInd++) {
        if (m_vcs[vcInd].virtualChannelId == vcId) {
            return &m_vcs[vcInd];
        }
    }

    return nullptr;
}

AosDeframer::AosDeframerVc* AosDeframer::parseAndValidateHeader(Fw::Buffer& data, ComCfg::FrameContext& context) {
    // Deserialize the AOS Primary Header (per CCSDS 732.0-B-5 Section 4.1.2)
    AOSHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserializeTo(header);
    if (status != Fw::FW_SERIALIZE_OK) {
        // Buffer too small to contain a valid AOS primary header
        this->log_WARNING_HI_InvalidFrameLength(data.getSize(), m_fixedFrameSize);
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_LENGTH);
        return nullptr;
    }

    // Extract Transfer Frame Version Number (Section 4.1.2.2.2)
    // AOS uses Tfvn::AOS = 0x1 ('01' binary)
    U8 tfvn = static_cast<U8>((header.get_globalVcId() & AOSHeaderSubfields::frameVersionMask) >>
                              AOSHeaderSubfields::frameVersionOffset);
    if (tfvn != static_cast<U8>(Tfvn::AOS)) {
        this->log_WARNING_HI_InvalidTfvn(tfvn, static_cast<U8>(Tfvn::AOS));
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_VERSION);
        return nullptr;
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
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_SCID);
        return nullptr;
    }

    // Extract Virtual Channel ID (Section 4.1.2.3)
    U8 vcId = static_cast<U8>(header.get_globalVcId() & AOSHeaderSubfields::virtualChannelIdMask);
    AosDeframerVc* vc = this->getVcStruct(vcId);

    if (vc == nullptr) {
        // TODO: Handle logging all valid vcIds
        this->log_ACTIVITY_LO_InvalidVcId(vcId, m_vcs[0].virtualChannelId);
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_VCID);
        return vc;
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

    // Gap detect after the first accepted frame on a VC
    if (vc->framesProcessed > 0U) {
        const U32 expectedVcFrameCount = vc->vcFrameCount + 1U;
        if (vcFrameCount != expectedVcFrameCount) {
            this->log_WARNING_HI_VcFrameCountGap(vcId, vcFrameCount, expectedVcFrameCount);
            this->notifyErrorIfConnected(Ccsds::FrameError::AOS_VC_FRAME_COUNT_GAP);
            this->abandonSpanningPacket(*vc);
        }
    }

    // Store VC frame count in the VC struct for reference (e.g., gap detection)
    vc->vcFrameCount = vcFrameCount;
    this->tlmWrite_LatestVcFrameCount(vcFrameCount);

    // Update context with extracted values
    context.set_vcId(vcId);

    return vc;
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
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_CRC);
        this->tlmWrite_CrcErrorCount(++m_crcErrorCount);
        return false;
    }

    return true;
}

bool AosDeframer::appendToSpanningPacket(AosDeframerVc& vc, U8* data, FwSizeType size, ComCfg::FrameContext& context) {
    if (!vc.spanningPacket.buffer.isValid()) {
        const FwSizeType headerCap = AosDeframerVc::SpanningPacketState::HEADER_BUF_SIZE;
        const FwSizeType toHeader = FW_MIN(size, headerCap - vc.spanningPacket.bytesReceived);
        if (toHeader > 0) {
            ::memcpy(vc.spanningPacket.headerBuf + vc.spanningPacket.bytesReceived, data, toHeader);
            vc.spanningPacket.bytesReceived += toHeader;
        }

        if (vc.spanningPacket.expectedSize == 0) {
            if (vc.spanningPacket.pvn == ComCfg::Pvn::SPACE_PACKET_PROTOCOL &&
                vc.spanningPacket.bytesReceived >= SpacePacketHeader::SERIALIZED_SIZE) {
                const U16 dataLength =
                    static_cast<U16>((vc.spanningPacket.headerBuf[4] << 8) | vc.spanningPacket.headerBuf[5]);
                vc.spanningPacket.expectedSize = SpacePacketHeader::SERIALIZED_SIZE + dataLength + 1;
            } else if (vc.spanningPacket.pvn == ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL &&
                       vc.spanningPacket.bytesReceived >= 1U) {
                const U8 firstByte = vc.spanningPacket.headerBuf[0];
                const U8 packetType =
                    static_cast<U8>((firstByte & EPPSubfields::packetTypeMask) >> EPPSubfields::packetTypeOffset);

                if (packetType == static_cast<U8>(EppPacketType::EncapsulationIdle)) {
                    const U8 lengthOfLength = firstByte & EPPSubfields::lengthOfLengthMask;
                    if ((lengthOfLength > 0U) &&
                        (vc.spanningPacket.bytesReceived >= static_cast<FwSizeType>(1U + lengthOfLength))) {
                        FwSizeType packetDataLength = 0;
                        for (U8 i = 0; i < lengthOfLength; i++) {
                            packetDataLength = (packetDataLength << 8) | vc.spanningPacket.headerBuf[1 + i];
                        }
                        vc.spanningPacket.expectedSize = 1U + lengthOfLength + packetDataLength;
                    }
                } else if (vc.spanningPacket.bytesReceived >= 3U) {
                    const U16 packetDataLength =
                        static_cast<U16>((vc.spanningPacket.headerBuf[1] << 8) | vc.spanningPacket.headerBuf[2]);
                    vc.spanningPacket.expectedSize = 3U + packetDataLength;
                }
            }
        }

        if (vc.spanningPacket.expectedSize == 0) {
            return false;
        }

        Fw::Buffer allocated = this->allocate_out(0, vc.spanningPacket.expectedSize);
        if (!allocated.isValid() || allocated.getSize() < vc.spanningPacket.expectedSize) {
            this->log_WARNING_HI_SpanningPacketAllocFailed(vc.virtualChannelId, static_cast<U8>(vc.spanningPacket.pvn),
                                                           vc.spanningPacket.expectedSize);
            this->abandonSpanningPacket(vc);
            return false;
        }

        ::memcpy(allocated.getData(), vc.spanningPacket.headerBuf, vc.spanningPacket.bytesReceived);

        const FwSizeType alreadyCopied = toHeader;
        const FwSizeType remaining = size - alreadyCopied;
        const FwSizeType spaceLeft = vc.spanningPacket.expectedSize - vc.spanningPacket.bytesReceived;
        const FwSizeType moreBytes = FW_MIN(remaining, spaceLeft);
        if (moreBytes > 0) {
            ::memcpy(allocated.getData() + vc.spanningPacket.bytesReceived, data + alreadyCopied, moreBytes);
            vc.spanningPacket.bytesReceived += moreBytes;
        }

        vc.spanningPacket.buffer = allocated;
    } else {
        const FwSizeType spaceLeft = vc.spanningPacket.expectedSize - vc.spanningPacket.bytesReceived;
        const FwSizeType bytesToCopy = FW_MIN(size, spaceLeft);
        if (bytesToCopy > 0) {
            ::memcpy(vc.spanningPacket.buffer.getData() + vc.spanningPacket.bytesReceived, data, bytesToCopy);
            vc.spanningPacket.bytesReceived += bytesToCopy;
        }
    }

    // Check if the spanning packet is now complete
    if (vc.spanningPacket.expectedSize > 0 && vc.spanningPacket.bytesReceived >= vc.spanningPacket.expectedSize) {
        vc.spanningPacket.buffer.setSize(vc.spanningPacket.expectedSize);

        ComCfg::FrameContext packetContext = context;
        packetContext.set_pvn(vc.spanningPacket.pvn);

        this->dataOut_out(0, vc.spanningPacket.buffer, packetContext);
        this->tlmWrite_PacketsExtracted(++vc.packetsExtracted);

        // Ownership of the buffer has transferred downstream; clear local handle before consolidating state reset.
        vc.spanningPacket.buffer = Fw::Buffer();
        this->abandonSpanningPacket(vc);
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
        // Frame contains only idle data - abandon any in-progress spanning packet
        this->log_ACTIVITY_LO_IdleFrame(vc.virtualChannelId);
        this->abandonSpanningPacket(vc);
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
        this->abandonSpanningPacket(vc);
    }

    // Move to first packet header
    FwSizeType currentOffset = firstHeaderPointer;

    // Extract packets starting at First Header Pointer
    for (; currentOffset < dataZoneSize;) {
        U8* packetStart = dataZone + currentOffset;
        FwSizeType remainingBytes = dataZoneSize - currentOffset;

        // Determine packet type from PVN (upper 3 bits of first byte)
        U8 pvn = getPacketVersion(packetStart[0]);
        const bool isSpp = (pvn == static_cast<U8>(ComCfg::Pvn::SPACE_PACKET_PROTOCOL));
        const bool isEpp = (pvn == static_cast<U8>(ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL));

        FwSizeType packetSize = 0;

        if (isSpp && (vc.pvnMask & PvnBitfield::SPP_MASK)) {
            // Space Packet Protocol
            packetSize = extractSppPacket(vc, packetStart, remainingBytes, context);
        } else if (isEpp && (vc.pvnMask & PvnBitfield::EPP_MASK)) {
            // Encapsulation Packet Protocol
            packetSize = extractEppPacket(vc, packetStart, remainingBytes, context);
        } else {
            if (!isSpp && !isEpp) {
                this->log_WARNING_HI_InvalidPvn(vc.virtualChannelId, pvn);
            }
            // Unknown or disabled packet type - stop processing this frame
            break;
        }

        if (packetSize == 0) {
            // Packet spans to next frame - save state
            vc.spanningPacket.active = true;
            vc.spanningPacket.pvn =
                isSpp ? ComCfg::Pvn::SPACE_PACKET_PROTOCOL : ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL;
            vc.spanningPacket.buffer = Fw::Buffer();
            vc.spanningPacket.bytesReceived = 0;
            vc.spanningPacket.expectedSize = 0;
            (void)this->appendToSpanningPacket(vc, packetStart, remainingBytes, context);
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
    this->tlmWrite_PacketsExtracted(++vc.packetsExtracted);

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
        this->tlmWrite_PacketsExtracted(++vc.packetsExtracted);

        return totalPacketSize;
    }
}

U8 AosDeframer::getPacketVersion(U8 firstByte) {
    // PVN is the upper 3 bits per both CCSDS 133.0-B-2 and 133.1-B-3
    return (firstByte >> 5) & 0x07;
}

}  // namespace Ccsds
}  // namespace Svc
