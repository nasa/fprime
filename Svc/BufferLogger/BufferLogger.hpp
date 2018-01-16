// ======================================================================
// \title  BufferLogger.hpp
// \author bocchino, dinkel
// \brief  ASTERIA Buffer Logger interface
//
// \copyright
// Copyright (C) 2015-2017 California Institute of Technology.
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

#ifndef Svc_BufferLogger_HPP
#define Svc_BufferLogger_HPP

#include "Svc/BufferLogger/BufferLoggerComponentAc.hpp"
#include "Os/File.hpp"
#include "Fw/Types/EightyCharString.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Mutex.hpp"
#include "Utils/Hash/Hash.hpp"

namespace Svc {

  class BufferLogger :
    public BufferLoggerComponentBase
  {

    PRIVATE:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

      //! A BufferLogger file
      class File {

        public:

          //! The file mode
          struct Mode {
            typedef enum {
              CLOSED = 0,
              OPEN = 1
            } t;
          };

        public:

          //! Construct a File object
          File(
              BufferLogger& bufferLogger, //!< The enclosing BufferLogger instance
              const char* prefix, //!< The file name prefix
              const char* suffix, //!< The file name suffix
              const U32 maxSize, //!< The maximum file size
              const U8 sizeOfSize //!< The number of bytes to use when storing the size field and the start of each buffer
          );

          //! Destroy a File object
          ~File(void);

        public:

          //! Log a buffer
          void logBuffer(
              const U8 *const data, //!< The buffer data
              const U32 size //!< The size
          );

          //! Close the file and emit an event
          void closeAndEmitEvent(void);

          //! Flush the file
          bool flush(void);

        PRIVATE:

          //! Open the file
          void open(void);

          //! Write a buffer to a file
          //! \return Success or failure
          bool writeBuffer(
              const U8 *const data, //!< The buffer data
              const U32 size //!< The number of bytes to write
          );

          //! Write the size field of a buffer
          //! \return Success or failure
          bool writeSize(
              const U32 size //!< The size
          );

          //! Write bytes to a file
          //! \return Success or failure
          bool writeBytes(
              const void *const data, //!< The data
              const U32 length //!< The number of bytes to write
          );

          //! Write a hash file
          void writeHashFile(void);

          //! Close the file
          void close(void);

        PRIVATE:

          //! The enclosing BufferLogger instance
          BufferLogger& bufferLogger;

          //! The prefix to use for file names
          const Fw::EightyCharString prefix;

          //! The suffix to use for file names
          const Fw::EightyCharString suffix;

          //! The maximum file size
          const U32 maxSize;

          //! The number of bytes to use when storing the size field at the start of each buffer
          const U8 sizeOfSize;

          //! The name of the currently open file
          Fw::EightyCharString name;

          // The current mode
          Mode::t mode;

          //! The underlying Os::File representation
          Os::File osFile;

          //! The number of bytes written to the current file
          U32 bytesWritten;

      }; // class File

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Create a BufferLogger object
      BufferLogger(
          const char *const compName, //!< The component name
          const char *const logFilePrefix, //!< The log file name prefix
          const char *const logFileSuffix, //!< The log file name suffix
          const U32 maxFileSize, //!< The maximum file size
          const U8 sizeOfSize //!< The number of bytes to use when storing the size field at the start of each buffer
      );

      //! Initialize a BufferLogger object
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance //!< The instance number
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for bufferSendIn
      //!
      void bufferSendIn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::Buffer fwBuffer
      );

      //! Handler implementation for comIn
      //!
      void comIn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          Fw::ComBuffer &data, //!< Buffer containing packet data
          U32 context //!< Call context value; meaning chosen by user
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          U32 key //!< Value to return to pinger
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for BL_OpenFile command handler
      //! Open a new log file with specified name; required before activating logging
      void BL_OpenFile_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          const Fw::CmdStringArg& file
      );

      //! Implementation for BL_CloseFile command handler
      //! Close the currently open log file, if any
      void BL_CloseFile_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

      //! Implementation for BL_SetLogging command handler
      //! Sets the volatile logging state
      void BL_SetLogging_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          LogState state
      );

      //! Implementation for BL_FlushFile command handler
      //! Flushes the current open log file to disk
      void BL_FlushFile_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

  PRIVATE:

      // ----------------------------------------------------------------------
      // Private instance variables
      // ----------------------------------------------------------------------

      //! The logging state
      LogState m_state;

      //! The file
      File m_file;

  };

}

#endif
