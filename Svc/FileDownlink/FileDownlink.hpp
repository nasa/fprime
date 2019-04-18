// ====================================================================== 
// \title  FileDownlink.hpp
// \author bocchino
// \brief  hpp file for FileDownlink component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// 
// ====================================================================== 

#ifndef Svc_FileDownlink_HPP
#define Svc_FileDownlink_HPP

#include <Svc/FileDownlink/FileDownlinkComponentAc.hpp>
#include <Fw/FilePacket/FilePacket.hpp>
#include <Os/File.hpp>
#include <Os/Mutex.hpp>

namespace Svc {

  class FileDownlink :
    public FileDownlinkComponentBase
  {

    PRIVATE:

      // ----------------------------------------------------------------------
      // Types 
      // ----------------------------------------------------------------------

      //! The Mode class
      class Mode {

        public:

          //! The Mode type
          typedef enum { IDLE, DOWNLINK, CANCEL } Type;

        public:

          //! Constructor
          Mode(void) : value(IDLE) { }

        public:

          //! Set the Mode value
          void set(const Type value) { 
            this->mutex.lock();
            this->value = value; 
            this->mutex.unLock();
          }

          //! Get the Mode value
          Type get(void) { 
            this->mutex.lock();
            const Type value = this->value; 
            this->mutex.unLock();
            return value;
          }

        private:

          //! The Mode value
          Type value;

          //! The Mode mutex
          Os::Mutex mutex;

      };

      //! Class representing an outgoing file
      class File {

        public:

          //! Constructor
          File() : size(0) { }

        public:
          
          //! The source file name
          Fw::LogStringArg sourceName;

          //! The destination file name
          Fw::LogStringArg destName;

          //! The underlying OS file
          Os::File osFile;
          
          //! The file size
          U32 size;

        PRIVATE:

          //! The checksum for the file
          ::CFDP::Checksum checksum;

        public:

          //! Open the OS file for reading and initialize the checksum
          Os::File::Status open(
              const char *const sourceFileName, //!< The source file name
              const char *const destFileName //!< The destination file name
          );

          //! Read bytes from the OS file and update the checksum
          Os::File::Status read(
              U8 *const data,
              const U32 byteOffset,
              const U32 size
          );

          //! Get the checksum
          void getChecksum(::CFDP::Checksum& checksum) {
            checksum = this->checksum;
          }

      };

      //! Class to record files sent
      class FilesSent {

        public:

          //! Construct a FilesSent object
          FilesSent(FileDownlink *const fileDownlink) : 
            n(0), fileDownlink(fileDownlink) 
          { }

        public:

          //! Record a file sent
          void fileSent(void) {
            ++this->n;
            this->fileDownlink->tlmWrite_FileDownlink_FilesSent(n);
          }

        PRIVATE:

          //! The total number of downlinks canceled
          U32 n;

          //! The enclosing FileDownlink object
          FileDownlink *const fileDownlink;

      };

      //! Class to record packets sent
      class PacketsSent {

        public:

          //! Construct a PacketsSent object
          PacketsSent(FileDownlink *const fileDownlink) : 
            n(0), fileDownlink(fileDownlink) 
          { }

        public:

          //! Record a packet sent
          void packetSent(void) {
            ++this->n;
            this->fileDownlink->tlmWrite_FileDownlink_PacketsSent(n);
          }

        PRIVATE:

          //! The total number of downlinks canceled
          U32 n;

          //! The enclosing FileDownlink object
          FileDownlink *const fileDownlink;

      };

      //! Class to record warnings
      class Warnings {

        public:

          //! Construct an Warnings object
          Warnings(FileDownlink *const fileDownlink) : 
            n(0), fileDownlink(fileDownlink)
          { }

        public:

          //! Issue a File Open Error warning
          void fileOpenError(void);

          //! Issue a File Read Error warning
          void fileRead(void);

        PRIVATE:

          //! Record a warning
          void warning(void) {
            ++this->n;
            this->fileDownlink->tlmWrite_FileDownlink_Warnings(n);
          }

        PRIVATE:

          //! The total number of warnings
          U32 n;

          //! The enclosing FileDownlink object
          FileDownlink *const fileDownlink;

      };

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object FileDownlink
      //!
      FileDownlink(
          const char *const compName, //!< The component name
          const U16 downlinkPacketSize //!< The size of a downlink packet
      );

      //! Initialize object FileDownlink
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance //!< The instance number
      );

      //! Destroy object FileDownlink
      //!
      ~FileDownlink(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations 
      // ----------------------------------------------------------------------

      //! Implementation for FileDownlink_SendFile command handler
      //!
      void FileDownlink_SendFile_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& sourceFileName, //!< The name of the on-board file to send
          const Fw::CmdStringArg& destFileName //!< The name of the destination file on the ground
      );

      //! Implementation for FileDownlink_Cancel command handler
      //!
      void FileDownlink_Cancel_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq //!< The command sequence number
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );


    PRIVATE:

      // ----------------------------------------------------------------------
      // Private helper methods 
      // ----------------------------------------------------------------------

      Os::File::Status sendDataPacket(const U32 byteOffset);

      Os::File::Status sendDataPackets(void);

      void sendCancelPacket(void);

      void sendEndPacket(void);

      void sendStartPacket(void);

      void sendFilePacket(const Fw::FilePacket& filePacket);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Member variables 
      // ----------------------------------------------------------------------

      //! The size of a downlink packet
      const U32 downlinkPacketSize;

      //! The mode
      Mode mode;

      //! The file
      File file;

      //! Files sent
      FilesSent filesSent;

      //! Packets sent
      PacketsSent packetsSent;

      //! Warnings
      Warnings warnings;

      //! The current sequence index
      U32 sequenceIndex;

    };

} // end namespace Svc

#endif
