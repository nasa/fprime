// ======================================================================
// \title  TcDeframer.cpp
// \author thomas-bc
// \brief  cpp file for TcDeframer component implementation class
// ======================================================================

#include "Svc/Ccsds/TcDeframer/TcDeframer.hpp"
#include <cstring>
#include "Svc/Ccsds/Types/FppConstantsAc.hpp"
#include "Svc/Ccsds/Types/TCHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCSegmentHeaderSerializableAc.hpp"
#include "Svc/Ccsds/Types/TCTrailerSerializableAc.hpp"
#include "Svc/Ccsds/Utils/CRC16.hpp"
#include "config/FpConfig.hpp"

namespace Svc {
namespace Ccsds {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

TcDeframer ::TcDeframer(const char* const compName)
    : TcDeframerComponentBase(compName), m_spacecraftId(ComCfg::SpacecraftId) {}

TcDeframer ::~TcDeframer() {}

void TcDeframer::configure(U16 vcId, U16 spacecraftId, bool acceptAllVcid) {
    this->m_vcId = vcId;
    this->m_spacecraftId = spacecraftId;
    this->m_acceptAllVcid = acceptAllVcid;
}

void TcDeframer::configureSegmentation(bool segmentHeaderPresent, U8 mapId, FwSizeType maxSpanningPacketSize) {
    if (segmentHeaderPresent) {
        FW_ASSERT(this->isConnected_allocate_OutputPort(0));
        FW_ASSERT(this->isConnected_deallocate_OutputPort(0));
        FW_ASSERT(maxSpanningPacketSize > 0);
        FW_ASSERT((mapId & TCSubfields::SegmentMapIdMask) == mapId, mapId);
    }
    this->m_segmentHeaderPresent = segmentHeaderPresent;
    this->m_mapId = mapId;
    this->m_maxSpanningPacketSize = maxSpanningPacketSize;
}
// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void TcDeframer ::dataIn_handler(FwIndexType portNum, Fw::Buffer& data, const ComCfg::FrameContext& context) {
    // CCSDS TC Format:
    // 5 octets - TC Primary Header
    // Up to 1019 octets - Data Field (including optional 2 octets frame error control field)

    // Note: F Prime uses Type-BD
    // CCSDS TC Primary Header:
    // 2b - 00  - TF Version Number
    // 1b - 0/1 - Bypass Flag            (0 = Type-A FARM checks enabled, 1 = Type-B FARM checks bypassed)
    // 1b - 0/1 - Control Command Flag   (0 = Type-D data, 1 = Type-C control command)
    // 2b - 00  - Reserved Spare         (set to 00)
    // 10b- XX  - Spacecraft ID
    // 6b - XX  - Virtual Channel ID
    // 10b- XX  - Frame Length
    // 8b - XX  - Frame Sequence Number  (unused for Type-B frames)

    // CCSDS TC Trailer:
    // 16b - Frame Error Control Field (FECF): CRC16
    if (data.getSize() <= TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE) {
        // Incoming buffer is not long enough to contain a valid frame (header+trailer)
        this->log_WARNING_LO_InvalidPacket();
        this->dataReturnOut_out(0, data, context);
        return;
    }

    TCHeader header;
    Fw::SerializeStatus status = data.getDeserializer().deserializeTo(header);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    // TC protocol defines the Frame Length as number of bytes minus 1, so we add 1 back to get length in bytes
    U16 total_frame_length = static_cast<U16>((header.get_vcIdAndLength() & TCSubfields::FrameLengthMask) + 1);
    U8 vc_id = static_cast<U8>((header.get_vcIdAndLength() & TCSubfields::VcIdMask) >> TCSubfields::VcIdOffset);
    U16 spacecraft_id = header.get_flagsAndScId() & TCSubfields::SpacecraftIdMask;

    if (spacecraft_id != this->m_spacecraftId) {
        this->log_WARNING_LO_InvalidSpacecraftId(spacecraft_id, this->m_spacecraftId);
        this->errorNotifyHelper(Ccsds::FrameError::TC_INVALID_SCID);
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }
    // check that deserialized frame_length is at least large enough to hold header and trailer, and
    // that it is not larger than the actual data available in the buffer
    if ((data.getSize() < static_cast<Fw::Buffer::SizeType>(total_frame_length)) or
        (total_frame_length < TCHeader::SERIALIZED_SIZE + TCTrailer::SERIALIZED_SIZE)) {
        FwSizeType maxDataAvailable = static_cast<FwSizeType>(data.getSize());
        this->log_WARNING_HI_InvalidFrameLength(total_frame_length, maxDataAvailable);
        this->errorNotifyHelper(Ccsds::FrameError::TC_INVALID_LENGTH);
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }
    if (not this->m_acceptAllVcid && vc_id != this->m_vcId) {
        this->log_ACTIVITY_LO_InvalidVcId(vc_id, this->m_vcId);
        this->errorNotifyHelper(Ccsds::FrameError::TC_INVALID_VCID);
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }
    // Note: F Prime uses TC Type-BD frames for now, so the FARM checks are not ran
    // This means there is no sequence count checks at the TC level (there are at the Space Packet level)

