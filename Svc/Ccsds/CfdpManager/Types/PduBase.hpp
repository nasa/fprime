// ======================================================================
// \title  PduBase.hpp
// \author Brian Campuzano
// \brief  Base class for all CFDP PDU types
//
// This base class provides a common interface for all PDU types,
// enabling proper construction/destruction and type safety.
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_PduBase_HPP
#define Svc_Ccsds_Cfdp_PduBase_HPP

#include <Fw/Types/Serializable.hpp>
#include <Fw/Buffer/Buffer.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Svc/Ccsds/CfdpManager/Types/PduHeader.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

//! Base class for all CFDP PDUs
//! Inherits from Fw::Serializable for F-Prime ecosystem integration
class PduBase : public Fw::Serializable {
  protected:
    //! The PDU header (common to all PDUs)
    PduHeader m_header;

  public:
    //! Constructor
    PduBase() {}

    //! Virtual destructor for proper cleanup
    virtual ~PduBase() = default;

    //! Fw::Serializable interface - serialize to buffer
    //! @param buffer The buffer to serialize to
    //! @param mode The endianness mode (default: BIG)
    //! @return Serialization status
    virtual Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer,
                                            Fw::Endianness mode = Fw::Endianness::BIG) const override = 0;

    //! Fw::Serializable interface - deserialize from buffer
    //! @param buffer The buffer to deserialize from
    //! @param mode The endianness mode (default: BIG)
    //! @return Deserialization status
    virtual Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer,
                                                Fw::Endianness mode = Fw::Endianness::BIG) override = 0;

    //! Get the buffer size needed to hold this PDU
    //! @return Buffer size in bytes
    virtual U32 getBufferSize() const = 0;

    //! Get the PDU type
    //! @return PDU type
    PduTypeEnum getType() const { return this->m_header.getType(); }

    //! Get the direction
    //! @return PduDirection (toward receiver or sender)
    PduDirection getDirection() const { return this->m_header.getDirection(); }

    //! Get the transmission mode
    //! @return Transmission mode (Class 1 or Class 2)
    Cfdp::Class::T getTxmMode() const { return this->m_header.getTxmMode(); }

    //! Get the source entity ID
    //! @return Source entity ID
    EntityId getSourceEid() const { return this->m_header.getSourceEid(); }

    //! Get the transaction sequence number
    //! @return Transaction sequence number
    TransactionSeq getTransactionSeq() const { return this->m_header.getTransactionSeq(); }

    //! Get the destination entity ID
    //! @return Destination entity ID
    EntityId getDestEid() const { return this->m_header.getDestEid(); }

    //! Get the header
    //! @return Reference to the PDU header
    const PduHeader& getHeader() const { return this->m_header; }
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

// Include all concrete PDU types for convenience (umbrella header pattern)
#include <Svc/Ccsds/CfdpManager/Types/MetadataPdu.hpp>
#include <Svc/Ccsds/CfdpManager/Types/FileDataPdu.hpp>
#include <Svc/Ccsds/CfdpManager/Types/EofPdu.hpp>
#include <Svc/Ccsds/CfdpManager/Types/FinPdu.hpp>
#include <Svc/Ccsds/CfdpManager/Types/AckPdu.hpp>
#include <Svc/Ccsds/CfdpManager/Types/NakPdu.hpp>

#endif  // Svc_Ccsds_Cfdp_PduBase_HPP
