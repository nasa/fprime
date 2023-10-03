// ======================================================================
// \title  BufferLogger/test/ut/Tester.hpp
// \author bocchino, mereweth
// \brief  hpp file for BufferLogger test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "BufferLoggerGTestBase.hpp"
#include "Svc/BufferLogger/BufferLogger.hpp"

#define COM_BUFFER_LENGTH 4
#define MAX_ENTRIES_PER_FILE 5
#define SIZE_TYPE U32
#define MAX_BYTES_PER_FILE \
  (MAX_ENTRIES_PER_FILE*COM_BUFFER_LENGTH + MAX_ENTRIES_PER_FILE*sizeof(SIZE_TYPE))

namespace Svc {

  class BufferLoggerTester :
    public BufferLoggerGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object BufferLoggerTester
      //!
      BufferLoggerTester(
          bool doInitLog = true
      );

      //! Destroy object BufferLoggerTester
      //!
      ~BufferLoggerTester();

      // ----------------------------------------------------------------------
      // Tests (rest are in Errors, Health, and Logging classes)
      // ----------------------------------------------------------------------

    public:

      //! No-one called initLog
      void LogNoInit();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_bufferSendOut
      //!
      void from_bufferSendOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          Fw::Buffer& fwBuffer
      );

      //! Handler for from_pingOut
      //!
      void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

    protected:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Dispatch one message on the queue
      void dispatchOne();

      //! Dispatch all messages on the queue
      void dispatchAll();

      //! Generate a test time
      Fw::Time generateTestTime(
          const U32 seconds //!< The seconds value
      );

      //! Set test time seconds
      void setTestTimeSeconds(
          const U32 seconds //!< The seconds value
      );

      //! Send com buffers to comIn
      void sendComBuffers(
          const U32 n //!< The number of buffers to send
      );

      //! Send managed buffers to bufferSendIn
      void sendManagedBuffers(
          const U32 n //!< The number of buffers to send
      );

      //! Check that file exists
      void checkFileExists(
          const Fw::StringBase& fileName //!< The file name
      );

      //! Check that hash file exists
      void checkHashFileExists(
          const Fw::StringBase& fileName //!< The file name
      );

      //! Check the integrity of a log file
      void checkLogFileIntegrity(
          const char *const fileName, //!< The file name
          const U32 expectedSize, //!< The expected file size in bytes
          const U32 expectedNumBuffers //!< The expected number of buffers
      );

      //! Check file validation
      void checkFileValidation(
          const char *const fileName //!< The file name
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

    protected:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      BufferLogger component;

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! Data for input buffers
      static U8 data[COM_BUFFER_LENGTH];

  };

} // end namespace Svc

#endif
