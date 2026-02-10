// ======================================================================
// \title  AckPdu.hpp
// \author Brian Campuzano
// \brief  hpp file for CFDP ACK PDU
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_AckPdu_HPP
#define Svc_Ccsds_Cfdp_AckPdu_HPP

#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>
#include <Svc/Ccsds/CfdpManager/Types/Types.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

//! The type of an ACK PDU
class AckPdu : public PduBase {
  private:
    //! Directive being acknowledged
    FileDirective m_directiveCode;

    //! Directive subtype code
    U8 m_directiveSubtypeCode;

    //! Condition code
    ConditionCode m_conditionCode;

    //! Transaction status
    AckTxnStatus m_transactionStatus;

  public:
    //! Constructor
    AckPdu() : m_directiveCode(FILE_DIRECTIVE_INVALID_MIN),
               m_directiveSubtypeCode(0),
               m_conditionCode(CONDITION_CODE_NO_ERROR),
               m_transactionStatus(ACK_TXN_STATUS_UNDEFINED) {}

    //! Initialize an ACK PDU
    void initialize(PduDirection direction,
                   Cfdp::Class::T txmMode,
                   EntityId sourceEid,
                   TransactionSeq transactionSeq,
                   EntityId destEid,
                   FileDirective directiveCode,
                   U8 directiveSubtypeCode,
                   ConditionCode conditionCode,
                   AckTxnStatus transactionStatus);

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

    //! Get directive code
    FileDirective getDirectiveCode() const { return this->m_directiveCode; }

    //! Get directive subtype code
    U8 getDirectiveSubtypeCode() const { return this->m_directiveSubtypeCode; }

    //! Get condition code
    ConditionCode getConditionCode() const { return this->m_conditionCode; }

    //! Get transaction status
    AckTxnStatus getTransactionStatus() const { return this->m_transactionStatus; }

  private:
    //! Initialize this AckPdu from a SerialBuffer
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBufferBase& serialBuffer);

    //! Write this AckPdu to a SerialBuffer
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const;
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_AckPdu_HPP
