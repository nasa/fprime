// ====================================================================== 
// \title  FileUplink.hpp
// \author bocchino
// \brief  hpp file for FileUplink component implementation class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef Svc_FileUplink_HPP
#define Svc_FileUplink_HPP

#include <Svc/FileUplink/FileUplinkComponentAc.hpp>
#include <Fw/FilePacket/FilePacket.hpp>
#include <Os/File.hpp>

namespace Svc {

  class FileUplink :
    public FileUplinkComponentBase
  {

    PRIVATE:

      // ----------------------------------------------------------------------
      // Types 
      // ----------------------------------------------------------------------

      //! The ReceiveMode type
      typedef enum { START, DATA } ReceiveMode;

      //! An object representing an incoming file
      class File {

        public:
          
          //! The file size
          U32 size;
         
          //! The file name
          Fw::LogStringArg name;

          //! The underlying OS file
          Os::File osFile;

        PRIVATE:

          //! The checksum for the file
          ::CFDP::Checksum checksum;

        public:

          //! Open the OS file for writing and initialize the checksum
          Os::File::Status open(
              const Fw::FilePacket::StartPacket& startPacket
          );

          //! Write bytes into the OS file and update the checksum
          Os::File::Status write(
              const U8 *const data,
              const U32 byteOffset,
              const U32 length
          );

          //! Get the checksum
          void getChecksum(::CFDP::Checksum& checksum) {
            checksum = this->checksum;
          }

      };

      //! Object to record files received
      class FilesReceived {

        public:

          //! Construct a FilesReceived object
          FilesReceived(FileUplink *const fileUplink) : 
            n(0), fileUplink(fileUplink) 
          { }

        public:

          //! Record a received file
          void fileReceived(void) {
            ++this->n;
            this->fileUplink->tlmWrite_FileUplink_FilesReceived(n);
          }

        PRIVATE:

          //! The total number of files received
          U32 n;

          //! The enclosing FileUplink object
          FileUplink *const fileUplink;

      };

      //! Object to record packets received
      class PacketsReceived {

        public:

          //! Construct a PacketsReceived object
          PacketsReceived(FileUplink *const fileUplink) : 
            n(0), fileUplink(fileUplink)
          { }

        public:

          //! Record a packet received
          void packetReceived(void) {
            ++this->n;
            this->fileUplink->tlmWrite_FileUplink_PacketsReceived(n);
          }

        PRIVATE:

          //! The total number of cancel packets
          U32 n;

          //! The enclosing FileUplink object
          FileUplink *const fileUplink;

      };

      //! Object to record warnings
      class Warnings {

        public:

          //! Construct an Warnings object
          Warnings(FileUplink *const fileUplink) : 
            n(0), fileUplink(fileUplink)
          { }

        public:

          //! Record an Invalid Receive Mode warning
          void invalidReceiveMode(const Fw::FilePacket::Type packetType);

          //! Record a File Open warning
          void fileOpen(Fw::LogStringArg& fileName);

          //! Record a Packet Out of Bounds warning
          void packetOutOfBounds(
              const U32 sequenceIndex,
              Fw::LogStringArg& fileName
          );

          //! Record a Packet Out of Order warning
          void packetOutOfOrder(
              const U32 sequenceIndex,
              const U32 lastSequenceIndex
          );

          //! Record a File Write warning
          void fileWrite(Fw::LogStringArg& fileName);

          //! Record a Bad Checksum warning
          void badChecksum(
              const U32 computed,
              const U32 read
          );

        PRIVATE:

          //! Record a warning
          void warning(void) {
            ++this->n;
            this->fileUplink->tlmWrite_FileUplink_Warnings(n);
          }

        PRIVATE:

          //! The total number of warnings
          U32 n;

          //! The enclosing FileUplink object
          FileUplink *const fileUplink;

      };

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object FileUplink
      //!
      FileUplink(
          const char *const name //!< The component name
      );

      //! Initialize object FileUplink
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance //!< The instance number
      );

      //! Destroy object FileUplink
      //!
      ~FileUplink(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for bufferSendIn
      //!
      void bufferSendIn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer& buffer
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );


    PRIVATE:

      // ----------------------------------------------------------------------
      // Private helper functions 
      // ----------------------------------------------------------------------

      //! Handle a start packet
      void handleStartPacket(const Fw::FilePacket::StartPacket& startPacket);

      //! Handle a data packet
      void handleDataPacket(const Fw::FilePacket::DataPacket& dataPacket);

      //! Handle an end packet
      void handleEndPacket(const Fw::FilePacket::EndPacket& endPacket);

      //! Handle a cancel packet
      void handleCancelPacket(void);

      //! Check sequence index
      void checkSequenceIndex(const U32 sequenceIndex);

      //! Compare checksums
      void compareChecksums(const Fw::FilePacket::EndPacket& endPacket);

      //! Go to START mode
      void goToStartMode(void);

      //! Go to DATA mode
      void goToDataMode(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Member variables 
      // ----------------------------------------------------------------------

      //! The receive mode
      ReceiveMode receiveMode;

      //! The sequence index of the last packet received
      U32 lastSequenceIndex;

      //! The file being assembled
      File file;

      //! The total number of files received
      FilesReceived filesReceived;

      //! The total number of cancel packets
      PacketsReceived packetsReceived;

      //! The total number of warnings
      Warnings warnings;

  };

}

#endif
