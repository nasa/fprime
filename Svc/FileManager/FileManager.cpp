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

#include <stdio.h>
#include <stdlib.h>

#include "Svc/FileManager/FileManager.hpp"
#include "Os/File.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  FileManager ::
    FileManager(
        const char *const compName //!< The component name
    ) :
      FileManagerComponentBase(compName),
      commandCount(0),
      errorCount(0)
  {

  }

  void FileManager ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    ) 
  {
    FileManagerComponentBase::init(queueDepth, instance);
  }

  FileManager ::
    ~FileManager(void)
  {

  }

  // ----------------------------------------------------------------------
  // Command handler implementations 
  // ----------------------------------------------------------------------

  void FileManager ::
    CreateDirectory_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& dirName
    )
  {
    Fw::LogStringArg logStringDirName(dirName.toChar());
    this->log_ACTIVITY_HI_CreateDirectoryStarted(logStringDirName);
    const Os::FileSystem::Status status = 
      Os::FileSystem::createDirectory(dirName.toChar());
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_DirectoryCreateError(
          logStringDirName,
          status
      );
    } else {
      this->log_ACTIVITY_HI_CreateDirectorySucceeded(logStringDirName);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
  }

  void FileManager ::
    RemoveFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& fileName
    )
  {
    Fw::LogStringArg logStringFileName(fileName.toChar());
    this->log_ACTIVITY_HI_RemoveFileStarted(logStringFileName);
    const Os::FileSystem::Status status =
      Os::FileSystem::removeFile(fileName.toChar());
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_FileRemoveError(
          logStringFileName,
          status
      );
    } else {
      this->log_ACTIVITY_HI_RemoveFileSucceeded(logStringFileName);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
  }

  void FileManager ::
    MoveFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& sourceFileName,
        const Fw::CmdStringArg& destFileName
    )
  {
    Fw::LogStringArg logStringSource(sourceFileName.toChar());
    Fw::LogStringArg logStringDest(destFileName.toChar());
    this->log_ACTIVITY_HI_MoveFileStarted(logStringSource, logStringDest);
    const Os::FileSystem::Status status = 
      Os::FileSystem::moveFile(
          sourceFileName.toChar(), 
          destFileName.toChar()
      );
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_FileMoveError(
          logStringSource, logStringDest, status
      );
    } else {
      this->log_ACTIVITY_HI_MoveFileSucceeded(logStringSource, logStringDest);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
  }

  void FileManager ::
    RemoveDirectory_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& dirName
    )
  {
    Fw::LogStringArg logStringDirName(dirName.toChar());
    this->log_ACTIVITY_HI_RemoveDirectoryStarted(logStringDirName);
    const Os::FileSystem::Status status =
      Os::FileSystem::removeDirectory(dirName.toChar());
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_DirectoryRemoveError(
          logStringDirName,
          status
      );
    } else {
      this->log_ACTIVITY_HI_RemoveDirectorySucceeded(logStringDirName);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
  }

  void FileManager ::
    ShellCommand_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& command,
        const Fw::CmdStringArg& logFileName
    )
  {
    Fw::LogStringArg logStringCommand(command.toChar());
    this->log_ACTIVITY_HI_ShellCommandStarted(
          logStringCommand
      );
    NATIVE_INT_TYPE status = 
      this->systemCall(command, logFileName);
    if (status == 0) {
      this->log_ACTIVITY_HI_ShellCommandSucceeded(
          logStringCommand
      );
    } else {
      this->log_WARNING_HI_ShellCommandFailed(
          logStringCommand, status
      );
    }
    this->emitTelemetry(
        status == 0 ? Os::FileSystem::OP_OK : Os::FileSystem::OTHER_ERROR
    );
    this->sendCommandResponse(
        opCode, 
        cmdSeq, 
        status == 0 ? Os::FileSystem::OP_OK : Os::FileSystem::OTHER_ERROR
    );
  }

  /**
   * A helper function to determine if two files are the same; returns OP_OK
   * if no errors occur, and the boolean result via a pointer
   * 
   * TODO: Unsure how to check for two different paths pointing to the same
   * nonexistent file?
   */
  Os::FileSystem::Status isPathToSameFile(
    const Fw::CmdStringArg& filePath1, 
    const Fw::CmdStringArg& filePath2,
    bool* isSamePathResult
  ) 
  {
    // Check for literal string path equality first
    *isSamePathResult = filePath1 == filePath2;
    if (*isSamePathResult) {
      // Path literals equal, so we know they're the same
      return Os::FileSystem::OP_OK;
    }

    // Try checking the actual files
    U32 file1ID;
    U32 file2ID;

    Os::File::Status fileStatus = Os::File::niceCRC32(file1ID, filePath1.toChar());
    if (fileStatus != Os::File::OP_OK) {
      if (fileStatus == Os::File::DOESNT_EXIST) {
        // File doesn't exist, which is a valid possibility
        *isSamePathResult = false;
        return Os::FileSystem::OP_OK;
      }
      return Os::FileSystem::OTHER_ERROR;
    }

    fileStatus = Os::File::niceCRC32(file2ID, filePath2.toChar());
    if (fileStatus != Os::File::OP_OK) {
      if (fileStatus == Os::File::DOESNT_EXIST) {
        // File doesn't exist, which is a valid possibility
        *isSamePathResult = false;
        return Os::FileSystem::OP_OK;
      }
      return Os::FileSystem::OTHER_ERROR;
    }

    *isSamePathResult = file1ID == file2ID;
    return Os::FileSystem::OP_OK;
  }

  void FileManager ::
    ConcatFiles_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& fileName1,
        const Fw::CmdStringArg& fileName2,
        const Fw::CmdStringArg& destFileName
    )
  {
    Fw::LogStringArg logStringFile1(fileName1.toChar());
    Fw::LogStringArg logStringFile2(fileName2.toChar());
    Fw::LogStringArg logStringDest(destFileName.toChar());
    this->log_ACTIVITY_HI_ConcatFilesStarted(
      logStringFile1, logStringFile2, logStringDest
    );

    // If destFile == file2, handle as a special case
    bool isFile2AlsoDest;
    Os::FileSystem::Status status =
      isPathToSameFile(
        fileName2.toChar(),
        destFileName.toChar(),
        &isFile2AlsoDest
    );
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_ConcatFilesFailed(
        logStringFile1, logStringFile2, logStringDest, status
      );
      this->emitTelemetry(status);
      this->sendCommandResponse(opCode, cmdSeq, status);
      return;
    }

    if (isFile2AlsoDest) {
      // TODO: Actually handle this case
      this->log_ACTIVITY_HI_ConcatFilesSucceeded(
        logStringFile1, logStringFile2, logStringDest
      );
      this->emitTelemetry(status);
      this->sendCommandResponse(opCode, cmdSeq, status);
      return;
    }

    // Otherwise, handle as normal case
    status = Os::FileSystem::appendFile(
        fileName1.toChar(),
        destFileName.toChar(),
        true
    );
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_ConcatFilesFailed(
        logStringFile1, logStringFile2, logStringDest, status
      );
      this->emitTelemetry(status);
      this->sendCommandResponse(opCode, cmdSeq, status);
      return;
    }

    status = Os::FileSystem::appendFile(
        fileName2.toChar(),
        destFileName.toChar()
    );
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_ConcatFilesFailed(
        logStringFile1, logStringFile2, logStringDest, status
      );
      this->emitTelemetry(status);
      this->sendCommandResponse(opCode, cmdSeq, status);
      return;
    }

    this->log_ACTIVITY_HI_ConcatFilesSucceeded(
      logStringFile1, logStringFile2, logStringDest
    );
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
  }

  void FileManager ::
    pingIn_handler(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
      // return key
      this->pingOut_out(0,key);
  }
  // ----------------------------------------------------------------------
  // Helper methods 
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE FileManager ::
    systemCall(
        const Fw::CmdStringArg& command,
        const Fw::CmdStringArg& logFileName
    ) const
  {
    const U32 bufferSize = 10 + 2 * FW_CMD_STRING_MAX_SIZE;
    char buffer[bufferSize];
    snprintf(
        buffer, sizeof(buffer), "eval '%s' 1>>%s 2>&1\n", 
        command.toChar(), 
        logFileName.toChar()
    );
    const int status = system(buffer);
    return status;
  }

  void FileManager ::
    emitTelemetry(const Os::FileSystem::Status status)
  {
    if (status == Os::FileSystem::OP_OK) {
      ++this->commandCount;
      this->tlmWrite_CommandsExecuted(this->commandCount);
    }
    else {
      ++this->errorCount;
      this->tlmWrite_Errors(this->errorCount);
    }
  }

  void FileManager ::
    sendCommandResponse(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Os::FileSystem::Status status
    )
  {
    this->cmdResponse_out(
        opCode, 
        cmdSeq, 
        (status == Os::FileSystem::OP_OK) ? 
          Fw::COMMAND_OK : Fw::COMMAND_EXECUTION_ERROR
    );
  }

}
