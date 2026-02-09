// ======================================================================
// \title  FileDataPdu.hpp
// \author campuzan
// \brief  hpp file for CFDP File Data PDU
// ======================================================================

#ifndef Svc_Ccsds_Cfdp_FileDataPdu_HPP
#define Svc_Ccsds_Cfdp_FileDataPdu_HPP

#include <Svc/Ccsds/CfdpManager/Types/PduBase.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <config/FileSizeAliasAc.hpp>

namespace Svc {
namespace Ccsds {
namespace Cfdp {

//! The type of a File Data PDU
class FileDataPdu : public PduBase {
  private:
    //! File offset
    FileSize m_offset;

    //! Data size
    U16 m_dataSize;

    //! Pointer to file data
    const U8* m_data;

  public:
    //! Constructor
    FileDataPdu() : m_offset(0), m_dataSize(0), m_data(nullptr) {}

    //! Initialize a File Data PDU
    void initialize(PduDirection direction,
                   Cfdp::Class::T txmMode,
                   EntityId sourceEid,
                   TransactionSeq transactionSeq,
                   EntityId destEid,
                   FileSize offset,
                   U16 dataSize,
                   const U8* data);

    //! Compute the buffer size needed
    U32 getBufferSize() const override;

    //! Convert this FileDataPdu to a Buffer
    Fw::SerializeStatus toBuffer(Fw::Buffer& buffer) const;

    //! Initialize this FileDataPdu from a Buffer
    Fw::SerializeStatus fromBuffer(const Fw::Buffer& buffer);

    //! Fw::Serializable interface - serialize to buffer
    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer,
                                    Fw::Endianness mode = Fw::Endianness::BIG) const override;

    //! Fw::Serializable interface - deserialize from buffer
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer,
                                        Fw::Endianness mode = Fw::Endianness::BIG) override;

    //! Get this as a Header
    const PduHeader& asHeader() const { return this->m_header; }

    //! Get the file offset
    FileSize getOffset() const { return this->m_offset; }

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

}  // namespace Cfdp
}  // namespace Ccsds
}  // namespace Svc

#endif  // Svc_Ccsds_Cfdp_FileDataPdu_HPP
