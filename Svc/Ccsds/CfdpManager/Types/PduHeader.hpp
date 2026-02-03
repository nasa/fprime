// ======================================================================
// \title  PduHeader.hpp
// \author campuzan
// \brief  hpp file for CFDP PDU Header
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_PduHeader_HPP
#define Svc_Ccsds_Cfdp_PduHeader_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/Serializable.hpp>
#include <Fw/Buffer/Buffer.hpp>
#include <config/CfdpEntityIdAliasAc.hpp>
#include <config/CfdpTransactionSeqAliasAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/ClassEnumAc.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// CFDP PDU Type
enum PduType : U8 {
    PDU_TYPE_DIRECTIVE = 0,  // File directive PDU
    PDU_TYPE_FILE_DATA = 1   // File data PDU
};

// CFDP Direction
enum Direction : U8 {
    DIRECTION_TOWARD_RECEIVER = 0,  // Toward file receiver
    DIRECTION_TOWARD_SENDER = 1     // Toward file sender
};

// CFDP CRC Flag
enum CrcFlag : U8 {
    CRC_NOT_PRESENT = 0,  // CRC not present
    CRC_PRESENT = 1       // CRC present
};

// CFDP Large File Flag
enum LargeFileFlag : U8 {
    LARGE_FILE_32_BIT = 0,  // 32-bit file size
    LARGE_FILE_64_BIT = 1   // 64-bit file size
};

// PDU type enum (discriminator for the union and for type identification)
enum PduTypeEnum : U8 {
    T_METADATA = 0,
    T_EOF = 1,
    T_FIN = 2,
    T_ACK = 3,
    T_NAK = 4,
    T_FILE_DATA = 5,
    T_NONE = 255
};

//! The type of a PDU header (common to all PDUs)
class PduHeader {
  private:
    //! PDU type (derived from directive code or file data flag)
    PduTypeEnum m_type;

    //! CFDP version (should be 1)
    U8 m_version;

    //! PDU type
    PduType m_pduType;

    //! Direction
    Direction m_direction;

    //! Transmission mode
    Cfdp::Class::T m_class;

    //! CRC flag
    CrcFlag m_crcFlag;

    //! Large file flag
    LargeFileFlag m_largeFileFlag;

    //! Segmentation control
    U8 m_segmentationControl;

    //! Segment metadata flag
    U8 m_segmentMetadataFlag;

    //! PDU data length (excluding header)
    U16 m_pduDataLength;

    //! Source entity ID
    CfdpEntityId m_sourceEid;

    //! Transaction sequence number
    CfdpTransactionSeq m_transactionSeq;

    //! Destination entity ID
    CfdpEntityId m_destEid;

  public:
    //! Header size (variable due to EID/TSN lengths)
    enum { MIN_HEADERSIZE = 7 }; // Minimum fixed portion

    //! Initialize a PDU header
    void initialize(PduTypeEnum type,
                   Direction direction,
                   Cfdp::Class::T txmMode,
                   CfdpEntityId sourceEid,
                   CfdpTransactionSeq transactionSeq,
                   CfdpEntityId destEid);

    //! Compute the buffer size needed to hold this Header
    U32 getBufferSize() const;

    //! Calculate the number of bytes needed to encode a value
    //! @param value The value to encode
    //! @return Number of bytes needed (1, 2, 4, or 8)
    static U8 getValueEncodedSize(U64 value);

    //! Initialize this Header from a SerialBufferBase
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBufferBase& serialBuffer);

    //! Write this Header to a SerialBufferBase
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const;

    //! Get the PDU type
    PduTypeEnum getType() const { return this->m_type; }

    //! Get the direction
    Direction getDirection() const { return this->m_direction; }

    //! Get the transmission mode
    Cfdp::Class::T getTxmMode() const { return this->m_class; }

    //! Get the source entity ID
    CfdpEntityId getSourceEid() const { return this->m_sourceEid; }

    //! Get the transaction sequence number
    CfdpTransactionSeq getTransactionSeq() const { return this->m_transactionSeq; }

    //! Get the destination entity ID
    CfdpEntityId getDestEid() const { return this->m_destEid; }

    //! Get PDU data length
    U16 getPduDataLength() const { return this->m_pduDataLength; }

    //! Set PDU data length (used during encoding)
    void setPduDataLength(U16 length) { this->m_pduDataLength = length; }

    //! Get the large file flag
    LargeFileFlag getLargeFileFlag() const { return this->m_largeFileFlag; }

    //! Check if segment metadata is present
    bool hasSegmentMetadata() const { return this->m_segmentMetadataFlag != 0; }

    //! Set the large file flag (used for testing and configuration)
    void setLargeFileFlag(LargeFileFlag flag) { this->m_largeFileFlag = flag; }

    //! Allow PDU classes to access private members
    friend class MetadataPdu;
    friend class FileDataPdu;
    friend class EofPdu;
    friend class FinPdu;
    friend class AckPdu;
    friend class NakPdu;
};

//! Peek at the PDU type from a buffer without consuming it
//! @param buffer The buffer containing the PDU
//! @return The PDU type, or T_NONE if the buffer is invalid
PduTypeEnum peekPduType(const Fw::Buffer& buffer);

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_PduHeader_HPP
