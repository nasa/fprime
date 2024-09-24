// ======================================================================
// \title  FileManager.hpp
// \author bocchino
// \brief  hpp file for FileManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_FileManager_HPP
#define Svc_FileManager_HPP

#include "Svc/FileManager/FileManagerComponentAc.hpp"
#include "Os/FileSystem.hpp"

namespace Svc {

  class FileManager :
    public FileManagerComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object FileManager
      //!
      FileManager(
          const char *const compName //!< The component name
      );

      //! Destroy object FileManager
      //!
      ~FileManager();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for CreateDirectory command handler
      //!
      void CreateDirectory_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& dirName //!< The directory to create
      );

      //! Implementation for RemoveFile command handler
      //!
      void RemoveFile_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& fileName, //!< The file to remove
          const bool ignoreErrors //!< Ignore missing files
      );

      //! Implementation for MoveFile command handler
      //!
      void MoveFile_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& sourceFileName, //!< The source file name
          const Fw::CmdStringArg& destFileName //!< The destination file name
      );

      //! Implementation for RemoveDirectory command handler
      //!
      void RemoveDirectory_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& dirName //!< The directory to remove
      );

      //! Implementation for ShellCommand command handler
      //!
      void ShellCommand_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& command, //!< The shell command string
          const Fw::CmdStringArg& logFileName //!< The name of the log file
      );

      //! Implementation for ConcatFiles command handler
      //! Append 1 file's contents to the end of another.
      void AppendFile_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& source, //! The name of the file to take content from
          const Fw::CmdStringArg& target //! The name of the file to append to
      );

      //! Implementation for FileSize command handler
      //!
      void FileSize_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          const Fw::CmdStringArg& fileName //!< The file to get the size of
      );

      //! Handler implementation for pingIn
      //!
      void pingIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! A system command with no arguments
      //!
      NATIVE_INT_TYPE systemCall(
          const Fw::CmdStringArg& command, //!< The command
          const Fw::CmdStringArg& logFileName //!< The log file name
      ) const;

      //! Emit telemetry based on status
      //!
      void emitTelemetry(
          const Os::FileSystem::Status status //!< The status
      );

      //! Send command response based on status
      //!
      void sendCommandResponse(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence value
          const Os::FileSystem::Status status //!< The status
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The total number of commands successfully executed
      //!
      U32 commandCount;

      //! The total number of errors
      //!
      U32 errorCount;


  };

} // end namespace Svc

#endif
