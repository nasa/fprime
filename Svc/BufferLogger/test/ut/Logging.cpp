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
      public Logging::Tester
    {

      public:

        CloseFileTester(void) {
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
                Fw::COMMAND_OK
            );
          }

          ASSERT_CMD_RESPONSE_SIZE(n);

        }

        //! Check that files exist
        void checkFilesExist(void) {
          const Fw::EightyCharString& fileName = this->component.m_file.name;
          this->checkFileExists(fileName);
          this->checkHashFileExists(fileName);
        }

      public:

        void test(void) {
          this->component.m_file.baseName = Fw::EightyCharString("CloseFileTester");
          ASSERT_EVENTS_SIZE(0);
          this->sendCloseFileCommands(3);
          this->sendComBuffers(3);
          this->sendCloseFileCommands(3);
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_BL_LogFileClosed_SIZE(1);
          ASSERT_EVENTS_BL_LogFileClosed(0, component.m_file.name.toChar());
          this->checkFilesExist();
        }

    };

    void Tester ::
      CloseFile(void)
    {
      CloseFileTester tester;
      tester.test();
    }

    class SendBuffersTester :
      public Logging::Tester
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
            const char *const baseName //!< The baseName to use
        ) {
          this->sendCmd_BL_OpenFile(0, 0, baseName);
          this->dispatchOne();
          // Create file name
          Fw::EightyCharString currentFileName;
          currentFileName.format(
              "%s%s%s",
              this->component.m_file.prefix.toChar(),
              baseName,
              this->component.m_file.suffix.toChar()
          );
          this->sendBuffers(1);
          // 0th event has already happened (file open)
          for (U32 i = 1; i < numFiles+1; ++i) {
            // File was just created and name set
            ASSERT_EQ(currentFileName, this->component.m_file.name);
            // Write data to the file
            this->sendBuffers(MAX_ENTRIES_PER_FILE-1);
            // File still should have same name
            ASSERT_EQ(currentFileName, this->component.m_file.name);
            // Send more data
            // This should open a new file with the updated counter
            this->sendBuffers(1);
            currentFileName.format(
                "%s%s%d%s",
                this->component.m_file.prefix.toChar(),
                baseName,
                i,
                this->component.m_file.suffix.toChar()
            );
            // Assert file state
            ASSERT_EQ(BufferLogger::File::Mode::OPEN, component.m_file.mode);
            ASSERT_EQ(currentFileName, this->component.m_file.name);
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
            Fw::EightyCharString fileName;
            if (i == 0) {
                fileName.format(
                    "%s%s%s",
                    this->component.m_file.prefix.toChar(),
                    baseName,
                    this->component.m_file.suffix.toChar()
                );
            }
            else {
                fileName.format(
                    "%s%s%d%s",
                    this->component.m_file.prefix.toChar(),
                    baseName,
                    i,
                    this->component.m_file.suffix.toChar()
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

    void Tester ::
      ComIn(void)
    {
      ComInTester tester;
      tester.test(3, "ComIn");
    }

    class BufferSendInTester :
      public SendBuffersTester
    {

      void sendBuffers(const U32 n) {
        this->sendManagedBuffers(n);
      }

    };

    void Tester ::
      BufferSendIn(void)
    {
      BufferSendInTester tester;
      tester.test(3, "BufferSendIn");
    }

    class OnOffTester :
      Logging::Tester
    {
      private:

        //! Send data
        void sendData(void) {
          this->sendComBuffers(MAX_ENTRIES_PER_FILE);
        }

      public:

        //! Set the state
        void setState(
            const BufferLogger::LogState state //!< The state
        ) {
          this->clearHistory();
          this->sendCmd_BL_SetLogging(0, 0, state);
          this->dispatchOne();
          ASSERT_CMD_RESPONSE_SIZE(1);
          ASSERT_CMD_RESPONSE(
              0,
              BufferLogger::OPCODE_BL_SETLOGGING,
              0,
              Fw::COMMAND_OK
          );
        }

        //! Test logging on
        void testLoggingOn(void) {
          this->component.m_file.baseName = Fw::EightyCharString("OnOffTester");
          this->sendData();
          this->setState(BufferLogger::LOGGING_OFF);
          this->checkLogFileIntegrity(
              this->component.m_file.name.toChar(),
              MAX_BYTES_PER_FILE,
              MAX_ENTRIES_PER_FILE
          );
        }

        //! Test logging off
        void testLoggingOff(void) {
          this->setState(BufferLogger::LOGGING_OFF);
          this->sendData();
          ASSERT_EVENTS_SIZE(0);
          this->setState(BufferLogger::LOGGING_ON);
        }

    };

    void Tester ::
      OnOff(void)
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
