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

#include <cstdio>
#include <cstdlib>

#include "Fw/Types/ExternalString.hpp"
#include "Svc/FileManager/FileManager.hpp"
#include "Fw/Types/Assert.hpp"
#include <FpConfig.hpp>

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

  FileManager ::
    ~FileManager()
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
    bool errorIfDirExists = true;
    const Os::FileSystem::Status status =
      Os::FileSystem::createDirectory(dirName.toChar(), errorIfDirExists);
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
        const Fw::CmdStringArg& fileName,
        const bool ignoreErrors
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
      if (ignoreErrors == true) {
        ++this->errorCount;
        this->tlmWrite_Errors(this->errorCount);
        this->cmdResponse_out(
          opCode,
          cmdSeq,
          Fw::CmdResponse::OK
        );
        return;
      }
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
          logStringCommand, static_cast<U32>(status)
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

  void FileManager ::
    AppendFile_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& source,
        const Fw::CmdStringArg& target
    )
  {
    Fw::LogStringArg logStringSource(source.toChar());
    Fw::LogStringArg logStringTarget(target.toChar());
    this->log_ACTIVITY_HI_AppendFileStarted(logStringSource, logStringTarget);

    Os::FileSystem::Status status;
    status = Os::FileSystem::appendFile(source.toChar(), target.toChar(), true);
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_AppendFileFailed(
        logStringSource,
        logStringTarget,
        status
      );
    } else {
      this->log_ACTIVITY_HI_AppendFileSucceeded(
        logStringSource,
        logStringTarget
      );
    }

    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
  }

  void FileManager ::
    FileSize_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CmdStringArg& fileName
    )
  {
    Fw::LogStringArg logStringFileName(fileName.toChar());
    this->log_ACTIVITY_HI_FileSizeStarted(logStringFileName);

    FwSignedSizeType size_arg;
    const Os::FileSystem::Status status =
      Os::FileSystem::getFileSize(fileName.toChar(), size_arg);
    if (status != Os::FileSystem::OP_OK) {
      this->log_WARNING_HI_FileSizeError(
          logStringFileName,
          status
      );
    } else {
      U64 size = static_cast<U64>(size_arg);
      this->log_ACTIVITY_HI_FileSizeSucceeded(logStringFileName, size);
    }
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
    // Create a buffer of at least enough size for storing the eval string less the 2 %s tokens, two command strings,
    // and a null terminator at the end
    const char evalStr[] = "eval '%s' 1>>%s 2>&1\n";
    constexpr U32 bufferSize = (sizeof(evalStr) - 4) + (2 * FW_CMD_STRING_MAX_SIZE) + 1;
    char buffer[bufferSize];

    // Wrap that buffer in an external string for formatting purposes
    Fw::ExternalString stringBuffer(buffer, bufferSize);
    Fw::FormatStatus formatStatus = stringBuffer.format(evalStr, command.toChar(), logFileName.toChar());
    // Since the buffer is exactly sized, the only error can occur is a software error not caused by ground
    FW_ASSERT(formatStatus == Fw::FormatStatus::SUCCESS);

    // Call the system
    const int status = system(stringBuffer.toChar());
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
          Fw::CmdResponse::OK : Fw::CmdResponse::EXECUTION_ERROR
    );
  }

}
