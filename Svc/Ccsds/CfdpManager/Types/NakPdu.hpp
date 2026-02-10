// ======================================================================
// \title  NakPdu.hpp
// \author Brian Campuzano
// \brief  hpp file for CFDP NAK PDU
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_NakPdu_HPP
#define Svc_Ccsds_Cfdp_NakPdu_HPP

#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>
#include <Svc/Ccsds/CfdpManager/Types/Types.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

//! Segment request structure for NAK PDU
struct SegmentRequest {
    FileSize offsetStart;  //!< Start offset of missing data
    FileSize offsetEnd;    //!< End offset of missing data
};

//! The type of a NAK PDU
class NakPdu : public PduBase {
  private:
    //! Scope start offset
    FileSize m_scopeStart;

    //! Scope end offset
    FileSize m_scopeEnd;

    //! Number of segment requests
    U8 m_numSegments;

    //! Segment requests array (max CFDP_NAK_MAX_SEGMENTS = 58)
    SegmentRequest m_segments[58];

  public:
    //! Constructor
    NakPdu() : m_scopeStart(0), m_scopeEnd(0), m_numSegments(0) {}

    //! Initialize a NAK PDU
    void initialize(PduDirection direction,
                   Cfdp::Class::T txmMode,
                   EntityId sourceEid,
                   TransactionSeq transactionSeq,
                   EntityId destEid,
                   FileSize scopeStart,
                   FileSize scopeEnd);

    //! Compute the buffer size needed
    U32 getBufferSize() const override;

    //! Fw::Serializable interface - serialize to buffer
    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer,
                                    Fw::Endianness mode = Fw::Endianness::BIG) const override;

    //! Fw::Serializable interface - deserialize from buffer
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer,
                                        Fw::Endianness mode = Fw::Endianness::BIG) override;

    //! Get this as a Header
    const PduHeader& asHeader() const { return this->m_header; }

    //! Get scope start
    FileSize getScopeStart() const { return this->m_scopeStart; }

    //! Get scope end
    FileSize getScopeEnd() const { return this->m_scopeEnd; }

    //! Get number of segments
    U8 getNumSegments() const { return this->m_numSegments; }

    //! Get segment at index (no bounds checking - caller must verify index < getNumSegments())
    const SegmentRequest& getSegment(U8 index) const { return this->m_segments[index]; }

    //! Add a segment request
    //! @return True if segment was added, false if segment array is full
    bool addSegment(FileSize offsetStart, FileSize offsetEnd);

    //! Clear all segment requests
    void clearSegments();

    //! Get directive code
    FileDirective getDirectiveCode() const { return FILE_DIRECTIVE_NAK; }

  private:
    //! Initialize this NakPdu from a SerialBuffer
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBufferBase& serialBuffer);

    //! Write this NakPdu to a SerialBuffer
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const;
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_NakPdu_HPP
