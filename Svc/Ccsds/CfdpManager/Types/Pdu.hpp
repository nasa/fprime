// ======================================================================
// \title  Pdu.hpp
// \author campuzan
// \brief  hpp file for CFDP PDU classes
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_Pdu_HPP
#define Svc_Ccsds_Cfdp_Pdu_HPP

#include <Fw/Buffer/Buffer.hpp>
#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Fw/Types/Serializable.hpp>
#include <config/CfdpCfg.hpp>
#include <config/CfdpEntityIdAliasAc.hpp>
#include <config/CfdpTransactionSeqAliasAc.hpp>
#include <Svc/Ccsds/CfdpManager/Types/ClassEnumAc.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

// CFDP File Directive Codes
// Blue Book section 5.2, table 5-4
enum FileDirective : U8 {
    FILE_DIRECTIVE_INVALID_MIN = 0,  // Minimum used to limit range
    FILE_DIRECTIVE_END_OF_FILE = 4,  // End of File
    FILE_DIRECTIVE_FIN = 5,          // Finished
    FILE_DIRECTIVE_ACK = 6,          // Acknowledge
    FILE_DIRECTIVE_METADATA = 7,     // Metadata
    FILE_DIRECTIVE_NAK = 8,          // Negative Acknowledge
    FILE_DIRECTIVE_PROMPT = 9,       // Prompt
    FILE_DIRECTIVE_KEEP_ALIVE = 12,  // Keep Alive
    FILE_DIRECTIVE_INVALID_MAX = 13  // Maximum used to limit range
};

// CFDP Condition Codes
// Blue Book section 5.2.2, table 5-5
enum ConditionCode : U8 {
    CONDITION_CODE_NO_ERROR = 0,
    CONDITION_CODE_POS_ACK_LIMIT_REACHED = 1,
    CONDITION_CODE_KEEP_ALIVE_LIMIT_REACHED = 2,
    CONDITION_CODE_INVALID_TRANSMISSION_MODE = 3,
    CONDITION_CODE_FILESTORE_REJECTION = 4,
    CONDITION_CODE_FILE_CHECKSUM_FAILURE = 5,
    CONDITION_CODE_FILE_SIZE_ERROR = 6,
    CONDITION_CODE_NAK_LIMIT_REACHED = 7,
    CONDITION_CODE_INACTIVITY_DETECTED = 8,
    CONDITION_CODE_INVALID_FILE_STRUCTURE = 9,
    CONDITION_CODE_CHECK_LIMIT_REACHED = 10,
    CONDITION_CODE_UNSUPPORTED_CHECKSUM_TYPE = 11,
    CONDITION_CODE_SUSPEND_REQUEST_RECEIVED = 14,
    CONDITION_CODE_CANCEL_REQUEST_RECEIVED = 15
};

// CFDP ACK Transaction Status
// Blue Book section 5.2.4, table 5-8
enum AckTxnStatus : U8 {
    ACK_TXN_STATUS_UNDEFINED = 0,
    ACK_TXN_STATUS_ACTIVE = 1,
    ACK_TXN_STATUS_TERMINATED = 2,
    ACK_TXN_STATUS_UNRECOGNIZED = 3
};

// CFDP FIN Delivery Code
// Blue Book section 5.2.3, table 5-7
enum FinDeliveryCode : U8 {
    FIN_DELIVERY_CODE_COMPLETE = 0,    // Data complete
    FIN_DELIVERY_CODE_INCOMPLETE = 1   // Data incomplete
};

// CFDP FIN File Status
// Blue Book section 5.2.3, table 5-7
enum FinFileStatus : U8 {
    FIN_FILE_STATUS_DISCARDED = 0,            // File discarded deliberately
    FIN_FILE_STATUS_DISCARDED_FILESTORE = 1,  // File discarded due to filestore rejection
    FIN_FILE_STATUS_RETAINED = 2,             // File retained successfully
    FIN_FILE_STATUS_UNREPORTED = 3            // File status unreported
};

// CFDP Checksum Type
// Blue Book section 5.2.5, table 5-9
enum ChecksumType : U8 {
    CHECKSUM_TYPE_MODULAR = 0,        // Modular checksum
    CHECKSUM_TYPE_CRC_32 = 1,         // CRC-32 (not currently supported)
    CHECKSUM_TYPE_NULL_CHECKSUM = 15  // Null checksum
};

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

