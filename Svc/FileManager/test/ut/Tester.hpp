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
      Tester(void);

      //! Destroy object Tester
      //!
      ~Tester(void);

    public:

      // ---------------------------------------------------------------------- 
      // Tests
      // ---------------------------------------------------------------------- 

      //! Create directory (succeed)
      //!
      void createDirectorySucceed(void);

      //! Create directory (fail)
      //!
      void createDirectoryFail(void);

      //! Move file (succeed)
      //!
      void moveFileSucceed(void);

      //! Move file (fail)
      //!
      void moveFileFail(void);

      //! Remove directory (succeed)
      //!
      void removeDirectorySucceed(void);

      //! Remove directory (fail)
      //!
      void removeDirectoryFail(void);

      //! Remove file (succeed)
      //!
      void removeFileSucceed(void);

      //! Remove file (fail)
      //!
      void removeFileFail(void);

      //! Shell command (succeed)
      //!
      void shellCommandSucceed(void);

      //! Shell command (fail)
      //!
      void shellCommandFail(void);

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts(void);

      //! Initialize components
      //!
      void initComponents(void);

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
          const char *const fileName
      );

      //! Perform a shell command
      void shellCommand(
          const char *const command,
          const char *const logFileName
      );

      //! Assert successful command execution
      void assertSuccess(
          const FwOpcodeType opcode,
          const U32 eventSize = 0
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