    // -------------------------------------------------
    // CRC Check
    // -------------------------------------------------
    // Compute CRC over the entire frame buffer minus the FECF trailer
    U16 computed_crc = Ccsds::Utils::CRC16::compute(data.getData(), total_frame_length - TCTrailer::SERIALIZED_SIZE);
    TCTrailer trailer;
    auto deserializer = data.getDeserializer();
    status = deserializer.moveDeserToOffset(total_frame_length - TCTrailer::SERIALIZED_SIZE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    status = deserializer.deserializeTo(trailer);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    U16 transmitted_crc = trailer.get_fecf();
    if (transmitted_crc != computed_crc) {
        this->log_WARNING_HI_InvalidCrc(computed_crc, transmitted_crc);
        this->errorNotifyHelper(Ccsds::FrameError::TC_INVALID_CRC);
        this->dataReturnOut_out(0, data, context);  // drop the frame
        return;
    }

    // Point to the start of the data field and set appropriate size
    data.advance(TCHeader::SERIALIZED_SIZE);
    // Shrink size to that of the encapsulated data field ( header | data | trailer )
    data.setSize(total_frame_length - TCHeader::SERIALIZED_SIZE - TCTrailer::SERIALIZED_SIZE);

    // Carry the VC on in the context
    ComCfg::FrameContext contextCopy = context;
    contextCopy.set_vcId(vc_id);

    if (this->m_segmentHeaderPresent) {
        this->handleSegment(data, context, contextCopy);
        return;
    }
    this->dataOut_out(0, data, contextCopy);
}

void TcDeframer ::dataReturnIn_handler(FwIndexType portNum, Fw::Buffer& fwBuffer, const ComCfg::FrameContext& context) {
    // Reassembled packets were allocated here and are deallocated here; anything else belongs upstream
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(TcDeframer_MaxSpanningPacketsInFlight); i++) {
        if (this->m_inFlight[i].isValid() && (this->m_inFlight[i].getOriginalData() == fwBuffer.getOriginalData())) {
            Fw::Buffer allocated = this->m_inFlight[i];
            this->m_inFlight[i] = Fw::Buffer();
            this->deallocate_out(0, allocated);
            return;
        }
    }
    this->dataReturnOut_out(0, fwBuffer, context);
}

void TcDeframer::errorNotifyHelper(Ccsds::FrameError error) {
    if (this->isConnected_errorNotify_OutputPort(0)) {
        this->errorNotify_out(0, error);
    }
}

// ----------------------------------------------------------------------
// Segmentation helpers (CCSDS 232.0-B-4 Section 4.1.3.3)
// ----------------------------------------------------------------------

void TcDeframer::handleSegment(Fw::Buffer& data,
                               const ComCfg::FrameContext& frameContext,
                               const ComCfg::FrameContext& dataContext) {
    // TC Segment Header (1 octet): 2b sequence flags | 6b MAP ID, followed by at least one octet of segment data
    if (data.getSize() <= TCSegmentHeader::SERIALIZED_SIZE) {
        this->log_WARNING_LO_InvalidPacket();
        this->errorNotifyHelper(Ccsds::FrameError::TC_INVALID_LENGTH);
        this->dataReturnOut_out(0, data, frameContext);
        return;
    }
    TCSegmentHeader segmentHeader;
    Fw::SerializeStatus status = data.getDeserializer().deserializeTo(segmentHeader);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);
    const U8 mapId = static_cast<U8>(segmentHeader.get_flagsAndMapId() & TCSubfields::SegmentMapIdMask);
    const TCSegmentSequenceFlags sequenceFlags(static_cast<TCSegmentSequenceFlags::T>(
        (segmentHeader.get_flagsAndMapId() & TCSubfields::SegmentSequenceFlagsMask) >>
        TCSubfields::SegmentSequenceFlagsOffset));

    if (mapId != this->m_mapId) {
        this->log_WARNING_LO_InvalidMapId(mapId, this->m_mapId);
        this->errorNotifyHelper(Ccsds::FrameError::TC_INVALID_MAP_ID);
        this->dataReturnOut_out(0, data, frameContext);
        return;
    }
    data.advance(TCSegmentHeader::SERIALIZED_SIZE);

