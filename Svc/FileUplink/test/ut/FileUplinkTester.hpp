// ======================================================================
// \title  FileUplink/test/ut/Tester.hpp
// \author bocchino
// \brief  hpp file for FileUplink test harness implementation class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include <Svc/FileUplink/FileUplink.hpp>
#include "FileUplinkGTestBase.hpp"

#define PACKET_SIZE 5

namespace Svc {

  class FileUplinkTester :
    public FileUplinkGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object FileUplinkTester
      //!
      FileUplinkTester();

      //! Destroy object FileUplinkTester
      //!
      ~FileUplinkTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Send a file
      //!
      void sendFile();

      //! Send a file with a bad checksum value
      //!
      void badChecksum();

      //! Cause a file open error
      //!
      void fileOpenError();

      //! Cause a file write error
      //!
      void fileWriteError();

      //! Send a START packet in DATA mode
      //!
      void startPacketInDataMode();

      //! Send a DATA packet in START mode
      //!
      void dataPacketInStartMode();

      //! Send an END packet in START mode
      //!
      void endPacketInStartMode();

      //! Send a file with an out-of-bounds packet
      //!
      void packetOutOfBounds();

      //! Send a file with an out-of-order packet
      //!
      void packetOutOfOrder();

      //! Send a CANCEL packet in START mode
      //!
      void cancelPacketInStartMode();

      //! Send a CANCEL packet in DATA mode
      //!
      void cancelPacketInDataMode();

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

      //! Handler for from_pingOut
      //!
      void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );



    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

      //! Send a FilePacket
      //!
      void sendFilePacket(const Fw::FilePacket& filePacket);

      //! Send a StartPacket
      //!
      void sendStartPacket(
          const char *const sourcePath, //!< The source path
          const char *const destPath, //!< The destination path
          const size_t fileSize //!< The file size
      );

      //! Send a DataPacket
      //!
      void sendDataPacket(
          const size_t byteOffset,
          U8 *const packetData
      );

      //! Send an EndPacket
      //!
      void sendEndPacket(const CFDP::Checksum& checksum);

      //! Send a CancelPacket
      //!
      void sendCancelPacket();

      //! Verify file data
      //!
      void verifyFileData(
          const char *const path,
          const U8 *const sentData,
          const size_t sentDataSize
      );

      //! Remove a file
      //!
      void removeFile(const char *const path);

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      FileUplink component;

      //! The expected number of packets received so far
      //!
      U32 expectedPacketsReceived;

      //! The current sequence index
      //!
      U32 sequenceIndex;


  };

}

#endif
