// ======================================================================
// \title  Logging.cpp
// \author bocchino, mereweth
// \brief  Implementation for Buffer Logger logging tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Logging.hpp"
#include "Os/FileSystem.hpp"

namespace Svc {

  namespace Logging {

    class CloseFileTester :
      public Logging::BufferLoggerTester
    {

      public:

        CloseFileTester() {
          Fw::Time testTime = this->generateTestTime(0);
          this->setTestTime(testTime);
        }

      private:

        //! Send close file commands
        void sendCloseFileCommands(const U32 n) {

          this->clearHistory();

          for (U32 i = 0; i < n; ++i) {
            this->sendCmd_BL_CloseFile(0, i);
            this->dispatchOne();
            ASSERT_CMD_RESPONSE(
                i,
                BufferLogger::OPCODE_BL_CLOSEFILE,
                i,
                Fw::CmdResponse::OK
            );
          }

          ASSERT_CMD_RESPONSE_SIZE(n);

        }

        //! Check that files exist
        void checkFilesExist() {
          const Fw::String& fileName = this->component.m_file.m_name;
          this->checkFileExists(fileName);
          this->checkHashFileExists(fileName);
        }

      public:

        void test() {
          this->component.m_file.m_baseName = Fw::String("CloseFileTester");
          ASSERT_EVENTS_SIZE(0);
          this->sendCloseFileCommands(3);
          this->sendComBuffers(3);
          this->sendCloseFileCommands(3);
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_BL_LogFileClosed_SIZE(1);
          ASSERT_EVENTS_BL_LogFileClosed(0, component.m_file.m_name.toChar());
          this->checkFilesExist();
        }

    };

    void BufferLoggerTester ::
      CloseFile()
    {
      CloseFileTester tester;
      tester.test();
    }

    class SendBuffersTester :
      public Logging::BufferLoggerTester
    {

      protected:

        //! Send buffers
        virtual void sendBuffers(
            const U32 n //!< The number of buffers to send
        ) = 0;

      public:

        //! Run a test
        void test(
            const U32 numFiles, //!< The number of files to create
            const Fw::CmdStringArg& baseName //!< The baseName to use
        ) {
          this->sendCmd_BL_OpenFile(0, 0, baseName);
          this->dispatchOne();
          // Create file name
          Fw::String currentFileName;
          currentFileName.format(
              "%s%s%s",
              this->component.m_file.m_prefix.toChar(),
              baseName.toChar(),
              this->component.m_file.m_suffix.toChar()
          );
          this->sendBuffers(1);
          // 0th event has already happened (file open)
          for (U32 i = 1; i < numFiles+1; ++i) {
            // File was just created and name set
            ASSERT_EQ(currentFileName, this->component.m_file.m_name);
            // Write data to the file
            this->sendBuffers(MAX_ENTRIES_PER_FILE-1);
            // File still should have same name
            ASSERT_EQ(currentFileName, this->component.m_file.m_name);
            // Send more data
            // This should open a new file with the updated counter
            this->sendBuffers(1);
            currentFileName.format(
                "%s%s%d%s",
                this->component.m_file.m_prefix.toChar(),
                baseName.toChar(),
                i,
                this->component.m_file.m_suffix.toChar()
            );
            // Assert file state
            ASSERT_EQ(BufferLogger::File::Mode::OPEN, component.m_file.m_mode);
            ASSERT_EQ(currentFileName, this->component.m_file.m_name);
            // Assert events
            ASSERT_EVENTS_SIZE(i);
            ASSERT_EVENTS_BL_LogFileClosed_SIZE(i);
          }

          // Close the last file
          this->sendCmd_BL_CloseFile(0, 0);
          this->dispatchOne();

          // Check files
          for (U32 i = 0; i < numFiles; ++i) {
            // Create file name
            Fw::String fileName;
            if (i == 0) {
                fileName.format(
                    "%s%s%s",
                    this->component.m_file.m_prefix.toChar(),
                    baseName.toChar(),
                    this->component.m_file.m_suffix.toChar()
                );
            }
            else {
                fileName.format(
                    "%s%s%d%s",
                    this->component.m_file.m_prefix.toChar(),
                    baseName.toChar(),
                    i,
                    this->component.m_file.m_suffix.toChar()
                );
            }
            // Check events
            ASSERT_EVENTS_BL_LogFileClosed(i, fileName.toChar());
            // Check file integrity
            this->checkLogFileIntegrity(
                fileName.toChar(),
                MAX_BYTES_PER_FILE,
                MAX_ENTRIES_PER_FILE
            );
            // Check validation
            this->checkFileValidation(fileName.toChar());
          }

        }

    };

    class ComInTester :
      public SendBuffersTester
    {

      void sendBuffers(const U32 n) {
        this->sendComBuffers(n);
      }

    };

    void BufferLoggerTester ::
      ComIn()
    {
      ComInTester tester;
      tester.test(3, Fw::CmdStringArg("ComIn"));
    }

    class BufferSendInTester :
      public SendBuffersTester
    {

      void sendBuffers(const U32 n) {
        this->sendManagedBuffers(n);
      }

    };

    void BufferLoggerTester ::
      BufferSendIn()
    {
      BufferSendInTester tester;
      tester.test(3, Fw::CmdStringArg("BufferSendIn"));
    }

    class OnOffTester :
      Logging::BufferLoggerTester
    {
      private:

        //! Send data
        void sendData() {
          this->sendComBuffers(MAX_ENTRIES_PER_FILE);
        }

      public:

        //! Set the state
        void setState(
            const BufferLogger_LogState state //!< The state
        ) {
          this->clearHistory();
          this->sendCmd_BL_SetLogging(0, 0, state);
          this->dispatchOne();
          ASSERT_CMD_RESPONSE_SIZE(1);
          ASSERT_CMD_RESPONSE(
              0,
              BufferLogger::OPCODE_BL_SETLOGGING,
              0,
              Fw::CmdResponse::OK
          );
        }

        //! Test logging on
        void testLoggingOn() {
          this->component.m_file.m_baseName = Fw::String("OnOffTester");
          this->sendData();
          this->setState(BufferLogger_LogState::LOGGING_OFF);
          this->checkLogFileIntegrity(
              this->component.m_file.m_name.toChar(),
              MAX_BYTES_PER_FILE,
              MAX_ENTRIES_PER_FILE
          );
        }

        //! Test logging off
        void testLoggingOff() {
          this->setState(BufferLogger_LogState::LOGGING_OFF);
          this->sendData();
          ASSERT_EVENTS_SIZE(0);
          this->setState(BufferLogger_LogState::LOGGING_ON);
        }

    };

    void BufferLoggerTester ::
      OnOff()
    {
      {
          OnOffTester tester;
          tester.testLoggingOn();
      }
      {
          OnOffTester tester;
          tester.testLoggingOff();
      }
    }

  }

}
