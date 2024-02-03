// ======================================================================
// \title  FilePacket.hpp
// \author bocchino
// \brief  hpp file for FilePacket
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Fw_FilePacket_HPP
#define Fw_FilePacket_HPP

#include <CFDP/Checksum/Checksum.hpp>
#include <Fw/Buffer/Buffer.hpp>
#include <FpConfig.hpp>
#include <Fw/Types/SerialBuffer.hpp>
#include <Fw/Types/Serializable.hpp>

namespace Fw {

  //! \class FilePacket
  //! \brief A file packet
  //!
  union FilePacket {

    public:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

      //! Packet type
      typedef enum {
        T_START = 0,
        T_DATA = 1,
        T_END = 2,
        T_CANCEL = 3,
        T_NONE = 255
      } Type;

      //! The type of a path name
      class PathName {

          friend union FilePacket;

        public:

          //! The maximum length of a path name
          enum { MAX_LENGTH = 255 };

        PRIVATE:

          //! The length
          U8 m_length;

          //! Pointer to the path value
          const char *m_value;

        public:

          //! Initialize a PathName
          void initialize(
              const char *const value //! The path value
          );

          //! Compute the buffer size needed to hold this PathName
          U32 bufferSize() const;

          //! Get the length of the path name value
          U32 getLength(void) const {
              return this->m_length;
          };

          //! Get the path name value
          const char* getValue(void) const {
              return this->m_value;
          };

        PRIVATE:

          //! Initialize this PathName from a SerialBuffer
          SerializeStatus fromSerialBuffer(SerialBuffer& serialBuffer);

          //! Write this PathName to a SerialBuffer
          SerializeStatus toSerialBuffer(SerialBuffer& serialBuffer) const;

      };

      //! The type of a packet header
      class Header {

          friend union FilePacket;

        PRIVATE:

          //! The packet type
          Type m_type;

          //! The sequence index
          U32 m_sequenceIndex;

        public:

          //! Header size
          enum { HEADERSIZE = sizeof(U8) + sizeof(U32) };

        PRIVATE:

          //! Initialize a file packet header
          void initialize(
              const Type type, //!< The packet type
              const U32 sequenceIndex //!< The sequence index
          );

          //! Compute the buffer size needed to hold this Header
          U32 bufferSize() const;

          //! Initialize this Header from a SerialBuffer
          SerializeStatus fromSerialBuffer(SerialBuffer& serialBuffer);

          //! Write this Header to a SerialBuffer
          SerializeStatus toSerialBuffer(SerialBuffer& serialBuffer) const;

        public:
          Type getType(void) const {
              return this->m_type;
          };

          U32 getSequenceIndex(void) const {
              return this->m_sequenceIndex;
          };

      };

      //! The type of a start packet
      struct StartPacket {

          friend union FilePacket;

        PRIVATE:

          //! The packet header
          Header m_header;

          //! The file size
          U32 m_fileSize;

          //! The source path
          PathName m_sourcePath;

          //! The destination path
          PathName m_destinationPath;

        public:

          //! Initialize a StartPacket with sequence number 0
          void initialize(
              const U32 fileSize, //!< The file size
              const char *const sourcePath, //!< The source path
              const char *const destinationPath //!< The destination path
          );

          //! Compute the buffer size needed to hold this StartPacket
          U32 bufferSize() const;

          //! Convert this StartPacket to a Buffer
          SerializeStatus toBuffer(Buffer& buffer) const;

          //! Get the destination path
          const PathName& getDestinationPath() const {
              return this->m_destinationPath;
          };

          //! Get the source path
          const PathName& getSourcePath() const {
              return this->m_sourcePath;
          };

          //! Get the file size
          U32 getFileSize() const {
              return this->m_fileSize;
          };
        PRIVATE:

          //! Initialize this StartPacket from a SerialBuffer
          SerializeStatus fromSerialBuffer(SerialBuffer& serialBuffer);

          //! Write this StartPacket to a SerialBuffer
          SerializeStatus toSerialBuffer(SerialBuffer& serialBuffer) const;

      };

      //! The type of a data packet
      class DataPacket {

          friend union FilePacket;

        PRIVATE:

          //! The packet header
          Header m_header;

          //! The byte offset of the packet data into the destination file
          U32 m_byteOffset;

          //! The size of the file data in the packet
          U16 m_dataSize;

          //! Pointer to the file data
          const U8 *m_data;

        public:

          //! header size
          enum { HEADERSIZE = Header::HEADERSIZE +
              sizeof(U32) +
              sizeof(U16) };

          //! Initialize a data packet
          void initialize(
              const U32 sequenceIndex, //!< The sequence index
              const U32 byteOffset, //!< The byte offset
              const U16 dataSize, //!< The data size
              const U8 *const data //!< The file data
          );

          //! Compute the buffer size needed to hold this DataPacket
          U32 bufferSize() const;

          //! Convert this DataPacket to a Buffer
          SerializeStatus toBuffer(Buffer& buffer) const;

