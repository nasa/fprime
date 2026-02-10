// ======================================================================
// \title  EofPdu.hpp
// \author Brian Campuzano
// \brief  hpp file for CFDP EOF PDU
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_EofPdu_HPP
#define Svc_Ccsds_Cfdp_EofPdu_HPP

#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>
#include <Svc/Ccsds/CfdpManager/Types/Types.hpp>
#include <Svc/Ccsds/CfdpManager/Types/Tlv.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

//! The type of an EOF PDU
class EofPdu : public PduBase {
  private:
    //! Condition code
    ConditionCode m_conditionCode;

    //! File checksum
    U32 m_checksum;

    //! File size
    FileSize m_fileSize;

    //! TLV list (optional)
    TlvList m_tlvList;

  public:
    //! Constructor
    EofPdu() : m_conditionCode(CONDITION_CODE_NO_ERROR), m_checksum(0), m_fileSize(0) {}

    //! Initialize an EOF PDU
    void initialize(PduDirection direction,
                   Cfdp::Class::T txmMode,
                   EntityId sourceEid,
                   TransactionSeq transactionSeq,
                   EntityId destEid,
                   ConditionCode conditionCode,
                   U32 checksum,
                   FileSize fileSize);

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

    //! Get condition code
    ConditionCode getConditionCode() const { return this->m_conditionCode; }

    //! Get checksum
    U32 getChecksum() const { return this->m_checksum; }

    //! Get file size
    FileSize getFileSize() const { return this->m_fileSize; }

    //! Get directive code
    FileDirective getDirectiveCode() const { return FILE_DIRECTIVE_END_OF_FILE; }

    //! Add a TLV to this EOF PDU
    //! @return true if added successfully, false if list is full
    bool appendTlv(const Tlv& tlv) { return this->m_tlvList.appendTlv(tlv); }

    //! Get TLV list
    const TlvList& getTlvList() const { return this->m_tlvList; }

    //! Get number of TLVs
    U8 getNumTlv() const { return this->m_tlvList.getNumTlv(); }

  private:
    //! Initialize this EofPdu from a SerialBuffer
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBufferBase& serialBuffer);

    //! Write this EofPdu to a SerialBuffer
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const;
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_EofPdu_HPP
