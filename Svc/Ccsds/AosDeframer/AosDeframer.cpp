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
#include "Svc/Ccsds/Types/EppProtocolIdEnumAc.hpp"
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
      m_fixedFrameSize(ComCfg::AosMaxFrameFixedSize),
      m_fecfEnabled(true),
      m_spacecraftId(ComCfg::SpacecraftId),
      m_crcErrorCount(0) {
    // Initialize VC struct
    for (U8 vcInd = 0; vcInd < AosDeframer_NumVcs; vcInd++) {
        m_vcs[vcInd].vcStructIndex = vcInd;
    }
}

AosDeframer::~AosDeframer() {}

void AosDeframer::configure(U32 fixedFrameSize, bool frameErrorControlField, U16 spacecraftId, U8 vcId, U8 pvnMask) {
    // Validate frame size is within bounds
    FW_ASSERT(fixedFrameSize <= ComCfg::AosMaxFrameFixedSize, static_cast<FwAssertArgType>(fixedFrameSize),
              static_cast<FwAssertArgType>(ComCfg::AosMaxFrameFixedSize));

    // Frame must be large enough for header + M_PDU header + optional trailer
    const FwSizeType minSize = AOSHeader::SERIALIZED_SIZE + M_PDUHeader::SERIALIZED_SIZE +
                               (frameErrorControlField ? AOSTrailer::SERIALIZED_SIZE : 0);
    FW_ASSERT(fixedFrameSize > minSize, static_cast<FwAssertArgType>(fixedFrameSize),
              static_cast<FwAssertArgType>(minSize));

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

    // Clear out all VC stats
    for (U8 vcInd = 0; vcInd < AosDeframer_NumVcs; vcInd++) {
        m_vcs[vcInd].framesProcessed = 0;
        m_vcs[vcInd].packetsExtracted = 0;
        m_vcs[vcInd].vcFrameCount = 0;

        // Clear out the spanningPacket
        this->abandonSpanningPacket(m_vcs[vcInd]);
    }
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
    // Start null, and is set by the parse step
    AosDeframerVc* vc;

    // Parse and validate the AOS Primary Header (Section 4.1.2)
    // Note: parseAndValidateHeader handles warning events and errorNotify for header failures.
    if ((vc = this->parseAndValidateHeader(data, packetContext)) == nullptr) {
        this->dataReturnOut_out(0, data, context);
        return;
    }

    // Set the default context
    vc->spanningPacket.context = packetContext;

    // Update telemetry
    this->tlmWrite_FramesProcessed(++vc->framesProcessed);

    // Extract packets from the M_PDU data zone
    this->extractPackets(*vc, data);

    // Return the frame buffer
    this->dataReturnOut_out(0, data, context);
}

