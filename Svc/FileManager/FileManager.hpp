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

#include <atomic>
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "Svc/FileManager/FileManagerComponentAc.hpp"
#include "config/FileManagerConfig.hpp"

namespace Svc {

class FileManager final : public FileManagerComponentBase {
    friend class FileManagerTester;

  public:
    // ----------------------------------------------------------------------
    // Construction, initialization, and destruction
    // ----------------------------------------------------------------------

    //! Construct object FileManager
    //!
    FileManager(const char* const compName  //!< The component name
    );

    //! Destroy object FileManager
    //!
    ~FileManager();

  private:
    // ----------------------------------------------------------------------
    // Command handler implementations
    // ----------------------------------------------------------------------

    //! Implementation for CreateDirectory command handler
    //!
    void CreateDirectory_cmdHandler(const FwOpcodeType opCode,       //!< The opcode
                                    const U32 cmdSeq,                //!< The command sequence number
                                    const Fw::CmdStringArg& dirName  //!< The directory to create
                                    ) override;

    //! Implementation for RemoveFile command handler
    //!
    void RemoveFile_cmdHandler(const FwOpcodeType opCode,         //!< The opcode
                               const U32 cmdSeq,                  //!< The command sequence number
                               const Fw::CmdStringArg& fileName,  //!< The file to remove
                               const bool ignoreErrors            //!< Ignore missing files
                               ) override;

    //! Implementation for MoveFile command handler
    //!
    void MoveFile_cmdHandler(const FwOpcodeType opCode,               //!< The opcode
                             const U32 cmdSeq,                        //!< The command sequence number
                             const Fw::CmdStringArg& sourceFileName,  //!< The source file name
                             const Fw::CmdStringArg& destFileName     //!< The destination file name
                             ) override;

    //! Implementation for RemoveDirectory command handler
    //!
    void RemoveDirectory_cmdHandler(const FwOpcodeType opCode,       //!< The opcode
                                    const U32 cmdSeq,                //!< The command sequence number
                                    const Fw::CmdStringArg& dirName  //!< The directory to remove
                                    ) override;

    //! Implementation for AppendFile command handler
    //! Append 1 file's contents to the end of another.
    void AppendFile_cmdHandler(const FwOpcodeType opCode,       //!< The opcode
                               const U32 cmdSeq,                //!< The command sequence number
                               const Fw::CmdStringArg& source,  //! The name of the file to take content from
                               const Fw::CmdStringArg& target   //! The name of the file to append to
                               ) override;

    //! Implementation for FileSize command handler
    //!
    void FileSize_cmdHandler(const FwOpcodeType opCode,        //!< The opcode
                             const U32 cmdSeq,                 //!< The command sequence number
                             const Fw::CmdStringArg& fileName  //!< The file to get the size of
                             ) override;

    //! Implementation for ListDirectory command handler
    //!
    void ListDirectory_cmdHandler(const FwOpcodeType opCode,       //!< The opcode
                                  const U32 cmdSeq,                //!< The command sequence number
                                  const Fw::CmdStringArg& dirName  //!< The directory to list
                                  ) override;

    //! Handler implementation for command CalculateCrc
    //!
    //! Calculate the CRC of a file
    void CalculateCrc_cmdHandler(FwOpcodeType opCode,              //!< The opcode
                                 U32 cmdSeq,                       //!< The command sequence number
                                 const Fw::CmdStringArg& filename  //!< The file to CRC
                                 ) override;

    //! Implementation for GenerateDp command handler
    //! Package a file into a data product, split into chunks
    void GenerateDp_cmdHandler(FwOpcodeType opCode,                    //!< The opcode
                               U32 cmdSeq,                             //!< The command sequence number
                               const Fw::CmdStringArg& fileName,       //!< The file to package as a data product
                               U32 chunkSize,                          //!< The maximum number of file bytes per chunk
                               U64 beginOffset,                        //!< The offset in the file at which to start
                               U64 endOffset,                          //!< The offset at which to stop, exclusive
                               U32 priority,                           //!< The container priority; 0 means the default
                               const FileManager_GenerateDpMode& mode  //!< Paced or immediate chunk emission
                               ) override;

