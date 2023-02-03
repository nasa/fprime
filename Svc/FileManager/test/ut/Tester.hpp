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

#include "GTestBase.hpp"
#include "Svc/FileManager/FileManager.hpp"

namespace Svc {

  class Tester :
    public FileManagerGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object Tester
      //!
      Tester();

      //! Destroy object Tester
      //!
      ~Tester();

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

      //! Shell command (succeed)
      //!
      void shellCommandSucceed();

      //! Shell command (fail)
      //!
      void shellCommandFail();

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
      static void system(const char *const cmd);

      //! Create a directory
      void createDirectory(
          const char *const dirName
      );

      //! Move a file
      void moveFile(
          const char *const sourceFileName,
          const char *const destFileName
      );

      //! Remove a directory
      void removeDirectory(
          const char *const dirName
      );

      //! Remove a file
      void removeFile(
          const char *const fileName,
          bool ignoreErrors
      );

      //! Perform a shell command
      void shellCommand(
          const char *const command,
          const char *const logFileName
      );

      //! Append 2 files together
      void appendFile(
          const char *const source,
          const char *const target
      );

      //! Assert successful command execution
      void assertSuccess(
          const FwOpcodeType opcode,
          const U32 eventSize = 2 // Starting event + Error or Success msg
      ) const;

      //! Assert file content matches the expected string (up to the given size)
      void assertFileContent(
          const char *const fileName,
          const char *const expectedString,
          const U32 length
      ) const;

      //! Assert failed command execution
      void assertFailure(const FwOpcodeType opcode) const;
      //! Handler for from_pingOut
      //!
      void from_pingOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          U32 key /*!< Value to return to pinger*/
      );
    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      FileManager component;

  };

} // end namespace Svc

#endif
