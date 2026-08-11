// ======================================================================
// \title  FileManager/test/ut/Tester.hpp
// \author bocchino
// \brief  hpp file for FileManager test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "FileManagerGTestBase.hpp"
#include "Svc/FileManager/FileManager.hpp"

namespace Svc {

class FileManagerTester : public FileManagerGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 10;
    //! Construct object FileManagerTester
    //!
    FileManagerTester();

    //! Destroy object FileManagerTester
    //!
    ~FileManagerTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Create directory (succeed)
    //!
    void createDirectorySucceed();

    //! Create directory (fail)
    //!
    void createDirectoryFail();

    //! Move file (succeed)
    //!
    void moveFileSucceed();

    //! Move file (fail)
    //!
    void moveFileFail();

    //! Remove directory (succeed)
    //!
    void removeDirectorySucceed();

    //! Remove directory (fail)
    //!
    void removeDirectoryFail();

    //! Remove file (succeed)
    //!
    void removeFileSucceed();

    //! Remove file (fail)
    //!
    void removeFileFail();

    //! Append file (succeed, append to new file)
    //!
    void appendFileSucceed_newFile();

    //! Append file (succeed, append to existing file)
    //!
    void appendFileSucceed_existingFile();

    //! Append file (fail)
    //!
    void appendFileFail();

    //! File size (succeed)
    //!
    void fileSizeSucceed();

    //! File size (fail)
    //!
    void fileSizeFail();

    //! Calculate CRC (succeed)
    //!
    void calculateCrcSucceed();

    //! List directory (succeed)
    //!
    void listDirectorySucceed();

    //! List directory (fail)
    //!
    void listDirectoryFail();

    //! List directory with subdirectories (enhanced listing)
    //!
    void listDirectoryWithSubdirs();

    //! Generate a data product from a file and check the emitted chunks
    void generateDpSucceed();

    //! Generate a data product from a file that does not exist
    void generateDpFileNotFound();

    //! Generate a data product from an empty file
    void generateDpEmptyFile();

    //! Generate a data product with a chunk size larger than the read buffer
    void generateDpChunkSizeClamped();

    //! Generate a data product from part of a file using begin and end offsets
    void generateDpPartialRange();

    //! Reject a data product request with an invalid offset range
    void generateDpInvalidRange();

    //! Report a data product container allocation failure without failing the command
    void generateDpBufferFailure();

    //! Reject a data product request that arrives while another is in progress
    void generateDpWhileBusy();

    //! Emit all chunks in the command handler instead of pacing them
    void generateDpImmediateMode();

    //! Use a container priority supplied by the command
    void generateDpCustomPriority();

    //! Report a serialization failure without failing the command
    void generateDpSerializationFailure();

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

    //! Perform a system call and assert success
    //!
    static void system(const char* const cmd);

    //! Create a directory
    void createDirectory(const char* const dirName);

    //! Move a file
    void moveFile(const char* const sourceFileName, const char* const destFileName);

    //! Remove a directory
    void removeDirectory(const char* const dirName);

    //! Remove a file
    void removeFile(const char* const fileName, bool ignoreErrors);

    //! Append 2 files together
    void appendFile(const char* const source, const char* const target);

    //! List the contents of a directory
    void listDirectory(const char* const dirName);

    //! Simulate rate group execution for asynchronous directory listing
    //! This method simulates Rate Group 2 (0.5Hz) execution by repeatedly
    //! calling the schedule handler until the directory listing completes.
    //! Each cycle processes one directory entry to test the bounded execution.
    void runRateGroupCycles(const U32 cycles);

    //! Assert successful command execution
    void assertSuccess(const FwOpcodeType opcode,
                       const U32 eventSize = 2  // Starting event + Error or Success msg
    ) const;

    //! Assert file content matches the expected string (up to the given size)
    void assertFileContent(const char* const fileName, const char* const expectedString, const U32 length) const;

    //! Assert failed command execution
    void assertFailure(const FwOpcodeType opcode) const;

    // ----------------------------------------------------------------------
    // Data product test support
    // ----------------------------------------------------------------------

    //! Handler for the data product get port; hands out the local buffer
    Fw::Success::T productGet_handler(FwDpIdType id,      //!< The container ID
                                      FwSizeType size,    //!< The requested size
                                      Fw::Buffer& buffer  //!< The buffer (output)
                                      ) override;

    //! Handler for the data product send port; records the sent container
    void productSend_handler(FwDpIdType id,            //!< The container ID
                             const Fw::Buffer& buffer  //!< The buffer
                             ) override;

    //! Reset the data product bookkeeping between tests
    void resetDpState();

    //! Backing store for the data product container handed to the component
    U8 m_dpContainerData[FW_COM_BUFFER_MAX_SIZE] = {};

    //! Buffer wrapping m_dpContainerData
    Fw::Buffer m_dpContainerBuffer;

    //! Number of containers sent by the component
    U32 m_dpSendCount = 0;

    //! Total number of data bytes observed across sent containers
    FwSizeType m_dpBytesSent = 0;

    //! Whether the get port should fail, for testing the failure path
    bool m_dpGetShouldFail = false;

    //! Whether the get port should hand out a buffer too small for a chunk
    bool m_dpGetUndersizedBuffer = false;

    //! Priority carried by the most recently sent container
    FwDpPriorityType m_dpLastPriority = 0;

    //! Total data-product payload bytes observed across all sent containers,
    //! i.e. the sum of each container's data size (header + data records)
    FwSizeType m_dpPayloadBytes = 0;

    //! Smallest container data size seen, used to confirm every chunk carried
    //! a real (non-empty) header-plus-data payload
    FwSizeType m_dpMinPayloadBytes = 0;

    //! Handler for from_pingOut
    //!
    void from_pingOut_handler(const FwIndexType portNum, /*!< The port number*/
                              U32 key                    /*!< Value to return to pinger*/
                              ) override;

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    FileManager component;
};

}  // end namespace Svc

#endif
