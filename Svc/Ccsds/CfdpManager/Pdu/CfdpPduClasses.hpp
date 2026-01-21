// ======================================================================
// \title  CfdpPdu.hpp
// \author campuzan
// \brief  hpp file for CFDP PDU classes
//
// \copyright
// Copyright 2025, California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Ccsds_CfdpPdu_HPP
#define Svc_Ccsds_CfdpPdu_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Fw/Types/Serializable.hpp>
#include <config/CfdpCfg.hpp>
#include <config/CfdpEntityIdAliasAc.hpp>
#include <config/CfdpTransactionSeqAliasAc.hpp>

#include <Svc/Ccsds/Types/CfdpFileDirectiveEnumAc.hpp>
#include <Svc/Ccsds/Types/CfdpChecksumTypeEnumAc.hpp>
#include <Svc/Ccsds/Types/CfdpConditionCodeEnumAc.hpp>
#include <Svc/Ccsds/Types/CfdpFinDeliveryCodeEnumAc.hpp>
#include <Svc/Ccsds/Types/CfdpFinFileStatusEnumAc.hpp>
#include <Svc/Ccsds/Types/CfdpAckTxnStatusEnumAc.hpp>

namespace Svc {
namespace Ccsds {

//! \class CfdpPdu
//! \brief A CFDP PDU following the FilePacket pattern
//!
union CfdpPdu {
  public:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! PDU type for directive codes
    typedef enum {
        T_METADATA = 0,
        T_EOF = 1,
        T_FIN = 2,
        T_ACK = 3,
        T_NAK = 4,
        T_FILE_DATA = 5,
        T_NONE = 255
    } Type;

    //! The type of a PDU header (common to all PDUs)
    class Header {
        friend union CfdpPdu;
        friend class MetadataPdu;
        friend class FileDataPdu;
        friend class EofPdu;
        friend class FinPdu;
        friend class AckPdu;
        friend class NakPdu;

      private:
        //! PDU type (derived from directive code or file data flag)
        Type m_type;

        //! CFDP version (should be 1)
        U8 m_version;

        //! PDU type: 0=directive, 1=file data
        U8 m_pduType;

        //! Direction: 0=toward receiver, 1=toward sender
        U8 m_direction;

        //! Transmission mode: 0=acknowledged, 1=unacknowledged
        U8 m_txmMode;

        //! CRC flag: 0=not present, 1=present
        U8 m_crcFlag;

        //! Large file flag: 0=32-bit, 1=64-bit
        U8 m_largeFileFlag;

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
        void initialize(Type type,
                       U8 direction,
                       U8 txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid);

        //! Compute the buffer size needed to hold this Header
        U32 bufferSize() const;

        //! Initialize this Header from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this Header to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;

        //! Get the PDU type
        Type getType() const { return this->m_type; }

        //! Get the direction
        U8 getDirection() const { return this->m_direction; }

        //! Get the transmission mode
        U8 getTxmMode() const { return this->m_txmMode; }

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
    };

    //! The type of a Metadata PDU
    class MetadataPdu {
        friend union CfdpPdu;

      private:
        //! The PDU header
        Header m_header;

        //! Closure requested flag
        U8 m_closureRequested;

        //! Checksum type
        CfdpChecksumType m_checksumType;

        //! File size
        CfdpFileSize m_fileSize;

        //! Source filename length
        U8 m_sourceFilenameLength;

        //! Source filename
        const char* m_sourceFilename;

        //! Destination filename length
        U8 m_destFilenameLength;

        //! Destination filename
        const char* m_destFilename;

      public:
        //! Initialize a Metadata PDU
        void initialize(U8 direction,
                       U8 txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       U32 fileSize,
                       const char* sourceFilename,
                       const char* destFilename,
                       CfdpChecksumType checksumType,
                       U8 closureRequested);

        //! Compute the buffer size needed
        U32 bufferSize() const;

        //! Convert this MetadataPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get the file size
        U32 getFileSize() const { return this->m_fileSize; }

        //! Get the source filename
        const char* getSourceFilename() const { return this->m_sourceFilename; }

