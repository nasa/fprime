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

#include <Fw/FPrimeBasicTypes.hpp>
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/ExternalString.hpp"
#include "Os/Directory.hpp"
#include "Svc/FileManager/FileManager.hpp"
#include "config/FileManagerConfig.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

FileManager ::FileManager(const char* const compName  //!< The component name
                          )
    : FileManagerComponentBase(compName),
      commandCount(0),
      errorCount(0),
      m_listState(IDLE),
      m_totalEntries(0),
      m_currentOpCode(0),
      m_currentCmdSeq(0),
      m_runQueued(false),
      m_dpState(DP_IDLE),
      m_dpFileSize(0),
      m_dpOffset(0),
      m_dpChunkSize(0),
      m_dpEndOffset(0),
      m_dpPriority(0),
      m_dpChunkCount(0),
      m_dpOpCode(0),
      m_dpCmdSeq(0),
      m_dpBuffer{} {}

FileManager ::~FileManager() {}

// ----------------------------------------------------------------------
// Command handler implementations
// ----------------------------------------------------------------------

void FileManager ::CreateDirectory_cmdHandler(const FwOpcodeType opCode,
                                              const U32 cmdSeq,
                                              const Fw::CmdStringArg& dirName) {
    Fw::LogStringArg logStringDirName(dirName.toChar());
    this->log_ACTIVITY_HI_CreateDirectoryStarted(logStringDirName);
    bool errorIfDirExists = true;
    const Os::FileSystem::Status status = Os::FileSystem::createDirectory(dirName.toChar(), errorIfDirExists);
    if (status != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_DirectoryCreateError(logStringDirName, status);
    } else {
        this->log_ACTIVITY_HI_CreateDirectorySucceeded(logStringDirName);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
}

void FileManager ::RemoveFile_cmdHandler(const FwOpcodeType opCode,
                                         const U32 cmdSeq,
                                         const Fw::CmdStringArg& fileName,
                                         const bool ignoreErrors) {
    Fw::LogStringArg logStringFileName(fileName.toChar());
    this->log_ACTIVITY_HI_RemoveFileStarted(logStringFileName);
    const Os::FileSystem::Status status = Os::FileSystem::removeFile(fileName.toChar());
    if (status != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_FileRemoveError(logStringFileName, status);
        if (ignoreErrors == true) {
            ++this->errorCount;
            this->tlmWrite_Errors(this->errorCount);
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            return;
        }
    } else {
        this->log_ACTIVITY_HI_RemoveFileSucceeded(logStringFileName);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
}

void FileManager ::MoveFile_cmdHandler(const FwOpcodeType opCode,
                                       const U32 cmdSeq,
                                       const Fw::CmdStringArg& sourceFileName,
                                       const Fw::CmdStringArg& destFileName) {
    Fw::LogStringArg logStringSource(sourceFileName.toChar());
    Fw::LogStringArg logStringDest(destFileName.toChar());
    this->log_ACTIVITY_HI_MoveFileStarted(logStringSource, logStringDest);
    const Os::FileSystem::Status status = Os::FileSystem::moveFile(sourceFileName.toChar(), destFileName.toChar());
    if (status != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_FileMoveError(logStringSource, logStringDest, status);
    } else {
        this->log_ACTIVITY_HI_MoveFileSucceeded(logStringSource, logStringDest);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
}

void FileManager ::RemoveDirectory_cmdHandler(const FwOpcodeType opCode,
                                              const U32 cmdSeq,
                                              const Fw::CmdStringArg& dirName) {
    Fw::LogStringArg logStringDirName(dirName.toChar());
    this->log_ACTIVITY_HI_RemoveDirectoryStarted(logStringDirName);
    const Os::FileSystem::Status status = Os::FileSystem::removeDirectory(dirName.toChar());
    if (status != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_DirectoryRemoveError(logStringDirName, status);
    } else {
        this->log_ACTIVITY_HI_RemoveDirectorySucceeded(logStringDirName);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
}

void FileManager ::AppendFile_cmdHandler(const FwOpcodeType opCode,
                                         const U32 cmdSeq,
                                         const Fw::CmdStringArg& source,
                                         const Fw::CmdStringArg& target) {
    Fw::LogStringArg logStringSource(source.toChar());
    Fw::LogStringArg logStringTarget(target.toChar());
    this->log_ACTIVITY_HI_AppendFileStarted(logStringSource, logStringTarget);

    Os::FileSystem::Status status;
    status = Os::FileSystem::appendFile(source.toChar(), target.toChar(), true);
    if (status != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_AppendFileFailed(logStringSource, logStringTarget, status);
    } else {
        this->log_ACTIVITY_HI_AppendFileSucceeded(logStringSource, logStringTarget);
    }

    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
}

void FileManager ::FileSize_cmdHandler(const FwOpcodeType opCode, const U32 cmdSeq, const Fw::CmdStringArg& fileName) {
    Fw::LogStringArg logStringFileName(fileName.toChar());
    this->log_ACTIVITY_HI_FileSizeStarted(logStringFileName);

    FwSizeType size_arg;
    const Os::FileSystem::Status status = Os::FileSystem::getFileSize(fileName.toChar(), size_arg);
    if (status != Os::FileSystem::OP_OK) {
        this->log_WARNING_HI_FileSizeError(logStringFileName, status);
    } else {
        this->log_ACTIVITY_HI_FileSizeSucceeded(logStringFileName, size_arg);
    }
    this->emitTelemetry(status);
    this->sendCommandResponse(opCode, cmdSeq, status);
}

void FileManager ::ListDirectory_cmdHandler(const FwOpcodeType opCode,
                                            const U32 cmdSeq,
                                            const Fw::CmdStringArg& dirName) {
    // Check if we're already listing a directory
    if (m_listState == LISTING_IN_PROGRESS) {
        this->log_WARNING_HI_ListDirectoryError(dirName, static_cast<U32>(Os::Directory::OTHER_ERROR));
        this->emitTelemetry(Os::FileSystem::OTHER_ERROR);
        this->sendCommandResponse(opCode, cmdSeq, Os::FileSystem::OTHER_ERROR);
        return;
    }

    this->log_ACTIVITY_HI_ListDirectoryStarted(dirName);

    // Open the directory for reading
    Os::Directory::Status status = m_currentDir.open(dirName.toChar(), Os::Directory::OpenMode::READ);

    if (status != Os::Directory::OP_OK) {
        this->log_WARNING_HI_ListDirectoryError(dirName, static_cast<U32>(status));
        this->emitTelemetry(Os::FileSystem::OTHER_ERROR);
        this->sendCommandResponse(opCode, cmdSeq, Os::FileSystem::OTHER_ERROR);
        return;
    }

    // Initialize state machine for asynchronous processing
    m_listState = LISTING_IN_PROGRESS;
    m_currentDirName = dirName;
    m_currentOpCode = opCode;
    m_currentCmdSeq = cmdSeq;
    m_totalEntries = 0;

    // Directory listing will be processed asynchronously by the rate group.
    // The schedIn_handler will process FILES_PER_RATE_TICK directory entries per rate tick to
    // prevent event flooding while maintaining configurable performance.
    // Command response will be sent when listing completes.
}

void FileManager ::CalculateCrc_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& filename) {
    Os::File file;
    U32 crcValue = 0;
    this->log_ACTIVITY_HI_CalculateCrcStarted(filename);

    Os::File::Status status = file.open(filename.toChar(), Os::File::OPEN_READ);
    if (status == Os::File::OP_OK) {
        status = file.calculateCrc(crcValue);
    }

    if (status == Os::File::OP_OK) {
        this->log_ACTIVITY_HI_CalculateCrcSucceeded(filename, crcValue);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
    } else {
        this->log_WARNING_HI_CalculateCrcFailed(filename, status);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
    file.close();
}

void FileManager ::GenerateDp_cmdHandler(FwOpcodeType opCode,
                                         U32 cmdSeq,
                                         const Fw::CmdStringArg& fileName,
                                         U32 chunkSize,
                                         U64 beginOffset,
                                         U64 endOffset,
                                         U32 priority,
                                         const FileManager_GenerateDpMode& mode) {
    Fw::LogStringArg logFileName(fileName.toChar());

    // Reject a second request while one is already running
    if (this->m_dpState != DP_IDLE) {
        this->log_WARNING_HI_GenerateDpFailed(logFileName, FileManager_GenerateDpStage::BUSY, 0);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        return;
    }

    // Data products must be available
    if (!this->isConnected_productGetOut_OutputPort(0) || !this->isConnected_productSendOut_OutputPort(0)) {
        this->log_WARNING_HI_GenerateDpBufferFailed(logFileName);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        return;
    }

    // Clamp the requested chunk size to the configured read buffer
    U32 effectiveChunkSize = chunkSize;
    if ((effectiveChunkSize == 0) || (effectiveChunkSize > FileManagerConfig::GENERATE_DP_MAX_CHUNK_SIZE)) {
        effectiveChunkSize = FileManagerConfig::GENERATE_DP_MAX_CHUNK_SIZE;
    }

    Os::File::Status status = this->m_dpFile.open(fileName.toChar(), Os::File::OPEN_READ);
    if (status != Os::File::OP_OK) {
        this->log_WARNING_HI_GenerateDpFailed(logFileName, FileManager_GenerateDpStage::OPEN, static_cast<U32>(status));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        return;
    }

    FwSizeType fileSize = 0;
    status = this->m_dpFile.size(fileSize);
    if (status != Os::File::OP_OK) {
        this->m_dpFile.close();
        this->log_WARNING_HI_GenerateDpFailed(logFileName, FileManager_GenerateDpStage::SIZE, static_cast<U32>(status));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        return;
    }

    // An end offset of zero, or one past the end of the file, means the end of
    // the file. Ranges let an operator retransmit part of a file or spread the
    // downlink over several commands.
    U64 effectiveEnd = endOffset;
    if ((effectiveEnd == 0) || (effectiveEnd > static_cast<U64>(fileSize))) {
        effectiveEnd = static_cast<U64>(fileSize);
    }

    const bool emptyFile = (fileSize == 0);
    const bool badRange = (beginOffset > static_cast<U64>(fileSize)) || (!emptyFile && (beginOffset >= effectiveEnd));
    if (badRange) {
        this->m_dpFile.close();
        this->log_WARNING_HI_GenerateDpInvalidRange(logFileName, beginOffset, endOffset, static_cast<U64>(fileSize));
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
        return;
    }

    // Position the file at the start of the requested range
    if (beginOffset > 0) {
        status = this->m_dpFile.seek(static_cast<FwSignedSizeType>(beginOffset), Os::File::SeekType::ABSOLUTE);
        if (status != Os::File::OP_OK) {
            this->m_dpFile.close();
            this->log_WARNING_HI_GenerateDpFailed(logFileName, FileManager_GenerateDpStage::SEEK,
                                                  static_cast<U32>(status));
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            return;
        }
    }

    this->m_dpFileName = Fw::String(fileName.toChar());
    this->m_dpFileSize = fileSize;
    this->m_dpOffset = beginOffset;
    this->m_dpEndOffset = effectiveEnd;
    this->m_dpChunkSize = effectiveChunkSize;
    this->m_dpChunkCount = 0;
    this->m_dpOpCode = opCode;
    this->m_dpCmdSeq = cmdSeq;
    // A priority of zero reverts to the configured default
    this->m_dpPriority = (priority == 0) ? static_cast<FwDpPriorityType>(FileManagerCfg::DEFAULT_DP_PRIORITY)
                                         : static_cast<FwDpPriorityType>(priority);
    this->m_dpState = DP_IN_PROGRESS;

    // Report the number of bytes that will be written, which is the requested
    // range rather than the size of the whole file
    this->log_ACTIVITY_HI_GenerateDpStarted(logFileName, this->m_dpEndOffset - this->m_dpOffset);

    // An empty range produces no chunks, so complete immediately
    if (this->m_dpOffset >= this->m_dpEndOffset) {
        this->log_ACTIVITY_HI_GenerateDpComplete(logFileName, this->m_dpChunkCount);
        this->finishDpGeneration();
        return;
    }

    // In immediate mode the whole range is emitted here, so that a project that
    // wants the file out quickly is not limited by the rate group. In paced
    // mode the rate group meters the work out and the response is deferred.
    if (mode == FileManager_GenerateDpMode::IMMEDIATE) {
        this->processDpChunks(0);
    }
}

void FileManager ::processDpChunks(U32 chunkLimit) {
    Fw::LogStringArg logFileName(this->m_dpFileName.toChar());

    // A limit of zero means emit the whole remaining range in this call
    const bool paced = (chunkLimit > 0);

    for (U32 chunk = 0; !paced || (chunk < chunkLimit); chunk++) {
        // Number of bytes remaining in the requested range. The loop returns as
        // soon as the range is exhausted, so this is always non-zero here.
        const FwSizeType remaining = static_cast<FwSizeType>(this->m_dpEndOffset - this->m_dpOffset);

        const FwSizeType requestedSize = (remaining < static_cast<FwSizeType>(this->m_dpChunkSize))
                                             ? remaining
                                             : static_cast<FwSizeType>(this->m_dpChunkSize);

        // The file size is known, so a short read means the file changed underneath us
        FwSizeType readSize = requestedSize;
        const Os::File::Status status = this->m_dpFile.read(this->m_dpBuffer, readSize);
        if ((status != Os::File::OP_OK) || (readSize != requestedSize)) {
            this->log_WARNING_HI_GenerateDpFailed(logFileName, FileManager_GenerateDpStage::READ,
                                                  static_cast<U32>(status));
            this->finishDpGeneration();
            return;
        }

        // Request a container large enough for this chunk's header and data
        const FwSizeType dpSize = SIZE_OF_FileChunkHeaderRecord_RECORD + SIZE_OF_FileChunkDataRecord_RECORD(readSize);
        DpContainer container;
        const Fw::Success::T dpStatus = this->dpGet_FileDpContainer(dpSize, container);
        if (dpStatus != Fw::Success::SUCCESS) {
            this->log_WARNING_HI_GenerateDpBufferFailed(logFileName);
            this->finishDpGeneration();
            return;
        }
        container.setPriority(this->m_dpPriority);

        // Each chunk is a metadata record followed by a data record, so that
        // ground tools can reassemble the file from any number of containers
        const FileManager_FileChunkHeader header(this->m_dpFileName, this->m_dpOffset, static_cast<U32>(readSize));
        Fw::SerializeStatus serializeStatus = container.serializeRecord_FileChunkHeaderRecord(header);
        if (serializeStatus == Fw::FW_SERIALIZE_OK) {
            serializeStatus = container.serializeRecord_FileChunkDataRecord(this->m_dpBuffer, readSize);
        }
        if (serializeStatus != Fw::FW_SERIALIZE_OK) {
            this->log_WARNING_HI_GenerateDpFailed(logFileName, FileManager_GenerateDpStage::SERIALIZE,
                                                  static_cast<U32>(serializeStatus));
            this->finishDpGeneration();
            return;
        }

        this->dpSend(container);

        this->m_dpOffset += static_cast<U64>(readSize);
        this->m_dpChunkCount++;

        // Last chunk of the requested range
        if (this->m_dpOffset >= this->m_dpEndOffset) {
            this->log_ACTIVITY_HI_GenerateDpComplete(logFileName, this->m_dpChunkCount);
            this->finishDpGeneration();
            return;
        }
    }
}

void FileManager ::finishDpGeneration() {
    this->m_dpFile.close();
    this->m_dpState = DP_IDLE;
    this->m_dpOffset = 0;
    this->m_dpEndOffset = 0;
    this->m_dpFileSize = 0;
    // Failures emit a warning event but still respond with OK, so that a bad
    // file name or a transient resource problem does not stop a whole sequence
    this->cmdResponse_out(this->m_dpOpCode, this->m_dpCmdSeq, Fw::CmdResponse::OK);
}

void FileManager ::pingIn_handler(const FwIndexType portNum, U32 key) {
    // return key
    this->pingOut_out(0, key);
}

void FileManager ::schedIn_handler(const FwIndexType portNum, U32 context) {
    bool isQueued = false;
    // m_runQueued will be compared to isQueued (false). When equal (i.e. m_runQueued is false) the atomic will be
    // set to true and the function will return true indicating that a run was successfully marked as queued and thus
    // the internal handler should be invoked.
    bool expects_enqueue = this->m_runQueued.compare_exchange_strong(isQueued, true);
    if (expects_enqueue) {
        this->run_internalInterfaceInvoke();
    }
}

void FileManager ::run_internalInterfaceHandler() {
    FW_ASSERT(this->m_runQueued);
    this->m_runQueued = false;  // Run is not queued anymore (we are running)
    // Data product generation is paced the same way as directory listing
    if (this->m_dpState == DP_IN_PROGRESS) {
        this->processDpChunks(FileManagerConfig::CHUNKS_PER_RATE_TICK);
    }

    // Only process if we're in the middle of a directory listing
    if (m_listState == LISTING_IN_PROGRESS) {
        // Process multiple files per rate tick based on configuration
        for (U32 fileCount = 0; fileCount < Svc::FileManagerConfig::FILES_PER_RATE_TICK; fileCount++) {
            Fw::String filename;
            Os::Directory::Status status = m_currentDir.read(filename);

            if (status == Os::Directory::NO_MORE_FILES) {
                // We're done listing - close directory and send response
                m_currentDir.close();
                m_listState = IDLE;

                this->log_ACTIVITY_HI_ListDirectorySucceeded(m_currentDirName, m_totalEntries);
                this->emitTelemetry(Os::FileSystem::OP_OK);
                this->sendCommandResponse(m_currentOpCode, m_currentCmdSeq, Os::FileSystem::OP_OK);
                break;  // Exit the loop since we're done

            } else if (status == Os::Directory::OP_OK) {
                // Construct full path for type checking
                Fw::String fullPath;
                Fw::FormatStatus formatStatus = fullPath.format("%s/%s", m_currentDirName.toChar(), filename.toChar());

                // Determine entry type
                Os::FileSystem::PathType pathType = (formatStatus == Fw::FormatStatus::SUCCESS)
                                                        ? Os::FileSystem::getPathType(fullPath.toChar())
                                                        : Os::FileSystem::NOT_EXIST;

                if (formatStatus != Fw::FormatStatus::SUCCESS) {
                    // Cannot determine the type of an entry whose path did not format
                    this->log_WARNING_HI_FileNameFormatError(filename,
                                                             static_cast<Fw::StringFormatStatus::T>(formatStatus));
                } else if (pathType == Os::FileSystem::FILE) {
                    // Regular file: get size and emit file event
                    FwSizeType fileSize;
                    Os::FileSystem::Status sizeStatus = Os::FileSystem::getFileSize(fullPath.toChar(), fileSize);
                    this->log_ACTIVITY_HI_DirectoryListing(
                        m_currentDirName, filename,
                        (sizeStatus == Os::FileSystem::OP_OK) ? fileSize : static_cast<FwSizeType>(0));
                } else if (pathType == Os::FileSystem::DIRECTORY) {
                    // Subdirectory: emit subdirectory event
                    this->log_ACTIVITY_HI_DirectoryListingSubdir(m_currentDirName, filename);
                } else {
                    // Special file or inaccessible: treat as file with 0 size
                    this->log_ACTIVITY_HI_DirectoryListing(m_currentDirName, filename, static_cast<FwSizeType>(0));
                }

                m_totalEntries++;

            } else {
                // Error reading directory - close and send error response
                m_currentDir.close();
                m_listState = IDLE;

                this->log_WARNING_HI_ListDirectoryError(m_currentDirName, static_cast<U32>(status));
                this->emitTelemetry(Os::FileSystem::OTHER_ERROR);
                this->sendCommandResponse(m_currentOpCode, m_currentCmdSeq, Os::FileSystem::OTHER_ERROR);
                break;  // Exit the loop since we had an error
            }
        }
    }
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void FileManager ::emitTelemetry(const Os::FileSystem::Status status) {
    if (status == Os::FileSystem::OP_OK) {
        ++this->commandCount;
        this->tlmWrite_CommandsExecuted(this->commandCount);
    } else {
        ++this->errorCount;
        this->tlmWrite_Errors(this->errorCount);
    }
}

void FileManager ::sendCommandResponse(const FwOpcodeType opCode,
                                       const U32 cmdSeq,
                                       const Os::FileSystem::Status status) {
    this->cmdResponse_out(opCode, cmdSeq,
                          (status == Os::FileSystem::OP_OK) ? Fw::CmdResponse::OK : Fw::CmdResponse::EXECUTION_ERROR);
}

}  // namespace Svc
