// ====================================================================== 
// \title  Tester.hpp
// \author bocchino
// \brief  BufferLogger test harness interface
//
// \copyright
// Copyright 2015-2017, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef ASTERIA_Tester_HPP
#define ASTERIA_Tester_HPP

#include <stdio.h>

#include "ASTERIA/Components/BufferLogger/BufferLogger.hpp"
#include "Fw/Comp/ActiveComponentBase.hpp"
#include "Autocode/GTestBase.hpp"

#define QUEUE_DEPTH 10
#define MAX_ENTRIES_PER_FILE 5
#define COM_BUFFER_LENGTH 4
#define SIZE_TYPE U32
#define MAX_BYTES_PER_FILE \
  (MAX_ENTRIES_PER_FILE*COM_BUFFER_LENGTH + MAX_ENTRIES_PER_FILE*sizeof(SIZE_TYPE))

namespace ASTERIA {
  class Tester :
    public BufferLoggerGTestBase
  {

    public:

      // ----------------------------------------------------------------------
      // Constructors and destructors 
      // ----------------------------------------------------------------------

      Tester(
          const U32 maxBytesPerFile = MAX_BYTES_PER_FILE, //!< Max bytes per file
          const U8 sizeOfSize = sizeof(SIZE_TYPE) //!< Number of bytes to use when storing size
      );

      ~Tester(void);

    protected:

      // ----------------------------------------------------------------------
      // Helper functions 
      // ----------------------------------------------------------------------

      //! Generate a test time
      Fw::Time generateTestTime(
          const U32 seconds //!< The seconds value
      );

      //! Set test time seconds
      void setTestTimeSeconds(
          const U32 seconds //!< The seconds value
      );

      //! Connect ports
      void connectPorts(void);

      //! Initialize components
      void initComponents(void);

      //! Dispatch one message on the queue
      void dispatchOne(void);

      //! Dispatch all messages on the queue
      void dispatchAll(void);

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
          const String& fileName //!< The file name
      );

      //! Check that hash file exists
      void checkHashFileExists(
          const String& fileName //!< The file name
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
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_bufferSendOut
      //!
      void from_bufferSendOut_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer fwBuffer 
      );

      //! Handler for from_pingOut
      //!
      void from_pingOut_handler(
        const NATIVE_INT_TYPE portNum, //!< The port number
        U32 key //!< Value to return to pinger
      );

    protected:

      // ----------------------------------------------------------------------
      // Instance variables 
      // ----------------------------------------------------------------------

      //! The component under test
      BufferLogger component;

      //! Data for input buffers
      static const U8 data[COM_BUFFER_LENGTH];

  };

};

#endif
