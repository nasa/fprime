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

#include "Os/FileSystem.hpp"
#include "Svc/FileManager/FileManagerComponentAc.hpp"

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
    );

    //! Implementation for RemoveFile command handler
    //!
    void RemoveFile_cmdHandler(const FwOpcodeType opCode,         //!< The opcode
                               const U32 cmdSeq,                  //!< The command sequence number
                               const Fw::CmdStringArg& fileName,  //!< The file to remove
                               const bool ignoreErrors            //!< Ignore missing files
    );

    //! Implementation for MoveFile command handler
    //!
    void MoveFile_cmdHandler(const FwOpcodeType opCode,               //!< The opcode
                             const U32 cmdSeq,                        //!< The command sequence number
                             const Fw::CmdStringArg& sourceFileName,  //!< The source file name
                             const Fw::CmdStringArg& destFileName     //!< The destination file name
    );

    //! Implementation for RemoveDirectory command handler
    //!
    void RemoveDirectory_cmdHandler(const FwOpcodeType opCode,       //!< The opcode
                                    const U32 cmdSeq,                //!< The command sequence number
                                    const Fw::CmdStringArg& dirName  //!< The directory to remove
    );

    //! Implementation for ShellCommand command handler
    //!
    void ShellCommand_cmdHandler(const FwOpcodeType opCode,           //!< The opcode
                                 const U32 cmdSeq,                    //!< The command sequence number
                                 const Fw::CmdStringArg& command,     //!< The shell command string
                                 const Fw::CmdStringArg& logFileName  //!< The name of the log file
    );

    //! Implementation for ConcatFiles command handler
    //! Append 1 file's contents to the end of another.
    void AppendFile_cmdHandler(const FwOpcodeType opCode,       //!< The opcode
                               const U32 cmdSeq,                //!< The command sequence number
                               const Fw::CmdStringArg& source,  //! The name of the file to take content from
                               const Fw::CmdStringArg& target   //! The name of the file to append to
    );

    //! Implementation for FileSize command handler
    //!
    void FileSize_cmdHandler(const FwOpcodeType opCode,        //!< The opcode
                             const U32 cmdSeq,                 //!< The command sequence number
                             const Fw::CmdStringArg& fileName  //!< The file to get the size of
    );

    //! Implementation for ListDirectory command handler
    //!
    void ListDirectory_cmdHandler(const FwOpcodeType opCode,       //!< The opcode
                                  const U32 cmdSeq,                //!< The command sequence number
                                  const Fw::CmdStringArg& dirName  //!< The directory to list
    );

    //! Handler implementation for pingIn
    //!
    void pingIn_handler(const FwIndexType portNum, /*!< The port number*/
                        U32 key                    /*!< Value to return to pinger*/
    );

    //! Handler implementation for schedIn
    //! This handler is called by a Rate Group. It processes
    //! a configurable number of directory entries per rate tick to prevent
    //! event flooding and ensure bounded execution time.
    //!
    void schedIn_handler(const FwIndexType portNum, /*!< The port number*/
                         U32 context                /*!< The call order*/
    );

  private:
    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! A system command with no arguments
    //!
    int systemCall(const Fw::CmdStringArg& command,     //!< The command
                   const Fw::CmdStringArg& logFileName  //!< The log file name
    ) const;

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
    // flooding and ensures bounded execution time by processing one
    // directory entry per rate tick (0.5Hz).

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
};

}  // end namespace Svc

#endif
