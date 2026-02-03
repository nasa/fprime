// ======================================================================
// \title  MetadataPdu.hpp
// \author campuzan
// \brief  hpp file for CFDP Metadata PDU
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_MetadataPdu_HPP
#define Svc_Ccsds_Cfdp_MetadataPdu_HPP

#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>
#include <Svc/Ccsds/CfdpManager/Types/Types.hpp>
#include <Fw/Types/String.hpp>
#include <config/CfdpCfg.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

//! The type of a Metadata PDU
class MetadataPdu : public PduBase {
  private:
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
    U32 getBufferSize() const override;

    //! Fw::Serializable interface - serialize to buffer
    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer,
                                    Fw::Endianness mode = Fw::Endianness::BIG) const override;

    //! Fw::Serializable interface - deserialize from buffer
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer,
                                        Fw::Endianness mode = Fw::Endianness::BIG) override;

    //! Get this as a Header
    const PduHeader& asHeader() const { return this->m_header; }

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
    //! Initialize this MetadataPdu from a SerialBufferBase
    Fw::SerializeStatus fromSerialBuffer(Fw::SerialBufferBase& serialBuffer);

    //! Write this MetadataPdu to a SerialBufferBase
    Fw::SerializeStatus toSerialBuffer(Fw::SerialBufferBase& serialBuffer) const;
};

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_MetadataPdu_HPP
