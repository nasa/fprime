// ======================================================================
// \title  FileDownlink.hpp
// \author bocchino, mstarch
// \brief  hpp file for FileDownlink component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_FileDownlink_HPP
#define Svc_FileDownlink_HPP

#include <FileDownlinkCfg.hpp>
#include <Svc/FileDownlink/FileDownlinkComponentAc.hpp>
#include <Fw/FilePacket/FilePacket.hpp>
#include <Os/File.hpp>
#include <Os/Mutex.hpp>
#include <Os/Queue.hpp>


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
          typedef enum { IDLE, DOWNLINK, CANCEL, WAIT, COOLDOWN } Type;

        public:

          //! Constructor
          Mode() : m_value(IDLE) { }

        public:

          //! Set the Mode value
          void set(const Type value) {
            this->m_mutex.lock();
            this->m_value = value;
            this->m_mutex.unLock();
          }

          //! Get the Mode value
          Type get() {
            this->m_mutex.lock();
            const Type value = this->m_value;
            this->m_mutex.unLock();
            return value;
          }

        private:

          //! The Mode value
          Type m_value;

          //! The Mode mutex
          Os::Mutex m_mutex;
      };

      //! Class representing an outgoing file
      class File {

        public:

          //! Constructor
          File() : m_size(0) { }

        PRIVATE:

          //! The source file name
          Fw::LogStringArg m_sourceName;

          //! The destination file name
          Fw::LogStringArg m_destName;

          //! The underlying OS file
          Os::File m_osFile;

          //! The file size
          U32 m_size;

          //! The checksum for the file
          CFDP::Checksum m_checksum;

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
          void getChecksum(CFDP::Checksum& checksum) {
            checksum = this->m_checksum;
          }

          //! Get the source file name
          Fw::LogStringArg& getSourceName(void) {
            return this->m_sourceName;
          }

          //! Get the destination file name
          Fw::LogStringArg& getDestName(void) {
            return this->m_destName;
          }

          //! Get the underlying OS file
          Os::File& getOsFile(void) {
            return this->m_osFile;
          }

          //! Get the file size
          U32 getSize(void) {
            return this->m_size;
          }
      };

      //! Class to record files sent
      class FilesSent {

        public:

          //! Construct a FilesSent object
          FilesSent(FileDownlink *const fileDownlink) :
            m_sent_file_count(0),
            m_fileDownlink(fileDownlink)
          { }

        public:

          //! Record a file sent
          void fileSent() {
            ++this->m_sent_file_count;
            this->m_fileDownlink->tlmWrite_FilesSent(m_sent_file_count);
          }

        PRIVATE:

          //! The total number of file sent
          U32 m_sent_file_count;

          //! The enclosing FileDownlink object
          FileDownlink *const m_fileDownlink;

      };

      //! Class to record packets sent
      class PacketsSent {

        public:

          //! Construct a PacketsSent object
          PacketsSent(FileDownlink *const fileDownlink) :
            m_sent_packet_count(0),
            m_fileDownlink(fileDownlink)
          { }

        public:

          //! Record a packet sent
          void packetSent() {
            ++this->m_sent_packet_count;
            this->m_fileDownlink->tlmWrite_PacketsSent(m_sent_packet_count);
          }

        PRIVATE:

          //! The total number of downlinked packets
          U32 m_sent_packet_count;

          //! The enclosing FileDownlink object
          FileDownlink *const m_fileDownlink;

      };

      //! Class to record warnings
      class Warnings {

        public:

          //! Construct a Warnings object
          Warnings(FileDownlink *const fileDownlink) :
            m_warning_count(0),
            m_fileDownlink(fileDownlink)
          { }

        public:

          //! Issue a File Open Error warning
          void fileOpenError();

          //! Issue a File Read Error warning
          void fileRead(const Os::File::Status status);

        PRIVATE:

          //! Record a warning
          void warning() {
            ++this->m_warning_count;
            this->m_fileDownlink->tlmWrite_Warnings(m_warning_count);
          }

        PRIVATE:

          //! The total number of warnings
          U32 m_warning_count;

          //! The enclosing FileDownlink object
          FileDownlink *const m_fileDownlink;

      };

      //! Sources of send file requests
      enum CallerSource { COMMAND, PORT };

      #define FILE_ENTRY_FILENAME_LEN 101

      //! Used to track a single file downlink request
      struct FileEntry {
        char srcFilename[FILE_ENTRY_FILENAME_LEN]; // Name of requested file
        char destFilename[FILE_ENTRY_FILENAME_LEN]; // Name of requested file
        U32 offset;
        U32 length;
        CallerSource source; // Source of the downlink request
        FwOpcodeType opCode; // Op code of command, only set for CMD sources.
        U32 cmdSeq; // CmdSeq number, only set for CMD sources.
        U32 context; // Context id of request, only set for PORT sources.
      };

      //! Enumeration for packet types
      //! Each type has a buffer to store it.
      enum PacketType {
          FILE_PACKET,
          CANCEL_PACKET,
          COUNT_PACKET_TYPE
      };

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object FileDownlink
      //!
      FileDownlink(
          const char *const compName //!< The component name
      );

      //! Initialize object FileDownlink
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance //!< The instance number
      );

      //! Configure FileDownlink component
      //!
      void configure(
          U32 timeout, //!< Timeout threshold (milliseconds) while in WAIT state
          U32 cooldown, //!< Cooldown (in ms) between finishing a downlink and starting the next file.
          U32 cycleTime, //!< Rate at which we are running
          U32 fileQueueDepth //!< Max number of items in file downlink queue
      );

      //! Start FileDownlink component
      //! The component must be configured with configure() before starting.
      //!
      void preamble();

      //! Destroy object FileDownlink
      //!
      ~FileDownlink();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for Run
      //!
      void Run_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          NATIVE_UINT_TYPE context //!< The call order
      );


      //! Handler implementation for SendFile
      //!
      Svc::SendFileResponse SendFile_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          const sourceFileNameString& sourceFilename, /*!< Path of file to downlink*/
          const destFileNameString& destFilename, /*!< Path to store downlinked file at*/
          U32 offset, /*!< Amount of data in bytes to downlink from file. 0 to read until end of file*/
          U32 length /*!< Amount of data in bytes to downlink from file. 0 to read until end of file*/
      );

      //! Handler implementation for bufferReturn
      //!
      void bufferReturn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer &fwBuffer
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );



    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for FileDownlink_SendFile command handler
      //!
      void SendFile_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& sourceFilename, //!< The name of the on-board file to send
          const Fw::CmdStringArg& destFilename //!< The name of the destination file on the ground
      );

      //! Implementation for FileDownlink_Cancel command handler
      //!
      void Cancel_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq //!< The command sequence number
      );

      //! Implementation for FILE_DWN_SEND_PARTIAL command handler
      //!
      void SendPartial_cmdHandler(
          FwOpcodeType opCode, //!< The opcode
          U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& sourceFilename, //!< The name of the on-board file to send
          const Fw::CmdStringArg& destFilename, //!< The name of the destination file on the ground
          U32 startOffset, //!< Starting offset of the source file
          U32 length //!< Number of bytes to send from starting offset. Length of 0 implies until the end of the file
      );


    PRIVATE:

      // ----------------------------------------------------------------------
      // Private helper methods
      // ----------------------------------------------------------------------


      void sendFile(
          const char* sourceFilename, //!< The name of the on-board file to send
          const char* destFilename, //!< The name of the destination file on the ground
          U32 startOffset, //!< Starting offset of the source file
          U32 length //!< Number of bytes to send from starting offset. Length of 0 implies until the end of the file
      );

      //Individual packet transfer functions
      Os::File::Status sendDataPacket(U32 &byteOffset);
      void sendCancelPacket();
      void sendEndPacket();
      void sendStartPacket();
      void sendFilePacket(const Fw::FilePacket& filePacket);

      //State-helper functions
      void exitFileTransfer();
      void enterCooldown();

      //Function to acquire a buffer internally
      void getBuffer(Fw::Buffer& buffer, PacketType type);
      //Downlink the "next" packet
      void downlinkPacket();
      //Finish the file transfer
      void finishHelper(bool is_cancel);
      // Convert internal status enum to a command response;
      Fw::CmdResponse statusToCmdResp(SendFileStatus status);
      //Send response after completing file downlink
      void sendResponse(SendFileStatus resp);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Member variables
      // ----------------------------------------------------------------------

      //! Whether the configuration function has been called.
      bool m_configured;

      //! File downlink queue
      Os::Queue m_fileQueue;

      //!Buffer's memory backing
      U8 m_memoryStore[COUNT_PACKET_TYPE][FILEDOWNLINK_INTERNAL_BUFFER_SIZE];

      //! The mode
      Mode m_mode;

      //! The file
      File m_file;

      //! Files sent
      FilesSent m_filesSent;

      //! Packets sent
      PacketsSent m_packetsSent;

      //! Warnings
      Warnings m_warnings;

      //! The current sequence index
      U32 m_sequenceIndex;

      //! Timeout threshold (milliseconds) while in WAIT state
      U32 m_timeout;

      //! Cooldown (in ms) between finishing a downlink and starting the next file.
      U32 m_cooldown;

      //! current time residing in WAIT state
      U32 m_curTimer;

      //! rate (milliseconds) at which we are running
      U32 m_cycleTime;

      ////! Buffer for sending file data
      Fw::Buffer m_buffer;

      //! Buffer size for file data
      U32 m_bufferSize;

      //! Current byte offset in file
      U32 m_byteOffset;

      //! Amount of bytes left to read
      U32 m_endOffset;

      //! Set to true when all data packets have been sent
      Fw::FilePacket::Type m_lastCompletedType;

      //! Last buffer used
      U32 m_lastBufferId;

      //! Current in progress file entry from queue
      struct FileEntry m_curEntry;

      //! Incrementing context id used to unique identify a specific downlink request
      U32 m_cntxId;
    };

} // end namespace Svc

#endif