        //! Get the destination filename
        const char* getDestFilename() const { return this->m_destFilename; }

        //! Get checksum type
        CfdpChecksumType getChecksumType() const { return this->m_checksumType; }

        //! Get closure requested flag
        U8 getClosureRequested() const { return this->m_closureRequested; }

      private:
        //! Initialize this MetadataPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this MetadataPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of a File Data PDU
    class FileDataPdu {
        friend union CfdpPdu;

      private:
        //! The PDU header
        Header m_header;

        //! File offset
        U32 m_offset;

        //! Data size
        U16 m_dataSize;

        //! Pointer to file data
        const U8* m_data;

      public:
        //! Initialize a File Data PDU
        void initialize(U8 direction,
                       U8 txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       U32 offset,
                       U16 dataSize,
                       const U8* data);

        //! Compute the buffer size needed
        U32 bufferSize() const;

        //! Convert this FileDataPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get the file offset
        U32 getOffset() const { return this->m_offset; }

        //! Get the data size
        U16 getDataSize() const { return this->m_dataSize; }

        //! Get the data pointer
        const U8* getData() const { return this->m_data; }

      private:
        //! Initialize this FileDataPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this FileDataPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of an EOF PDU
    class EofPdu {
        friend union CfdpPdu;

      private:
        //! The PDU header
        Header m_header;

        //! Condition code
        CfdpConditionCode m_conditionCode;

        //! File checksum
        U32 m_checksum;

        //! File size
        U32 m_fileSize;

      public:
        //! Initialize an EOF PDU
        void initialize(U8 direction,
                       U8 txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       CfdpConditionCode conditionCode,
                       U32 checksum,
                       U32 fileSize);

        //! Compute the buffer size needed
        U32 bufferSize() const;

        //! Convert this EofPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get condition code
        CfdpConditionCode getConditionCode() const { return this->m_conditionCode; }

        //! Get checksum
        U32 getChecksum() const { return this->m_checksum; }

        //! Get file size
        U32 getFileSize() const { return this->m_fileSize; }

      private:
        //! Initialize this EofPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this EofPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of a Finished PDU
    class FinPdu {
        friend union CfdpPdu;

      private:
        //! The PDU header
        Header m_header;

        //! Condition code
        CfdpConditionCode m_conditionCode;

        //! Delivery code
        CfdpFinDeliveryCode m_deliveryCode;

        //! File status
        CfdpFinFileStatus m_fileStatus;

      public:
        //! Initialize a Finished PDU
        void initialize(U8 direction,
                       U8 txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       CfdpConditionCode conditionCode,
                       CfdpFinDeliveryCode deliveryCode,
                       CfdpFinFileStatus fileStatus);

        //! Compute the buffer size needed
        U32 bufferSize() const;

        //! Convert this FinPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get condition code
        CfdpConditionCode getConditionCode() const { return this->m_conditionCode; }

        //! Get delivery code
        CfdpFinDeliveryCode getDeliveryCode() const { return this->m_deliveryCode; }

        //! Get file status
        CfdpFinFileStatus getFileStatus() const { return this->m_fileStatus; }

      private:
        //! Initialize this FinPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this FinPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of an ACK PDU
    class AckPdu {
        friend union CfdpPdu;

      private:
        //! The PDU header
        Header m_header;

        //! Directive being acknowledged
        CfdpFileDirective m_directiveCode;

        //! Directive subtype code
        U8 m_directiveSubtypeCode;

        //! Condition code
        CfdpConditionCode m_conditionCode;

        //! Transaction status
        CfdpAckTxnStatus m_transactionStatus;

      public:
        //! Initialize an ACK PDU
        void initialize(U8 direction,
                       U8 txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       CfdpFileDirective directiveCode,
                       U8 directiveSubtypeCode,
                       CfdpConditionCode conditionCode,
                       CfdpAckTxnStatus transactionStatus);

        //! Compute the buffer size needed
        U32 bufferSize() const;

        //! Convert this AckPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get directive code
        CfdpFileDirective getDirectiveCode() const { return this->m_directiveCode; }

