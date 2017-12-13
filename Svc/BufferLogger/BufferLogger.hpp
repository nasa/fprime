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

#ifndef ASTERIA_BufferLogger_HPP
#define ASTERIA_BufferLogger_HPP

#include "ASTERIA/Components/BufferLogger/BufferLoggerComponentAc.hpp"
#include "ASTERIA/Types/Files/File.hpp"
#include "ASTERIA/Types/Files/ValidatedFile.hpp"
#include "ASTERIA/Types/NonVolatile/NonVolatileU8.hpp"
#include "ASTERIA/Types/String/String.hpp"
#include "Fw/Types/Assert.hpp"
#include "Os/Mutex.hpp"
#include "Utils/Hash/Hash.hpp"

namespace ASTERIA {

  class BufferLogger :
    public BufferLoggerComponentBase
  {

    PRIVATE:
     
      // ----------------------------------------------------------------------
      // Types 
      // ----------------------------------------------------------------------

      //! The logging state
      class State {

        public:

          //! Construct a State object
          State(
              BufferLogger& bufferLogger, //!< The enclosing BufferLogger instance
              const char *const filePath //!< The file path for storing the non-volatile state
          );

        public:

          //! Get the volatile state
          //! \return The state
          OnOff::t get(void) const;

          //! Set the volatile state
          void set(
              const OnOff::t state //!< The state
          );

          //! Save the non-volatile state to the disk
          //! \return The status
          NonVolatileU8::Status::t save(void);

          //! Load the non-volatile state from the disk
          //! \return The status
          NonVolatileU8::Status::t load(void);

        PRIVATE:

          //! The enclosing BufferLogger instance
          BufferLogger& bufferLogger;

          //! The volatile state
          OnOff::t volatileState;

          //! The non-volatile state
          NonVolatileU8 nonVolatileState;

      };

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

          //! Open errors
          class OpenErrors {

            public:

              //! Construct an OpenErrors object
              OpenErrors(
                  BufferLogger& bufferLogger //!< The enclosing BufferLogger instance
              );

            public:

              //! Emit a write error
              void emit(
                  const U32 status, //!< The status returned from the open operation
                  Fw::LogStringArg& fileName //!< The file name
              );

              //! Clear the error state
              void clear(void);

            PRIVATE:

              //! The enclosing BufferLogger instance
              BufferLogger& bufferLogger;

              //! Whether a write error has occurred
              bool errorOccurred;

          };

          //! Write errors
          class WriteErrors {

            public:

              //! Construct a WriteErrors object
              WriteErrors(
                  BufferLogger& bufferLogger //!< The enclosing BufferLogger instance
              );

            public:

              //! Emit a write error
              void emit(
                  const U32 status, //!< The status returned from the write operation
                  const U32 bytesWritten, //!< The number of bytes successfully written
                  const U32 bytesAttempted, //!< The number of bytes in the write attempt
                  Fw::LogStringArg& fileName //!< The file name
              );

              //! Clear the error state
              void clear(void);

            PRIVATE:

              //! The enclosing BufferLogger instance
              BufferLogger& bufferLogger;

              //! Whether a write error has occurred
              bool errorOccurred;

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

          //! Format a file name using the stored prefix,
          //! the time, and the stored suffix
          void formatName(
              String& name, //!< The name to format
              const Fw::Time& time //!< The time stamp
          );

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
          const String prefix;

          //! The suffix to use for file names
          const String suffix;

          //! The maximum file size
          const U32 maxSize;

          //! The number of bytes to use when storing the size field at the start of each buffer
          const U8 sizeOfSize;

          //! The name of the currently open file
          String name;
          
          // The current mode
          Mode::t mode;

          //! The underlying ASTERIA::File representation
          ASTERIA::File asteriaFile;

          //! The number of bytes written to the current file
          U32 bytesWritten;

          //! Open errors
          OpenErrors openErrors;

          //! Write errors
          WriteErrors writeErrors;

      };

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Create a BufferLogger object
      BufferLogger(
          const char *const compName, //!< The component name
          const char *const stateFilePath, //!< The path to the file for storing the non-volatile on/off state
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

      //! Call this after topology construction
      void setup(void);

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

      //! Implementation for CloseFile command handler
      //! Close the currently open log file, if any
      void CloseFile_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq //!< The command sequence number
      );

      //! Handler for command SetSaveState
      //! Sets the volatile logging state and saves the state to non-volatile memory 
      void SetSaveState_cmdHandler(
        const FwOpcodeType opCode, //!< The opcode
        const U32 cmdSeq, //!< The command sequence number
        OnOff state //!< The state
      );

      //! Handler for command SetState
      //! Sets the volatile logging state 
      void SetVolatileState_cmdHandler(
        const FwOpcodeType opCode, //!< The opcode
        const U32 cmdSeq, //!< The command sequence number
        OnOff state //!< The state
      );

      //! Handler for command SaveState
      //! Saves the current volatile logging state to non-volatile memory 
      void SaveState_cmdHandler(
        const FwOpcodeType opCode, //!< The opcode
        const U32 cmdSeq //!< The command sequence number
      );

      //! Handler for command LoadState
      //! Loads the non-volatile logging state 
      void LoadState_cmdHandler(
        const FwOpcodeType opCode, //!< The opcode
        const U32 cmdSeq //!< The command sequence number
      );

      //! Handler for command Flush
      //! Flushes the current open log file to disk 
      void Flush_cmdHandler(
        const FwOpcodeType opCode, //!< The opcode
        const U32 cmdSeq //!< The command sequence number
      );

  PRIVATE:

      // ----------------------------------------------------------------------
      // Private instance variables
      // ----------------------------------------------------------------------

      //! The logging state
      State state;

      //! The file
      File file;

  };

}

#endif