// CFDP TLV Types
// Blue Book section 5.4, table 5-3
enum TlvType : U8 {
    TLV_TYPE_FILESTORE_REQUEST = 0,       // Filestore request
    TLV_TYPE_FILESTORE_RESPONSE = 1,      // Filestore response
    TLV_TYPE_MESSAGE_TO_USER = 2,         // Message to user
    TLV_TYPE_FAULT_HANDLER_OVERRIDE = 4,  // Fault handler override
    TLV_TYPE_FLOW_LABEL = 5,              // Flow label
    TLV_TYPE_ENTITY_ID = 6                // Entity ID
};

//! TLV data storage
class TlvData {
  private:
    union {
        CfdpEntityId m_eid;      // Valid when type=ENTITY_ID
        U8 m_rawData[256];       // Valid for other types (max 255 bytes + null term)
    };
    U8 m_dataLength;             // Actual length of data

  public:
    TlvData();

    // Set entity ID (for TLV type 6)
    void setEntityId(CfdpEntityId eid);

    // Set raw data (for other TLV types)
    void setData(const U8* data, U8 length);

    // Get entity ID
    CfdpEntityId getEntityId() const;

    // Get raw data pointer
    const U8* getData() const;

    // Get data length
    U8 getLength() const;
};

//! Single TLV entry
class Tlv {
  private:
    TlvType m_type;
    TlvData m_data;

  public:
    Tlv();

    // Initialize with entity ID
    void initialize(CfdpEntityId eid);

    // Initialize with raw data
    void initialize(TlvType type, const U8* data, U8 length);

    // Getters
    TlvType getType() const;
    const TlvData& getData() const;

    // Compute encoded size
    U32 getEncodedSize() const;

    // Encode to SerialBuffer
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;

    // Decode from SerialBuffer
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);
};

//! List of TLVs
class TlvList {
  private:
    U8 m_numTlv;
    Tlv m_tlvs[CFDP_MAX_TLV];

  public:
    TlvList();

    // Add a TLV (returns false if list is full)
    bool appendTlv(const Tlv& tlv);

    // Clear all TLVs
    void clear();

    // Get number of TLVs
    U8 getNumTlv() const;

    // Get TLV at index
    const Tlv& getTlv(U8 index) const;

    // Compute total encoded size of all TLVs
    U32 getEncodedSize() const;

    // Encode all TLVs to SerialBuffer
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;

    // Decode all TLVs from SerialBuffer (reads until buffer exhausted)
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);
};

//! \class Pdu
//!
union Pdu {
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
        friend union Pdu;
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
        void initialize(Type type,
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

        //! Initialize this Header from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this Header to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;

        //! Get the PDU type
        Type getType() const { return this->m_type; }

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
    };

    //! The type of a Metadata PDU
    class MetadataPdu {
        friend union Pdu;

      private:
        //! The PDU header
        Header m_header;

        //! Closure requested flag
        U8 m_closureRequested;

        //! Checksum type
        ChecksumType m_checksumType;

        //! File size
        CfdpFileSize m_fileSize;

        //! Source filename
        Fw::String m_sourceFilename;

        //! Destination filename
        Fw::String m_destFilename;

      public:
        //! Constructor
        MetadataPdu() : m_sourceFilename(""), m_destFilename("") {}

        //! Initialize a Metadata PDU
        void initialize(Direction direction,
                       Cfdp::Class::T txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       CfdpFileSize fileSize,
                       const Fw::String& sourceFilename,
                       const Fw::String& destFilename,
                       ChecksumType checksumType,
                       U8 closureRequested);

        //! Compute the buffer size needed
        U32 getBufferSize() const;

        //! Convert this MetadataPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Initialize this MetadataPdu from a Buffer
        Fw::SerializeStatus fromBuffer(const Fw::Buffer& buffer);

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get the file size
        CfdpFileSize getFileSize() const { return this->m_fileSize; }

        //! Get the source filename
        const Fw::String& getSourceFilename() const { return this->m_sourceFilename; }

        //! Get the destination filename
        const Fw::String& getDestFilename() const { return this->m_destFilename; }