          //! Get this as a Header
          const FilePacket::Header& asHeader() const {
              return this->m_header;
          };

          //! Get the byte offset
          U32 getByteOffset() const {
              return this->m_byteOffset;
          };

          //! Get the data size
          U32 getDataSize() const {
              return this->m_dataSize;
          };

          //! Get the data
          const U8* getData() const {
              return this->m_data;
          };
        PRIVATE:

          //! Initialize this DataPacket from a SerialBuffer
          SerializeStatus fromSerialBuffer(SerialBuffer& serialBuffer);

          //! Compute the fixed-length data size of a StartPacket
          U32 fixedLengthSize() const;

          //! Write this DataPacket to a SerialBuffer
          SerializeStatus toSerialBuffer(SerialBuffer& serialBuffer) const;

      };

      //! The type of an end packet
      class EndPacket {

          friend union FilePacket;

        PRIVATE:

          //! The packet header
          Header m_header;

        public:

          //! Set the checksum
          void setChecksum(const CFDP::Checksum& checksum);

          //! Get the checksum
          void getChecksum(CFDP::Checksum& checksum) const;

          //! Compute the buffer size needed to hold this EndPacket
          U32 bufferSize() const;

          //! Convert this EndPacket to a Buffer
          SerializeStatus toBuffer(Buffer& buffer) const;

          //! Get this as a Header
          const FilePacket::Header& asHeader() const {
              return this->m_header;
          };
        public:

          //! Initialize an end packet
          void initialize(
              const U32 sequenceIndex, //!< The sequence index
              const CFDP::Checksum& checksum //!< The checksum
          );

        PRIVATE:

          //! The checksum
          U32 m_checksumValue;

          //! Initialize this EndPacket from a SerialBuffer
          SerializeStatus fromSerialBuffer(SerialBuffer& serialBuffer);

          //! Write this EndPacket to a SerialBuffer
          SerializeStatus toSerialBuffer(SerialBuffer& serialBuffer) const;

      };

      //! The type of a cancel packet
      class CancelPacket {

          friend union FilePacket;

        PRIVATE:

          //! The packet header
          Header m_header;

        public:

          //! Initialize a cancel packet
          void initialize(
              const U32 sequenceIndex //!< The sequence index
          );

          //! Compute the buffer size needed to hold this CancelPacket
          U32 bufferSize() const;

          //! Convert this CancelPacket to a Buffer
          SerializeStatus toBuffer(Buffer& buffer) const;

          //! Get this as a Header
          const FilePacket::Header& asHeader() const {
              return this->m_header;
          };
        PRIVATE:

          //! Initialize this CancelPacket from a SerialBuffer
          SerializeStatus fromSerialBuffer(SerialBuffer& serialBuffer);

      };

    public:

      // ----------------------------------------------------------------------
      // Constructor
      // ----------------------------------------------------------------------

      FilePacket() { this->m_header.m_type = T_NONE; }

    public:

      // ----------------------------------------------------------------------
      // Public instance methods
      // ----------------------------------------------------------------------

      //! Initialize this from a Buffer
      //!
      SerializeStatus fromBuffer(const Buffer& buffer);

      //! Get this as a Header
      //!
      const Header& asHeader() const;

      //! Get this as a StartPacket
      //!
      const StartPacket& asStartPacket() const;

      //! Get this as a DataPacket
      //!
      const DataPacket& asDataPacket() const;

      //! Get this as an EndPacket
      //!
      const EndPacket& asEndPacket() const;

      //! Get this as a CancelPacket
      //!
      const CancelPacket& asCancelPacket() const;

      //! Initialize this with a StartPacket
      //!
      void fromStartPacket(const StartPacket& startPacket);

      //! Initialize this with a DataPacket
      //!
      void fromDataPacket(const DataPacket& dataPacket);

      //! Initialize this with an EndPacket
      //!
      void fromEndPacket(const EndPacket& endPacket);

      //! Initialize this with a CancelPacket
      //!
      void fromCancelPacket(const CancelPacket& cancelPacket);

      //! Get the buffer size needed to hold this FilePacket
      //!
      U32 bufferSize() const;

      //! Convert this FilePacket to a Buffer
      //!
      SerializeStatus toBuffer(Buffer& buffer) const;

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private methods
      // ----------------------------------------------------------------------

      //! Initialize this from a SerialBuffer
      //!
      SerializeStatus fromSerialBuffer(SerialBuffer& serialBuffer);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Private data
      // ----------------------------------------------------------------------

      //! this, seen as a header
      //!
      Header m_header;

      //! this, seen as a Start packet
      //!
      StartPacket m_startPacket;

      //! this, seen as a Data packet
      //!
      DataPacket m_dataPacket;

      //! this, seen as an End packet
      //!
      EndPacket m_endPacket;

      //! this, seen as a Cancel packet
      //!
      CancelPacket m_cancelPacket;

  };

}

#endif