void AosDeframer::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    // Deallocate this dynamically allocated packet
    this->deallocate_out(0, fwBuffer);
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
    vc.spanningPacket.bytesReceived = 0;
    vc.spanningPacket.context.set_pvn(ComCfg::Pvn::INVALID_UNINITIALIZED);
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
    U16 spacecraftId = static_cast<U16>(((header.get_globalVcId() & AOSHeaderSubfields::spacecraftIdLsbMask) >>
                                         AOSHeaderSubfields::spacecraftIdLsbOffset));
    spacecraftId |= static_cast<U16>((header.get_frameCountAndSignaling() & AOSHeaderSubfields::spacecraftIdMsbMask)
                                     << (8 - AOSHeaderSubfields::spacecraftIdMsbOffset));

    if (spacecraftId != m_spacecraftId) {
        this->log_WARNING_LO_InvalidSpacecraftId(spacecraftId, m_spacecraftId);
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_SCID);
        return nullptr;
    }

    // Extract Virtual Channel ID (Section 4.1.2.3)
    U8 vcId = static_cast<U8>(header.get_globalVcId() & AOSHeaderSubfields::virtualChannelIdMask);
    AosDeframerVc* vc = this->getVcStruct(vcId);

    if (vc == nullptr) {
        // TODO: Multi VC | Handle logging all valid vcIds
        this->log_ACTIVITY_LO_InvalidVcId(vcId, m_vcs[0].virtualChannelId);
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_VCID);
        return vc;
    }

    // Extract Virtual Channel Frame Count (Section 4.1.2.4)
    // 24 bits in the upper 3 bytes of frameCountAndSignaling
    U32 vcFrameCount = (header.get_frameCountAndSignaling() & AOSHeaderSubfields::vcFrameCountMask) >>
                       AOSHeaderSubfields::vcFrameCountOffset;

    // Extract VC Frame Count Cycle if in use (Section 4.1.2.5.3)
    if ((header.get_frameCountAndSignaling() & AOSHeaderSubfields::cycleCountFlagMask) != 0) {
        const U8 vcFrameCountCycle = header.get_frameCountAndSignaling() & AOSHeaderSubfields::vcFrameCountCycleMask;
        // Extend the 24-bit frame count with the 4-bit cycle count
        vcFrameCount |= static_cast<U32>(vcFrameCountCycle) << 24;
    }

    // Gap detect after the first accepted frame on a VC
    if (vc->framesProcessed > 0U) {
        const U32 expectedVcFrameCount = vc->vcFrameCount + 1U;
        if (vcFrameCount != expectedVcFrameCount) {
            this->log_WARNING_HI_VcFrameCountGap(vcId, vcFrameCount, expectedVcFrameCount);
            this->notifyErrorIfConnected(Ccsds::FrameError::AOS_VC_FRAME_COUNT_GAP);
            // Other errors will implicitly drop their spanning packet once we finally lock back onto a valid frame
            this->abandonSpanningPacket(*vc);
        }
    }

    // Store VC frame count in the VC struct for reference (e.g., gap detection)
    this->tlmWrite_LatestVcFrameCount(vc->vcFrameCount = vcFrameCount);

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

FwSizeType AosDeframer::appendToSpanningPacket(AosDeframerVc& vc, U8* data, FwSizeType size) {
    // Seek amount
    FwSizeType seekForward = 0;

    if (!vc.spanningPacket.buffer.isValid()) {
        // Fill the tmp header buff w/ what we got
        const FwSizeType headerCap = AosDeframerVc::SpanningPacketState::HEADER_BUF_SIZE;
        const FwSizeType toHeader = FW_MIN(size, headerCap - vc.spanningPacket.bytesReceived);
        if (toHeader > 0) {
            ::memcpy(vc.spanningPacket.headerBuf + vc.spanningPacket.bytesReceived, data, toHeader);
            vc.spanningPacket.bytesReceived += toHeader;

            // We'll work w/ everything past the copied header if we get a clean parse
            data += toHeader;
            size -= toHeader;
            seekForward += toHeader;
        }

        // Attempt to find a size w/ what we have (zero means this frame is over)
        const FwSizeType packetSize = sizePacket(vc, vc.spanningPacket.headerBuf, vc.spanningPacket.bytesReceived);
        if (packetSize == 0) {
            return 0;
        }

        // Try to allocate a buffer for the whole packet
        vc.spanningPacket.buffer = this->allocate_out(0, packetSize);
        if (vc.spanningPacket.buffer.getSize() < packetSize) {
            this->log_WARNING_HI_SpanningPacketAllocFailed(vc.virtualChannelId, vc.spanningPacket.context.get_pvn(),
                                                           packetSize);
            this->abandonSpanningPacket(vc);

            // Seek past this packet/frame
            const FwSizeType remainingLength = seekForward + packetSize - vc.spanningPacket.bytesReceived;
            if (remainingLength > size) {
                return 0;
            } else {
                return remainingLength;
            }
        }

        // Load the header into the dynamic buffer
        ::memcpy(vc.spanningPacket.buffer.getData(), vc.spanningPacket.headerBuf, vc.spanningPacket.bytesReceived);
    }

    // Already have the dynamic buffer, so fill away
    const FwSizeType spaceLeft = vc.spanningPacket.buffer.getSize() - vc.spanningPacket.bytesReceived;
    // Copy what we got
    const FwSizeType toBody = FW_MIN(size, spaceLeft);
    if (toBody > 0) {
        ::memcpy(vc.spanningPacket.buffer.getData() + vc.spanningPacket.bytesReceived, data, toBody);
        vc.spanningPacket.bytesReceived += toBody;
        seekForward += toBody;
    }

    // Check if the spanning packet is now complete
    if (vc.spanningPacket.buffer.getSize() > 0 &&
        vc.spanningPacket.bytesReceived >= vc.spanningPacket.buffer.getSize()) {
        this->dataOut_out(0, vc.spanningPacket.buffer, vc.spanningPacket.context);
        this->tlmWrite_PacketsExtracted(++vc.packetsExtracted);

        // Ownership of the buffer has transferred downstream; clear local handle before consolidating state reset.
        vc.spanningPacket.buffer = Fw::Buffer();
        // Buffer won't be returned now since we cleared the handle
        this->abandonSpanningPacket(vc);
    }

    return seekForward;
}