        //! Get directive subtype code
        U8 getDirectiveSubtypeCode() const { return this->m_directiveSubtypeCode; }

        //! Get condition code
        CfdpConditionCode getConditionCode() const { return this->m_conditionCode; }

        //! Get transaction status
        CfdpAckTxnStatus getTransactionStatus() const { return this->m_transactionStatus; }

      private:
        //! Initialize this AckPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this AckPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of a NAK PDU
    class NakPdu {
        friend union CfdpPdu;

      private:
        //! The PDU header
        Header m_header;

        //! Scope start offset
        U32 m_scopeStart;

        //! Scope end offset
        U32 m_scopeEnd;

      public:
        //! Initialize a NAK PDU
        void initialize(U8 direction,
                       U8 txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       U32 scopeStart,
                       U32 scopeEnd);

        //! Compute the buffer size needed
        U32 bufferSize() const;

        //! Convert this NakPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get scope start
        U32 getScopeStart() const { return this->m_scopeStart; }

        //! Get scope end
        U32 getScopeEnd() const { return this->m_scopeEnd; }

      private:
        //! Initialize this NakPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this NakPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

  public:
    // ----------------------------------------------------------------------
    // Constructor
    // ----------------------------------------------------------------------

    CfdpPdu() { this->m_header.m_type = T_NONE; }

  public:
    // ----------------------------------------------------------------------
    // Public instance methods
    // ----------------------------------------------------------------------

    //! Initialize this from a Buffer
    //!
    Fw::SerializeStatus fromBuffer(const Fw::Buffer& buffer);

    //! Get this as a Header
    //!
    const Header& asHeader() const;

    //! Get this as a MetadataPdu
    //!
    const MetadataPdu& asMetadataPdu() const;

    //! Get this as a FileDataPdu
    //!
    const FileDataPdu& asFileDataPdu() const;

    //! Get this as an EofPdu
    //!
    const EofPdu& asEofPdu() const;

    //! Get this as a FinPdu
    //!
    const FinPdu& asFinPdu() const;

    //! Get this as an AckPdu
    //!
    const AckPdu& asAckPdu() const;

    //! Get this as a NakPdu
    //!
    const NakPdu& asNakPdu() const;

    //! Initialize this with a MetadataPdu
    //!
    void fromMetadataPdu(const MetadataPdu& metadataPdu);

    //! Initialize this with a FileDataPdu
    //!
    void fromFileDataPdu(const FileDataPdu& fileDataPdu);

    //! Initialize this with an EofPdu
    //!
    void fromEofPdu(const EofPdu& eofPdu);

    //! Initialize this with a FinPdu
    //!
    void fromFinPdu(const FinPdu& finPdu);

    //! Initialize this with an AckPdu
    //!
    void fromAckPdu(const AckPdu& ackPdu);

    //! Initialize this with a NakPdu
    //!
    void fromNakPdu(const NakPdu& nakPdu);

    //! Get the buffer size needed to hold this CfdpPdu
    //!
    U32 bufferSize() const;

    //! Convert this CfdpPdu to a Buffer
    //!
    Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

  private:
    // ----------------------------------------------------------------------
    // Private methods
    // ----------------------------------------------------------------------

    //! Initialize this from a SerialBuffer
    //!
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

  private:
    // ----------------------------------------------------------------------
    // Private data
    // ----------------------------------------------------------------------

    //! this, seen as a header
    //!
    Header m_header;

    //! this, seen as a Metadata PDU
    //!
    MetadataPdu m_metadataPdu;

    //! this, seen as a File Data PDU
    //!
    FileDataPdu m_fileDataPdu;

    //! this, seen as an EOF PDU
    //!
    EofPdu m_eofPdu;

    //! this, seen as a Finished PDU
    //!
    FinPdu m_finPdu;

    //! this, seen as an ACK PDU
    //!
    AckPdu m_ackPdu;

    //! this, seen as a NAK PDU
    //!
    NakPdu m_nakPdu;
};

}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_CfdpPdu_HPP
