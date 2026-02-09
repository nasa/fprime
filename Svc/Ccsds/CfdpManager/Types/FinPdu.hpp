// ======================================================================
// \title  FinPdu.hpp
// \author campuzan
// \brief  hpp file for CFDP Finished PDU
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_FinPdu_HPP
#define Svc_Ccsds_Cfdp_FinPdu_HPP

#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>
#include <Svc/Ccsds/CfdpManager/Types/Types.hpp>
#include <Svc/Ccsds/CfdpManager/Types/Tlv.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

//! The type of a Finished PDU
class FinPdu : public PduBase {
  private:
    //! Condition code
    ConditionCode m_conditionCode;

    //! Delivery code
    FinDeliveryCode m_deliveryCode;

    //! File status
    FinFileStatus m_fileStatus;

    //! TLV list (optional)
    TlvList m_tlvList;

  public:
    //! Constructor
    FinPdu() : m_conditionCode(CONDITION_CODE_NO_ERROR),
               m_deliveryCode(FIN_DELIVERY_CODE_COMPLETE),
               m_fileStatus(FIN_FILE_STATUS_RETAINED) {}

    //! Initialize a Finished PDU
    void initialize(PduDirection direction,
                   Cfdp::Class::T txmMode,
                   EntityId sourceEid,
                   TransactionSeq transactionSeq,
                   EntityId destEid,
                   ConditionCode conditionCode,
                   FinDeliveryCode deliveryCode,
                   FinFileStatus fileStatus);

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

    //! Get delivery code
    FinDeliveryCode getDeliveryCode() const { return this->m_deliveryCode; }

    //! Get file status
    FinFileStatus getFileStatus() const { return this->m_fileStatus; }

    //! Get directive code
    FileDirective getDirectiveCode() const { return FILE_DIRECTIVE_FIN; }

    //! Add a TLV to this FIN PDU
    //! @return true if added successfully, false if list is full
    bool appendTlv(const Tlv& tlv) { return this->m_tlvList.appendTlv(tlv); }

    //! Get TLV list
    const TlvList& getTlvList() const { return this->m_tlvList; }

    //! Get number of TLVs
    U8 getNumTlv() const { return this->m_tlvList.getNumTlv(); }

  private:
    //! Initialize this FinPdu from a SerialBuffer
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBufferBase& serialBuffer);

    //! Write this FinPdu to a SerialBuffer
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const;
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_FinPdu_HPP
