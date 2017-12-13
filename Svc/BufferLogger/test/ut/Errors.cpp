// ====================================================================== 
// \title  Errors.cpp
// \author bocchino
// \brief  Implementation for Buffer Logger error tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#include <stdlib.h>

#include "Errors.hpp"

namespace ASTERIA {

  namespace Errors {

    void Tester ::
      LogFileOpen(void)
    {

      // Set test time
      Fw::Time testTime(1, 345678, 2);
      this->setTestTime(testTime);

      // Remove buf directory
      (void) system("rm -rf buf");

      // Check initial state
      ASSERT_EQ(BufferLogger::File::Mode::CLOSED, this->component.file.mode);
      ASSERT_EVENTS_SIZE(0);

      // Send data
      this->sendComBuffers(3);

      // Check events
      // We expect only one because of the throttle
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_LogFileOpenError_SIZE(1);
      ASSERT_EVENTS_LogFileOpenError(
        0,
        Os::File::DOESNT_EXIST,
        this->component.file.name.toChar()
      );

      // Create buf directory and try again
      (void) system("mkdir buf");
      ASSERT_EQ(BufferLogger::File::Mode::CLOSED, this->component.file.mode);

      // Send data
      this->sendComBuffers(3);

      // Check events
      // We expect only one because of the throttle
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_LogFileOpenError_SIZE(1);
      this->component.file.close();

      // Remove buf directory and try again
      (void) system("rm -rf buf");
      ASSERT_EQ(BufferLogger::File::Mode::CLOSED, this->component.file.mode);

      // Send data
      this->sendComBuffers(3);

      // Check events
      // We expect only one more because of the throttle
      ASSERT_EVENTS_SIZE(2);
      ASSERT_EVENTS_LogFileOpenError_SIZE(2);

    }

    void Tester ::
      LogFileWrite(void)
    {

      ASSERT_EQ(BufferLogger::File::Mode::CLOSED, this->component.file.mode);
      ASSERT_EVENTS_SIZE(0);

      Fw::Time time(1, 345678, 2);
      setTestTime(time);

      // Send data
      this->sendComBuffers(1);

      // Force close the file
      this->component.file.asteriaFile.close();

      // Send data
      this->sendComBuffers(1);

      // Construct file name
      String fileName;
      this->component.file.formatName(fileName, time);

      // Check events
      // We should see one event because write errors are throttled
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_LogFileWriteError_SIZE(1);
      ASSERT_EVENTS_LogFileWriteError(
          0,
          Os::File::NOT_OPENED, // errornum
          sizeof(SIZE_TYPE), // bytesWritten
          sizeof(SIZE_TYPE), // bytesAttempted
          fileName.toChar() // file
      );

      // Make comlogger open a new file:
      this->component.file.mode = BufferLogger::File::Mode::CLOSED;
      this->component.file.open();
      
      // Try to write and make sure it succeeds
      // Send data
      this->sendComBuffers(3);

      // Expect no new errors
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_LogFileWriteError_SIZE(1);

      // Force close the file from underneath the component
      component.file.asteriaFile.close();

      // Send data
      this->sendComBuffers(3);

      // Check events
      // We should see one event because write errors are throttled
      ASSERT_EVENTS_SIZE(2);
      ASSERT_EVENTS_LogFileWriteError_SIZE(2);
      ASSERT_EVENTS_LogFileWriteError(
          1,
          Os::File::NOT_OPENED,
          sizeof(SIZE_TYPE),
          sizeof(SIZE_TYPE),
          fileName.toChar()
      );

    }

    void Tester ::
      LogFileValidation(void)
    {
      // Set the test time
      Fw::Time time(1, 345678, 2);
      this->setTestTime(time);
      // Send data
      this->sendComBuffers(1);
      // Remove permission to buf directory
      (void) system("chmod -w buf");
      // Send close file command
      this->sendCmd_CloseFile(0, 0);
      this->dispatchOne();
      // Check events
      ASSERT_EVENTS_SIZE(2);
      String fileName;
      this->component.file.formatName(fileName, time);
      ASSERT_EVENTS_LogFileClosed(
          0,
          fileName.toChar()
      );
      ValidatedFile validatedFile(fileName.toChar());
      const String& hashFileName = validatedFile.getHashFileName();
      ASSERT_EVENTS_LogFileValidationError(
          0,
          hashFileName.toChar(),
          Os::ValidateFile::VALIDATION_FILE_NO_PERMISSION
      );
      // Restore permission
      (void) system("chmod +w buf");
    }

    class NonVolatileReadTester :
      public Errors::Tester 
    {

      public:

        void test(void) {
          ASSERT_EVENTS_SIZE(1);
          ASSERT_EVENTS_NonVolatileReadError_SIZE(1);
        }

    };

    void Tester ::
      NonVolatileRead(void)
    {
      // Create invalid (empty) state.bin
      (void) system("touch state.bin");
      // Provoke and check the error
      NonVolatileReadTester tester;
      tester.test();
      // Clean up
      (void) system("rm state.bin");
    }

    void Tester ::
      NonVolatileWrite(void)
    {
      // Create state.bin
      (void) system("touch state.bin");
      // Make state.bin not writable
      (void) system("chmod -w state.bin");
      // Try to write it
      this->sendCmd_SetSaveState(0, 0, OnOff::ON);
      this->dispatchAll();
      // CHeck responses
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          BufferLogger::OPCODE_SETSAVESTATE,
          0,
          Fw::COMMAND_EXECUTION_ERROR
      );
      ASSERT_EVENTS_SIZE(2);
      ASSERT_EVENTS_NonVolatileWriteError_SIZE(1);
      ASSERT_EVENTS_SetState_SIZE(1);
      ASSERT_EVENTS_SetState(0, OnOff::ON);
      // Clean up
      (void) system("chmod +w state.bin");
      (void) system("rm state.bin");
    }

  }

}