        //! Get checksum type
        ChecksumType getChecksumType() const { return this->m_checksumType; }

        //! Get closure requested flag
        U8 getClosureRequested() const { return this->m_closureRequested; }

        //! Get directive code
        FileDirective getDirectiveCode() const { return FILE_DIRECTIVE_METADATA; }

      private:
        //! Initialize this MetadataPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this MetadataPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of a File Data PDU
    class FileDataPdu {
        friend union Pdu;

      private:
        //! The PDU header
        Header m_header;

        //! File offset
        CfdpFileSize m_offset;

        //! Data size
        U16 m_dataSize;

        //! Pointer to file data
        const U8* m_data;

      public:
        //! Initialize a File Data PDU
        void initialize(Direction direction,
                       Cfdp::Class::T txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       CfdpFileSize offset,
                       U16 dataSize,
                       const U8* data);

        //! Compute the buffer size needed
        U32 getBufferSize() const;

        //! Convert this FileDataPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Initialize this FileDataPdu from a Buffer
        Fw::SerializeStatus fromBuffer(const Fw::Buffer& buffer);

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get the file offset
        CfdpFileSize getOffset() const { return this->m_offset; }

        //! Get the data size
        U16 getDataSize() const { return this->m_dataSize; }

        //! Get the data pointer
        const U8* getData() const { return this->m_data; }

        //! Calculate maximum file data payload size
        //! @return Maximum number of data bytes that can fit in a File Data PDU
        U32 getMaxFileDataSize();

      private:
        //! Initialize this FileDataPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this FileDataPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of an EOF PDU
    class EofPdu {
        friend union Pdu;

      private:
        //! The PDU header
        Header m_header;

        //! Condition code
        ConditionCode m_conditionCode;

        //! File checksum
        U32 m_checksum;

        //! File size
        CfdpFileSize m_fileSize;

        //! TLV list (optional)
        TlvList m_tlvList;

      public:
        //! Initialize an EOF PDU
        void initialize(Direction direction,
                       Cfdp::Class::T txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       ConditionCode conditionCode,
                       U32 checksum,
                       CfdpFileSize fileSize);

        //! Compute the buffer size needed
        U32 getBufferSize() const;

        //! Convert this EofPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Initialize this EofPdu from a Buffer
        Fw::SerializeStatus fromBuffer(const Fw::Buffer& buffer);

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get condition code
        ConditionCode getConditionCode() const { return this->m_conditionCode; }

        //! Get checksum
        U32 getChecksum() const { return this->m_checksum; }

        //! Get file size
        CfdpFileSize getFileSize() const { return this->m_fileSize; }

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
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this EofPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of a Finished PDU
    class FinPdu {
        friend union Pdu;

      private:
        //! The PDU header
        Header m_header;

        //! Condition code
        ConditionCode m_conditionCode;

        //! Delivery code
        FinDeliveryCode m_deliveryCode;

        //! File status
        FinFileStatus m_fileStatus;

        //! TLV list (optional)
        TlvList m_tlvList;

      public:
        //! Initialize a Finished PDU
        void initialize(Direction direction,
                       Cfdp::Class::T txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       ConditionCode conditionCode,
                       FinDeliveryCode deliveryCode,
                       FinFileStatus fileStatus);

        //! Compute the buffer size needed
        U32 getBufferSize() const;

        //! Convert this FinPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Initialize this FinPdu from a Buffer
        Fw::SerializeStatus fromBuffer(const Fw::Buffer& buffer);

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

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
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this FinPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! The type of an ACK PDU
    class AckPdu {
        friend union Pdu;

      private:
        //! The PDU header
        Header m_header;

        //! Directive being acknowledged
        FileDirective m_directiveCode;

        //! Directive subtype code
        U8 m_directiveSubtypeCode;

        //! Condition code
        ConditionCode m_conditionCode;

        //! Transaction status
        AckTxnStatus m_transactionStatus;

      public:
        //! Initialize an ACK PDU
        void initialize(Direction direction,
                       Cfdp::Class::T txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       FileDirective directiveCode,
                       U8 directiveSubtypeCode,
                       ConditionCode conditionCode,
                       AckTxnStatus transactionStatus);