void AosDeframer::extractPackets(AosDeframerVc& vc, Fw::Buffer& data) {
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
    else if (firstHeaderPointer == M_PDUSubfields::FHP_NO_PACKET_START) {
        // Entire data zone is continuation of previous packet
        if (vc.spanningPacket.bytesReceived > 0) {
            (void)this->appendToSpanningPacket(vc, dataZone, dataZoneSize);
        }
        // If no spanning packet active, this continuation data cannot be used
        return;
    }

    // There is continuation data before the first packet header
    if (firstHeaderPointer > 0 && vc.spanningPacket.bytesReceived > 0) {
        (void)this->appendToSpanningPacket(vc, dataZone, static_cast<FwSizeType>(firstHeaderPointer));
        // We must be done w/ the prior packet since we have a FHP
        this->abandonSpanningPacket(vc);
    }

    // Move to first packet header
    FwSizeType currentOffset = firstHeaderPointer;

    // Max Bound is a sequence of 1 byte EPP Idle Packets
    const FwIndexType maxIters = static_cast<FwIndexType>(dataZoneSize - firstHeaderPointer);

    // Extract packets starting at First Header Pointer
    // (All fresh packets from here on out)
    for (FwIndexType iter = 0; iter < maxIters && currentOffset < dataZoneSize; iter++) {
        // Clear out any prior packet data
        this->abandonSpanningPacket(vc);

        U8* packetStart = dataZone + currentOffset;
        FwSizeType remainingBytes = dataZoneSize - currentOffset;

        FwSizeType packetSize = this->appendToSpanningPacket(vc, packetStart, remainingBytes);

        if (packetSize == 0) {
            // Break out of loop since we ran out of data
            return;
        }

        currentOffset += packetSize;
    }
}

FwSizeType AosDeframer::sizePacket(AosDeframerVc& vc, U8* packetStart, FwSizeType remainingBytes) {
    // Determine packet type from PVN (upper 3 bits of first byte)
    U8 pvn = getPacketVersion(packetStart[0]);
    // Default to invalid, override if valid (non-idle) packet
    vc.spanningPacket.context.set_pvn(ComCfg::Pvn::INVALID_UNINITIALIZED);

    // Size the Packet (so we can alloc a buffer)
    switch (ComCfg::Pvn pvnEnum = static_cast<ComCfg::Pvn::T>(pvn)) {
        case ComCfg::Pvn::SPACE_PACKET_PROTOCOL:
        // Intentionally fallthrough since logic is more condensed this way
        case ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL:
            if (vc.pvnMask & (1 << pvn)) {
                vc.spanningPacket.context.set_pvn(pvnEnum);
                if (pvnEnum == ComCfg::Pvn::SPACE_PACKET_PROTOCOL) {
                    return sizeSppPacket(packetStart, remainingBytes);
                } else {
                    return sizeEppPacket(packetStart, remainingBytes);
                }
            } else {
                this->log_WARNING_HI_DisabledPvn(vc.virtualChannelId, pvnEnum);
                return 0;
            }
            break;
        default:
            this->log_WARNING_HI_InvalidPvn(vc.virtualChannelId, pvn);
            return 0;
    }
}