    switch (sequenceFlags.e) {
        case TCSegmentSequenceFlags::UNSEGMENTED:
            // Whole packet in one frame: forward zero-copy, downstream returns it via dataReturnIn
            this->abandonSpanningPacket();
            this->dataOut_out(0, data, dataContext);
            return;
        case TCSegmentSequenceFlags::FIRST:
            this->abandonSpanningPacket();
            if (this->startSpanningPacket(dataContext)) {
                (void)this->appendToSpanningPacket(data);
            }
            break;
        case TCSegmentSequenceFlags::CONTINUING:
        case TCSegmentSequenceFlags::LAST:
            if (not this->isSpanningPacketInProgress()) {
                this->log_WARNING_HI_UnexpectedSegment(sequenceFlags);
                this->errorNotifyHelper(Ccsds::FrameError::TC_SEGMENT_UNEXPECTED);
            } else if (this->appendToSpanningPacket(data) && (sequenceFlags == TCSegmentSequenceFlags::LAST)) {
                this->completeSpanningPacket();
            }
            break;
        default:
            FW_ASSERT(0, static_cast<FwAssertArgType>(sequenceFlags.e));
            break;
    }
    // Segment data has been copied out (or dropped): the frame goes back upstream
    this->dataReturnOut_out(0, data, frameContext);
}

bool TcDeframer::startSpanningPacket(const ComCfg::FrameContext& context) {
    FW_ASSERT(not this->isSpanningPacketInProgress());
    this->m_spanningBuffer = this->allocate_out(0, this->m_maxSpanningPacketSize);
    if ((not this->m_spanningBuffer.isValid()) || (this->m_spanningBuffer.getSize() < this->m_maxSpanningPacketSize)) {
        this->log_WARNING_HI_SpanningPacketAllocFailed(this->m_maxSpanningPacketSize);
        this->errorNotifyHelper(Ccsds::FrameError::TC_SEGMENT_ALLOC_FAILED);
        this->discardSpanningPacket();
        return false;
    }
    this->m_spanningBytesReceived = 0;
    this->m_spanningContext = context;
    return true;
}

bool TcDeframer::appendToSpanningPacket(const Fw::Buffer& data) {
    FW_ASSERT(this->isSpanningPacketInProgress());
    const FwSizeType newSize = this->m_spanningBytesReceived + data.getSize();
    if (newSize > this->m_maxSpanningPacketSize) {
        this->log_WARNING_HI_SpanningPacketOverflow(newSize, this->m_maxSpanningPacketSize);
        this->errorNotifyHelper(Ccsds::FrameError::TC_SEGMENT_OVERFLOW);
        this->discardSpanningPacket();
        return false;
    }
    FW_ASSERT(newSize <= this->m_spanningBuffer.getSize(), static_cast<FwAssertArgType>(newSize),
              static_cast<FwAssertArgType>(this->m_spanningBuffer.getSize()));
    (void)::memcpy(this->m_spanningBuffer.getData() + this->m_spanningBytesReceived, data.getData(), data.getSize());
    this->m_spanningBytesReceived = newSize;
    return true;
}

void TcDeframer::completeSpanningPacket() {
    FW_ASSERT(this->isSpanningPacketInProgress());
    for (FwIndexType i = 0; i < static_cast<FwIndexType>(TcDeframer_MaxSpanningPacketsInFlight); i++) {
        if (not this->m_inFlight[i].isValid()) {
            // Track the full allocation for deallocation; downstream sees only the packet bytes
            this->m_inFlight[i] = this->m_spanningBuffer;
            Fw::Buffer packet = this->m_spanningBuffer;
            packet.setSize(this->m_spanningBytesReceived);
            this->m_spanningBuffer = Fw::Buffer();
            this->m_spanningBytesReceived = 0;
            this->dataOut_out(0, packet, this->m_spanningContext);
            return;
        }
    }
    this->log_WARNING_HI_SpanningPacketInFlightLimit(TcDeframer_MaxSpanningPacketsInFlight);
    this->errorNotifyHelper(Ccsds::FrameError::TC_SEGMENT_IN_FLIGHT_LIMIT);
    this->discardSpanningPacket();
}

void TcDeframer::abandonSpanningPacket() {
    if (this->isSpanningPacketInProgress()) {
        this->log_WARNING_HI_SpanningPacketAbandoned(this->m_spanningBytesReceived);
        this->discardSpanningPacket();
    }
}

void TcDeframer::discardSpanningPacket() {
    if (this->m_spanningBuffer.isValid()) {
        this->deallocate_out(0, this->m_spanningBuffer);
    }
    this->m_spanningBuffer = Fw::Buffer();
    this->m_spanningBytesReceived = 0;
}

bool TcDeframer::isSpanningPacketInProgress() const {
    return this->m_spanningBuffer.isValid();
}

}  // namespace Ccsds
}  // namespace Svc