    //! Handler implementation for pingIn
    //!
    void pingIn_handler(const FwIndexType portNum, /*!< The port number*/
                        U32 key                    /*!< Value to return to pinger*/
                        ) override;

    //! Handler implementation for schedIn
    //! This handler is called by a Rate Group. It processes
    //! a configurable number of directory entries per rate tick to prevent
    //! event flooding and ensure bounded execution time.
    //!
    void schedIn_handler(const FwIndexType portNum, /*!< The port number*/
                         U32 context                /*!< The call order*/
                         ) override;

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Emit telemetry based on status
    //!
    void emitTelemetry(const Os::FileSystem::Status status  //!< The status
    );

    //! Send command response based on status
    //!
    void sendCommandResponse(const FwOpcodeType opCode,           //!< The opcode
                             const U32 cmdSeq,                    //!< The command sequence value
                             const Os::FileSystem::Status status  //!< The status
    );

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for user-defined internal interfaces
    // ----------------------------------------------------------------------

    //! Handler implementation for run
    //!
    //! Internal port for handling schedIn
    void run_internalInterfaceHandler() override;

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The total number of commands successfully executed
    //!
    U32 commandCount;

    //! The total number of errors
    //!
    U32 errorCount;

    // ----------------------------------------------------------------------
    // Directory listing state machine variables
    // ----------------------------------------------------------------------
    // The FileManager uses an asynchronous state machine to process
    // directory listings through Rate Group 2. This prevents event
    // flooding and ensures bounded execution time by processing
    // FILES_PER_RATE_TICK directory entries per rate tick.

    //! Directory listing state enumeration
    enum ListDirectoryState {
        IDLE,                //!< Not currently listing a directory
        LISTING_IN_PROGRESS  //!< Currently processing directory entries via rate group
    };

    //! Current state of directory listing operation
    ListDirectoryState m_listState;

    //! Directory handle being currently processed
    Os::Directory m_currentDir;

    //! Name of directory being listed (stored for event reporting)
    Fw::String m_currentDirName;

    //! Total entries processed (for completion event)
    U32 m_totalEntries;

    //! Command opcode stored for final response
    FwOpcodeType m_currentOpCode;

    //! Command sequence number stored for final response
    U32 m_currentCmdSeq;

    std::atomic<bool> m_runQueued;

    // ----------------------------------------------------------------------
    // Data product generation state machine variables
    // ----------------------------------------------------------------------
    // Files are packaged into data products one chunk at a time, paced by
    // the rate group in the same way as directory listing. This keeps the
    // work bounded per tick and lets the command succeed regardless of how
    // large the buffers allocated to data products are.

    //! Data product generation state enumeration
    enum GenerateDpState {
        DP_IDLE,        //!< Not currently generating a data product
        DP_IN_PROGRESS  //!< Currently emitting file chunks via rate group
    };

    //! Current state of data product generation
    GenerateDpState m_dpState;

    //! File handle for the file being packaged
    Os::File m_dpFile;

    //! Name of the file being packaged (stored for records and events)
    Fw::String m_dpFileName;

    //! Total size of the file being packaged
    FwSizeType m_dpFileSize;

    //! Offset of the next chunk to read from the file
    U64 m_dpOffset;

    //! Number of file bytes to read per chunk
    U32 m_dpChunkSize;

    //! Offset at which packaging stops, exclusive
    U64 m_dpEndOffset;

    //! Priority to use for the containers of the current request
    FwDpPriorityType m_dpPriority;

    //! Number of chunks emitted so far (for the completion event)
    U32 m_dpChunkCount;

    //! Command opcode stored for the deferred response
    FwOpcodeType m_dpOpCode;

    //! Command sequence number stored for the deferred response
    U32 m_dpCmdSeq;

    //! Read buffer for one chunk; sized by configuration so that no
    //! dynamic allocation is needed in flight code
    U8 m_dpBuffer[FileManagerConfig::GENERATE_DP_MAX_CHUNK_SIZE];

    //! Emit chunks of the current file; a limit of zero emits all remaining chunks
    void processDpChunks(U32 chunkLimit);

    //! Close out data product generation and respond to the command
    void finishDpGeneration();
};

}  // end namespace Svc

#endif
