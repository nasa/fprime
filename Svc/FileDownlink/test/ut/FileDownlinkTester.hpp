// ======================================================================
// \title  FileDownlink/test/ut/Tester.hpp
// \author bocchino
// \brief  hpp file for FileDownlink test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include <Svc/FileDownlink/FileDownlink.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Test/UnitTest.hpp>
#include "FileDownlinkGTestBase.hpp"

#define MAX_HISTORY_SIZE 10
#define FILE_BUFFER_CAPACITY 100

namespace Svc {

  class FileDownlinkTester :
    public FileDownlinkGTestBase
  {

    private:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

      // A buffer for assembling files
      class FileBuffer {

        public:

          //! Construct a FileBuffer from raw data
          FileBuffer(
              const U8 *const data,
              const size_t size
          );

          //! Construct a FileBuffer from a history of data packets
          FileBuffer(
              const History<Fw::FilePacket::DataPacket>& dataPackets
          );

        public:

          //! Write the buffer to file
          void write(const char *const fileName);

          //! Get the checksum for the file
          void getChecksum(CFDP::Checksum& checksum);

        public:

          //! Compare two file buffers
          static bool compare(
              const FileBuffer& fb1,
              const FileBuffer& fb2
          );

        private:

          //! Push data onto the buffer
          void push(
              const U8 *const data,
              const size_t size
          );

        private:

          //! The data
          U8 data[FILE_BUFFER_CAPACITY];

          //! The index into the buffer
          size_t index;

      };

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object FileDownlinkTester
      //!
      FileDownlinkTester();

      //! Destroy object FileDownlinkTester
      //!
      ~FileDownlinkTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Create a file F
      //! Downlink F
      //! Verify that the downlinked file matches F
      //!
      void downlink();

      //! Cause a file open error
      //!
      void fileOpenError();

      //! Start and then cancel a downlink
      //!
      void cancelDownlink();

      //! Send a cancel command in idle mode
      //!
      void cancelInIdleMode();

      //! Create a file F
      //! Downlink partial F
      //! Verify that the downlinked file matches F
      //!
      void downlinkPartial();

      //! Timeout
      //!
      void timeout();

      //! sendFilePort
      //! Test downlinking a file via a port
      //!
      void sendFilePort();

    private:

      // ----------------------------------------------------------------------
      // Handlers for from ports
      // ----------------------------------------------------------------------

      //! Handler for from_bufferSendOut
      //!
      void from_bufferSendOut_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer& buffer
      );

      //! Handler for from_bufferSendOut
      //!
      void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum,
          U32 key
      );

      //! Handler for from_FileComplete
      //!
      void from_FileComplete_handler(
          const NATIVE_INT_TYPE portNum,
          const Svc::SendFileResponse& resp
      );

    private:

      // ----------------------------------------------------------------------
      // Private instance methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

      //! Command the FileDownlink component to send a file
      //! Assert a command response
      //!
      void sendFile(
          const char *const sourceFileName, //!< The source file name
          const char *const destFileName, //!< The destination file name
          const Fw::CmdResponse response //!< The expected command response
      );

      //! Command the FileDownlink component to send a file
      //! Assert a command response
      //!
      void sendFilePartial(
          const char *const sourceFileName, //!< The source file name
          const char *const destFileName, //!< The destination file name
          const Fw::CmdResponse response, //!< The expected command response
          U32 startIndex, //!< The starting index
          U32 length //!< The amount of bytes to downlink
      );

      //! Command the FileDownlink component to cancel a file downlink
      //! Assert a command response
      //!
      void cancel(
          const Fw::CmdResponse response //!< The expected command response
      );

      //! Remove a file
      //!
      void removeFile(
          const char *const name //!< The file name
      );

      // ----------------------------------------------------------------------
      // Private static methods
      // ----------------------------------------------------------------------

      //! Validate a packet history and accumulate the data packets
      //!
      static void validatePacketHistory(
        const History<FromPortEntry_bufferSendOut>& historyIn, //!< The incoming history
        History<Fw::FilePacket::DataPacket>& historyOut, //!< The outgoing history
        const Fw::FilePacket::Type endPacketType, //!< The expected ending packet type
        const size_t numPackets, //!< The expected number of packets
        const CFDP::Checksum& checksum, //!< The expected checksum,
        U32 startOffset //!< Starting byte offset
      );

      //! Validate a file packet buffer and convert it to a file packet
      //!
      static void validateFilePacket(
          const Fw::Buffer& buffer, //!< The buffer
          Fw::FilePacket& filePacket //!< The buffer as a FilePacket
      );

      //! Validate a start packet buffer
      //!
      static void validateStartPacket(
          const Fw::Buffer& buffer //!< The buffer
      );

      //! Validate a data packet buffer, convert it to a data packet,
      //! and update the byte offset
      //!
      static void validateDataPacket(
          const Fw::Buffer& buffer, //!< The buffer
          Fw::FilePacket::DataPacket& dataPacket, //!< The buffer as a data packet
          const U32 sequenceIndex, //!< The expected sequence index
          U32& byteOffset //!< The expected byte offset
      );

      //! Validate an end data packet buffer
      //!
      static void validateEndPacket(
          const Fw::Buffer& buffer, //!< The buffer
          const U32 sequenceIndex, //!< The expected sequence index
          const CFDP::Checksum& checksum //!< The expected checksum
      );

      //! Validate a cancel packet buffer
      //!
      static void validateCancelPacket(
          const Fw::Buffer& buffer, //!< The buffer
          const U32 sequenceIndex //!< The expected sequence index
      );

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      FileDownlink component;

      // Allocated buffers storage
      U8* buffers[1000];


      //! Buffers index
      //!
      U32 buffers_index;

      //! The current sequence index
      //!
      U32 sequenceIndex;
  };

} // end namespace Svc

#endif