        //! Compute the buffer size needed
        U32 getBufferSize() const;

        //! Convert this AckPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Initialize this AckPdu from a Buffer
        Fw::SerializeStatus fromBuffer(const Fw::Buffer& buffer);

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

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
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this AckPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

    //! Segment request structure for NAK PDU
    struct SegmentRequest {
        CfdpFileSize offsetStart;  //!< Start offset of missing data
        CfdpFileSize offsetEnd;    //!< End offset of missing data
    };

    //! The type of a NAK PDU
    class NakPdu {
        friend union Pdu;

      private:
        //! The PDU header
        Header m_header;

        //! Scope start offset
        CfdpFileSize m_scopeStart;

        //! Scope end offset
        CfdpFileSize m_scopeEnd;

        //! Number of segment requests
        U8 m_numSegments;

        //! Segment requests array (max CF_NAK_MAX_SEGMENTS = 58)
        SegmentRequest m_segments[58];

      public:
        //! Initialize a NAK PDU
        void initialize(Direction direction,
                       Cfdp::Class::T txmMode,
                       CfdpEntityId sourceEid,
                       CfdpTransactionSeq transactionSeq,
                       CfdpEntityId destEid,
                       CfdpFileSize scopeStart,
                       CfdpFileSize scopeEnd);

        //! Compute the buffer size needed
        U32 getBufferSize() const;

        //! Convert this NakPdu to a Buffer
        Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

        //! Initialize this NakPdu from a Buffer
        Fw::SerializeStatus fromBuffer(const Fw::Buffer& buffer);

        //! Get this as a Header
        const Header& asHeader() const { return this->m_header; }

        //! Get scope start
        CfdpFileSize getScopeStart() const { return this->m_scopeStart; }

        //! Get scope end
        CfdpFileSize getScopeEnd() const { return this->m_scopeEnd; }

        //! Get number of segments
        U8 getNumSegments() const { return this->m_numSegments; }

        //! Get segment at index (no bounds checking - caller must verify index < getNumSegments())
        const SegmentRequest& getSegment(U8 index) const { return this->m_segments[index]; }

        //! Add a segment request
        //! @return True if segment was added, false if segment array is full
        bool addSegment(CfdpFileSize offsetStart, CfdpFileSize offsetEnd);

        //! Clear all segment requests
        void clearSegments();

        //! Get directive code
        FileDirective getDirectiveCode() const { return FILE_DIRECTIVE_NAK; }

      private:
        //! Initialize this NakPdu from a SerialBuffer
        Fw::SerializeStatus fromSerialBuffer(Fw::SerialBuffer& serialBuffer);

        //! Write this NakPdu to a SerialBuffer
        Fw::SerializeStatus toSerialBuffer(Fw::SerialBuffer& serialBuffer) const;
    };

  public:
    // ----------------------------------------------------------------------
    // Constructor/Destructor
    // ----------------------------------------------------------------------

    Pdu() : m_metadataPdu() { this->m_header.m_type = T_NONE; }

    ~Pdu() {
        // Explicitly destroy the active union member
        // Since we always construct m_metadataPdu, always destroy it
        this->m_metadataPdu.~MetadataPdu();
    }

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

    //! Get the directive code for this PDU
    //! \return The directive code, or FILE_DIRECTIVE_INVALID_MAX for file data PDUs
    //!
    FileDirective getDirectiveCode() const;

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

    //! Get this as a MetadataPdu (mutable, sets type)
    //!
    MetadataPdu& asMetadataPdu();

    //! Get this as a FileDataPdu (mutable, sets type)
    //!
    FileDataPdu& asFileDataPdu();

    //! Get this as an EofPdu (mutable, sets type)
    //!
    EofPdu& asEofPdu();

    //! Get this as a FinPdu (mutable, sets type)
    //!
    FinPdu& asFinPdu();

    //! Get this as an AckPdu (mutable, sets type)
    //!
    AckPdu& asAckPdu();

    //! Get this as a NakPdu (mutable, sets type)
    //!
    NakPdu& asNakPdu();

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

    //! Get the buffer size needed to hold this Pdu
    //!
    U32 getBufferSize() const;

    //! Convert this Pdu to a Buffer
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

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_Pdu_HPP