FwSizeType AosDeframer::sizeSppPacket(U8* payloadStart, FwSizeType payloadSize) {
    // Per CCSDS 133.0-B-2, Space Packet Header is 6 bytes
    if (payloadSize < SpacePacketHeader::SERIALIZED_SIZE) {
        return 0;  // Incomplete header - spans to next frame
    }

    SpacePacketHeader header;

    Fw::Buffer data(payloadStart, payloadSize);
    Fw::SerializeStatus status = data.getDeserializer().deserializeTo(header);

    if (status != Fw::FW_SERIALIZE_OK) {
        return 0;
    }

    // Per CCSDS 133.0-B-2 Section 4.1.3.5.2, packet data length = (actual length - 1)
    FwSizeType totalPacketSize = SpacePacketHeader::SERIALIZED_SIZE + header.get_packetDataLength() + 1;

    // TODO: Unify Deframers | bring the whole spp processing into this compoent
    // since we're only missing seq count logic?

    // Check for idle packet (APID = 0x7FF per CCSDS 133.0-B-2)
    U16 apid = static_cast<U16>(header.get_packetIdentification() & SpacePacketSubfields::ApidMask);

    // Idle means this is the last packet in the frame
    if (apid == static_cast<U16>(ComCfg::Apid::SPP_IDLE_PACKET)) {
        return 0;
    }

    return totalPacketSize;
}

FwSizeType AosDeframer::sizeEppPacket(const U8* const payloadStart, FwSizeType payloadSize) {
    // Per CCSDS 133.1-B-3 Section 4.1.2.1.1, EPP minimum header is 1 byte
    if (payloadSize < 1) {
        return 0;
    }

    // Parse first byte
    U8 firstByte = payloadStart[0];
    U8 protocolId = static_cast<U8>((firstByte & EPPSubfields::protocolIdMask) >> EPPSubfields::protocolIdOffset);

    FwSizeType totalPacketSize = 0;

    // Idle means this is the last packet in the frame
    if (protocolId == static_cast<U8>(EppProtocolId::Idle)) {
        return 0;
    }

    // Encapsulation Idle Packet per CCSDS 133.1-B-3 Section 4.1.3.2
    const U8 lengthOfLength = firstByte & EPPSubfields::lengthOfLengthMask;

    U8 lengthOffset = 1U;

    // If length of length is 2 or more then there's an extra byte of extension/user defined (4.1.2.1.1)
    if (lengthOfLength >= 2) {
        lengthOffset += 1U;
    }

    // If length of length is 4 then we add 2 bytes for the ccsds reserved field (4.1.2.1.1)
    if (lengthOfLength == 4) {
        lengthOffset += 2U;
    }

    // Bytes to get to length + length of length
    const U8 headerLength = lengthOffset + lengthOfLength;

    // Validate and read length field
    if (payloadSize < headerLength) {
        return 0;  // Incomplete
    }

    // Read length field (big-endian)
    U32 packetDataLength = 0;
    for (U8 i = 0; i < lengthOfLength; i++) {
        packetDataLength = (packetDataLength << 8) | payloadStart[lengthOffset + i];
    }

    totalPacketSize = headerLength + packetDataLength;

    return totalPacketSize;
}

U8 AosDeframer::getPacketVersion(U8 firstByte) {
    // PVN is the upper 3 bits per both CCSDS 133.0-B-2 and 133.1-B-3
    // EPP's Subfield array is done in bytes
    return firstByte >> EPPSubfields::packetVersionOffset;
}

}  // namespace Ccsds
}  // namespace Svc
