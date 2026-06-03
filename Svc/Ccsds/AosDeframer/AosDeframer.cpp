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
#include <cstring>
#include <limits>
#include "Svc/Ccsds/Types/EppLengthOfLengthEnumAc.hpp"
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

    // Set the default context only if we haven't for this packet already
    // Otherwise our PVN tracker gets overwritten
    if (!vc->spanningPacket.buffer.isValid()) {
        vc->spanningPacket.context = packetContext;
    }

    // Extract packets from the M_PDU data zone
    this->extractPackets(*vc, data);

    // Return the frame buffer
    this->dataReturnOut_out(0, data, context);

    // Update telemetry
    this->tlmWrite_FramesProcessed(++vc->framesProcessed);
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
        this->log_WARNING_HI_SpanningPacketAbandoned(vc.virtualChannelId, vc.spanningPacket.context.get_pvn(),
                                                     vc.spanningPacket.bytesReceived,
                                                     vc.spanningPacket.buffer.getSize());
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
    // We already checked that a header fits into fixedFrameSize & that this frame is >= fixedFrameSize
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, static_cast<FwAssertArgType>(status));

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
    // We extract and do logical OR in a single operation to appease GCC warnings related to int promotion in |=
    const U16 spacecraftId =
        static_cast<U16>(((header.get_globalVcId() & AOSHeaderSubfields::spacecraftIdLsbMask) >>
                          AOSHeaderSubfields::spacecraftIdLsbOffset) |
                         ((header.get_frameCountAndSignaling() & AOSHeaderSubfields::spacecraftIdMsbMask)
                          << (8 - AOSHeaderSubfields::spacecraftIdMsbOffset)));

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
    U32 rxVcFrameCount = (header.get_frameCountAndSignaling() & AOSHeaderSubfields::vcFrameCountMask) >>
                         AOSHeaderSubfields::vcFrameCountOffset;

    // Default Frame Count is a 24 bit counter (e.g. modulo 2^24)
    U32 frameCountMask = 0x00FF'FFFF;

    // Extract VC Frame Count Cycle if in use (Section 4.1.2.5.3)
    if ((header.get_frameCountAndSignaling() & AOSHeaderSubfields::cycleCountFlagMask) != 0) {
        const U8 rxVcFrameCountCycle = header.get_frameCountAndSignaling() & AOSHeaderSubfields::vcFrameCountCycleMask;
        // Extend the 24-bit frame count with the 4-bit cycle count
        rxVcFrameCount |= static_cast<U32>(rxVcFrameCountCycle) << 24;
        // Add the 4 additional bits to our modulo
        frameCountMask |= 0x0F00'0000;
    }

    // Gap detect after the first accepted frame on a VC
    if (vc->framesProcessed > 0U) {
        const U32 expectedVcFrameCount = vc->vcFrameCount + 1U;
        if (rxVcFrameCount != (expectedVcFrameCount & frameCountMask)) {
            this->log_WARNING_HI_VcFrameCountGap(vcId, rxVcFrameCount, expectedVcFrameCount);
            this->notifyErrorIfConnected(Ccsds::FrameError::AOS_VC_FRAME_COUNT_GAP);
            // Other errors will implicitly drop their spanning packet once we finally lock back onto a valid frame
            this->abandonSpanningPacket(*vc);
        }
    }

    // Store VC frame count in the VC struct for reference (e.g. gap detection)
    vc->vcFrameCount = rxVcFrameCount;
    this->tlmWrite_LatestVcFrameCount(vc->vcFrameCount);

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
    FW_ASSERT(data != nullptr);
    FW_ASSERT(size > 0, static_cast<FwAssertArgType>(size));

    // How much the outer func needs to seek forward in the AOS frame
    FwSizeType seekForward = 0;

    // We work out of the static header buffer until we know the full packet size
    if (!vc.spanningPacket.buffer.isValid()) {
        // (Keep) packing whatever we've got into the static header buffer
        const FwSizeType headerCap = AosDeframerVc::SpanningPacketState::HEADER_BUF_SIZE;
        // Pack the lesser of how much we have & how much room we have
        const FwSizeType toHeader = FW_MIN(size, headerCap - vc.spanningPacket.bytesReceived);
        if (toHeader > 0) {
            FW_ASSERT(vc.spanningPacket.bytesReceived < headerCap,
                      static_cast<FwAssertArgType>(vc.spanningPacket.bytesReceived),
                      static_cast<FwAssertArgType>(headerCap));
            FW_ASSERT(toHeader <= headerCap, static_cast<FwAssertArgType>(toHeader),
                      static_cast<FwAssertArgType>(headerCap));
            FW_ASSERT(vc.spanningPacket.bytesReceived + toHeader <= headerCap,
                      static_cast<FwAssertArgType>(vc.spanningPacket.bytesReceived),
                      static_cast<FwAssertArgType>(toHeader), static_cast<FwAssertArgType>(headerCap));
            ::memcpy(vc.spanningPacket.headerBuf + vc.spanningPacket.bytesReceived, data, toHeader);
            vc.spanningPacket.bytesReceived += toHeader;

            // We'll work w/ everything past the copied header if we get a clean parse
            data += toHeader;
            size -= toHeader;
            seekForward += toHeader;
        }

        // Attempt to find a size w/ what we have in our header buff (zero means we ran out of frame before valid
        // packet)
        const FwSizeType packetSize = sizePacket(vc, vc.spanningPacket.headerBuf, vc.spanningPacket.bytesReceived);
        if (packetSize == 0) {
            return 0;
        }

        // Try to allocate a buffer for the whole packet. If this size is invalid (too large) or if the buffer
        // manager is out of memory, this is handled below.
        vc.spanningPacket.buffer = this->allocate_out(0, packetSize);
        if ((not vc.spanningPacket.buffer.isValid()) || (vc.spanningPacket.buffer.getSize() < packetSize)) {
            this->log_WARNING_HI_SpanningPacketAllocFailed(vc.virtualChannelId, vc.spanningPacket.context.get_pvn(),
                                                           packetSize);
            // Save before abandon clears it -— needed for the correct seek offset below
            const FwSizeType remainingBody = packetSize - vc.spanningPacket.bytesReceived;
            this->abandonSpanningPacket(vc);

            // Seek past the failed packet (header bytes already consumed + remaining body)
            const FwSizeType remainingLength = seekForward + remainingBody;
            if (remainingLength > size) {
                return 0;
            } else {
                return remainingLength;
            }
        }

        // Load the header into the dynamic buffer
        FW_ASSERT(vc.spanningPacket.bytesReceived <= AosDeframerVc::SpanningPacketState::HEADER_BUF_SIZE,
                  static_cast<FwAssertArgType>(vc.spanningPacket.bytesReceived),
                  AosDeframerVc::SpanningPacketState::HEADER_BUF_SIZE);
        // Destination buffer must be large enough for the accumulated header bytes.
        // Protect against any future regression in sizeEppPacket/sizeSppPacket
        // reintroducing an overflow that makes packetSize < bytesReceived.
        FW_ASSERT(vc.spanningPacket.bytesReceived <= vc.spanningPacket.buffer.getSize(),
                  static_cast<FwAssertArgType>(vc.spanningPacket.bytesReceived),
                  static_cast<FwAssertArgType>(vc.spanningPacket.buffer.getSize()));
        ::memcpy(vc.spanningPacket.buffer.getData(), vc.spanningPacket.headerBuf, vc.spanningPacket.bytesReceived);
    }

    // Already have the dynamic buffer, so fill away
    const FwSizeType spaceLeft = vc.spanningPacket.buffer.getSize() - vc.spanningPacket.bytesReceived;
    // Copy what we got
    const FwSizeType toBody = FW_MIN(size, spaceLeft);
    if (toBody > 0) {
        FW_ASSERT(vc.spanningPacket.bytesReceived + toBody <= vc.spanningPacket.buffer.getSize(),
                  static_cast<FwAssertArgType>(vc.spanningPacket.bytesReceived), static_cast<FwAssertArgType>(toBody),
                  static_cast<FwAssertArgType>(vc.spanningPacket.buffer.getSize()));
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
        // Frame contains only idle data
        this->log_ACTIVITY_LO_IdleFrame(vc.virtualChannelId);
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

    // Guard against First Header Pointer pointing out of bounds (untrusted input)
    if (firstHeaderPointer >= dataZoneSize) {
        this->log_WARNING_HI_InvalidFhp(vc.virtualChannelId, firstHeaderPointer, dataZoneSize);
        this->notifyErrorIfConnected(Ccsds::FrameError::AOS_INVALID_LENGTH);
        // Abandon any existing data since this frame (and any continuing packets) are garbage now
        this->abandonSpanningPacket(vc);
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
    FW_ASSERT(remainingBytes > 0, static_cast<FwAssertArgType>(remainingBytes));

    // Determine packet type from PVN (upper 3 bits of first byte)
    U8 pvn = getPacketVersion(packetStart[0]);
    // Default to invalid, override if valid (non-idle) packet
    vc.spanningPacket.context.set_pvn(ComCfg::Pvn::INVALID_UNINITIALIZED);

    // Check if this pvn is disabled
    if (~vc.pvnMask & (1 << pvn)) {
        this->log_WARNING_HI_DisabledPvn(vc.virtualChannelId, pvn);
        return 0;
    }

    ComCfg::Pvn pvnEnum = static_cast<ComCfg::Pvn::T>(pvn);
    vc.spanningPacket.context.set_pvn(pvnEnum);

    // Size the Packet (so we can alloc a buffer)
    switch (pvnEnum) {
        case ComCfg::Pvn::SPACE_PACKET_PROTOCOL:
            return sizeSppPacket(packetStart, remainingBytes);
            break;
        case ComCfg::Pvn::ENCAPSULATION_PACKET_PROTOCOL:
            return sizeEppPacket(packetStart, remainingBytes);
            break;
        default:
            // User should only configure AOS Deframer to accept SPP &/| EPP
            FW_ASSERT(false, pvn);
            return 0;
    }
}

FwSizeType AosDeframer::sizeSppPacket(U8* payloadStart, FwSizeType payloadSize) {
    SpacePacketHeader header;

    Fw::Buffer data(payloadStart, payloadSize);
    Fw::SerializeStatus status = data.getDeserializer().deserializeTo(header);

    if (status != Fw::FW_SERIALIZE_OK) {
        return 0;  // Incomplete header - spans to next frame
    }

    // Per CCSDS 133.0-B-2 Section 4.1.3.5.2, packet data length = (actual length - 1)
    // packetDataLength is a 16-bit field (max 65535); SERIALIZED_SIZE is a small constant.
    // Guarantee at compile time that the maximum possible sum fits in FwSizeType. If
    // FwSizeType is ever narrowed below 17 bits, this fails to build and the addition
    // below must be guarded the same way sizeEppPacket is.
    constexpr FwSizeType MAX_LENGTH = std::numeric_limits<FwSizeType>::max() - SpacePacketHeader::SERIALIZED_SIZE;
    static_assert(MAX_LENGTH >= std::numeric_limits<U16>::max() + 1,
                  "FwSizeType must be wide enough to hold the maximum SPP packet size without overflow");
    FwSizeType totalPacketSize = SpacePacketHeader::SERIALIZED_SIZE + header.get_packetDataLength() + 1;

    // TODO: Unify Deframers | bring the whole spp processing into this component
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
    // Since we identified this as an EPP we had the 1 byte to read the PVN already
    FW_ASSERT(payloadSize > 0, static_cast<FwAssertArgType>(payloadSize));

    // Parse first byte
    U8 firstByte = payloadStart[0];
    U8 protocolId = static_cast<U8>((firstByte & EPPSubfields::protocolIdMask) >> EPPSubfields::protocolIdOffset);

    FwSizeType totalPacketSize = 0;

    // Idle means this is the last packet in the frame
    if (protocolId == static_cast<U8>(EppProtocolId::Idle)) {
        return 0;
    }

    // Encapsulation Idle Packet per CCSDS 133.1-B-3 Section 4.1.3.2
    U8 lengthOfLength = firstByte & EPPSubfields::lengthOfLengthMask;

    U8 lengthOffset = 1U;

    // If length of length is 2 or more then there's an extra byte of extension/user defined (4.1.2.1.1)
    if (lengthOfLength >= EppLengthOfLength::Two) {
        lengthOffset = static_cast<U8>(lengthOffset + 1U);
    }

    // If length of length is 4 then we add 2 bytes for the ccsds reserved field (4.1.2.1.1)
    if (lengthOfLength == EppLengthOfLength::Four) {
        lengthOffset = static_cast<U8>(lengthOffset + 2U);
        // '0d3' on the wire, but means 4
        lengthOfLength = 4;
    }

    // Bytes to get to length + length of length
    const U8 headerLength = static_cast<U8>(lengthOffset + lengthOfLength);

    // Validate and read length field
    if (payloadSize < headerLength) {
        return 0;  // Incomplete
    }

    // Read length field (big-endian)
    U32 packetDataLength = 0;
    for (U8 i = 0; i < lengthOfLength; i++) {
        packetDataLength = (packetDataLength << 8) | payloadStart[lengthOffset + i];
    }

    // Guard against integer overflow and return 0 as incomplete/invalid (if true).
    // This fires on 32-bit targets (FwSizeType = U32) where the sum would wrap.
    if (packetDataLength > (std::numeric_limits<FwSizeType>::max() - static_cast<FwSizeType>(headerLength))) {
        return 0;
    }

    // Cast both operands to FwSizeType BEFORE adding.
    // Without the cast, C++ computes headerLength(U8) + packetDataLength(U32) in U32
    // arithmetic, which wraps on both 32-bit and 64-bit hosts even when FwSizeType is
    // 64 bits wide.  The guard above is not sufficient on its own: on 64-bit it never
    // fires (packetDataLength can never exceed UINT64_MAX-8), so the unguarded addition
    // would still silently truncate to 4 on a 64-bit host.
    totalPacketSize = static_cast<FwSizeType>(headerLength) + static_cast<FwSizeType>(packetDataLength);

    return totalPacketSize;
}

U8 AosDeframer::getPacketVersion(U8 firstByte) {
    // PVN is the upper 3 bits per both CCSDS 133.0-B-2 and 133.1-B-3
    // EPP's Subfield array is done in bytes
    return static_cast<U8>(firstByte >> EPPSubfields::packetVersionOffset);
}

}  // namespace Ccsds
}  // namespace Svc
