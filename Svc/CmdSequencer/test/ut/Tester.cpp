// ======================================================================
// \title  CmdSequencer.cpp
// \author Canham/Bocchino
// \brief  CmdSequencer test implementation
//
// \copyright
// Copyright (C) 2009-2018 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include "Fw/Com/ComPacket.hpp"
#include "Os/Stubs/FileStubs.hpp"
#include "Svc/CmdSequencer/test/ut/CommandBuffers.hpp"
#include "Svc/CmdSequencer/test/ut/SequenceFiles/FPrime/FPrime.hpp"
#include "Svc/CmdSequencer/test/ut/Tester.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction and destruction
  // ----------------------------------------------------------------------

  Tester ::
    Tester(const SequenceFiles::File::Format::t format) :
      CmdSequencerGTestBase("Tester", MAX_HISTORY_SIZE),
      component("CmdSequencer"),
      format(format),
      sequences(this->component)
  {
    this->initComponents();
    this->connectPorts();
    this->setComponentSequenceFormat();
    this->component.allocateBuffer(
        ALLOCATOR_ID,
        this->mallocator,
        BUFFER_SIZE
    );
    this->component.preamble();
    this->component.setTimeout(TIMEOUT);
    this->component.regCommands();
  }

  Tester ::
    ~Tester()
  {
    this->component.deallocateBuffer(this->mallocator);
  }

  // ----------------------------------------------------------------------
  // Handlers for typed from ports
  // ----------------------------------------------------------------------

  void Tester ::
    from_seqDone_handler(
      const NATIVE_INT_TYPE portNum,
      FwOpcodeType opCode,
      U32 cmdSeq,
      const Fw::CmdResponse& response
    )
  {
    this->pushFromPortEntry_seqDone(opCode, cmdSeq, response);
  }

  void Tester ::
    from_comCmdOut_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    this->pushFromPortEntry_comCmdOut(data, context);
  }

  void Tester ::
    from_pingOut_handler(
      const NATIVE_INT_TYPE portNum,
      U32 key
    )
  {
    this->pushFromPortEntry_pingOut(key);
  }

  // ----------------------------------------------------------------------
  // Virtual function interface
  // ----------------------------------------------------------------------

  void Tester ::
    executeCommandsAuto(
        const char *const fileName,
        const U32 numCommands,
        const U32 bound,
        const CmdExecMode::t mode
    )
  {
    ASSERT_TRUE(false) << "executeCommandsAuto is not implemented\n";
  }

  void Tester ::
    executeCommandsError(
        const char *const fileName,
        const U32 numCommands
    )
  {
    ASSERT_TRUE(false) << "executeCommandsError is not implemented\n";
  }

  void Tester ::
    executeCommandsManual(
        const char *const fileName,
        const U32 numCommands
    )
  {
    ASSERT_TRUE(false) << "executeCommandsManual is not implemented\n";
  }

  // ----------------------------------------------------------------------
  // Tests parameterized by file type
  // ----------------------------------------------------------------------

  void Tester ::
    parameterizedAutoByCommand(
        SequenceFiles::File& file,
        const U32 numCommands,
        const U32 bound
    )
  {
    ASSERT_TRUE(false) << "parameterizedAutoByCommand is not implemented\n";
  }

  void Tester ::
    parameterizedCancel(
        SequenceFiles::File& file,
        const U32 numCommands,
        const U32 bound
    )
  {

    REQUIREMENT("ISF-CMDS-005");

    // Set the time
    Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    const char *const  fileName = file.getName().toChar();
    file.write();
    // Validate the file
    this->validateFile(0, fileName);
    // Run the sequence
    this->runSequence(0, fileName);
    // Execute commands
    this->executeCommandsAuto(
        fileName,
        numCommands,
        bound,
        CmdExecMode::NO_NEW_SEQUENCE
    );
    // Cancel sequence
    this->cancelSequence(100, fileName);

  }

  void Tester ::
    parameterizedFailedCommands(
        SequenceFiles::File& file,
        const U32 numCommands
    )
  {
    REQUIREMENT("ISF-CMDS-004");

    // Set the time
    Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    const char *const fileName = file.getName().toChar();
    file.write();
    // Validate the file
    this->validateFile(0, fileName);
    // Start the sequence
    this->runSequence(0, fileName);
    // Execute commands
    this->executeCommandsError(fileName, numCommands);
    // Check to see if the component has cleaned up
    ASSERT_EQ(
        CmdSequencerComponentImpl::STOPPED,
        this->component.m_runMode
    );
    ASSERT_EQ(
        CmdSequencerComponentImpl::Timer::CLEAR,
        this->component.m_cmdTimeoutTimer.m_state
    );
  }

  void Tester ::
    parameterizedFileErrors(SequenceFiles::File& file)
  {
    this->parameterizedFileOpenErrors(file);
    this->parameterizedHeaderReadErrors(file);
    this->parameterizedDataReadErrors(file);
  }

  void Tester ::
    parameterizedFileOpenErrors(SequenceFiles::File& file)
  {
    // Set the time
    Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    const char *const fileName = file.getName().toChar();
    file.write();
    // Get error info
    SequenceFiles::File::ErrorInfo errorInfo;
    file.getErrorInfo(errorInfo);
    const char *const errorFileName = errorInfo.open.fileName.toChar();
    // Enable open interceptor
    this->openInterceptor.enable();
    // DOESNT_EXIST
    {
      this->openInterceptor.fileStatus = Os::File::DOESNT_EXIST;
      // Validate the file
      this->sendCmd_CS_VALIDATE(0, 0, fileName);
      this->clearAndDispatch();
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileNotFound_SIZE(1);
      ASSERT_EVENTS_CS_FileNotFound(0, errorFileName);
    }
    // NO_PERMISSION
    {
      this->openInterceptor.fileStatus = Os::File::NO_PERMISSION;
      // Validate the file
      const U32 validateCmdSeq = 14;
      this->sendCmd_CS_VALIDATE(0, validateCmdSeq, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          validateCmdSeq,
          Fw::CmdResponse::EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileReadError_SIZE(1);
      ASSERT_EVENTS_CS_FileReadError(0, errorFileName);
    }
    // Disable open interceptor
    this->openInterceptor.disable();
  }

  void Tester ::
    parameterizedHeaderReadErrors(SequenceFiles::File& file)
  {
    // Set the time
    Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    const char *const fileName = file.getName().toChar();
    file.write();
    // Get error info
    SequenceFiles::File::ErrorInfo errorInfo;
    file.getErrorInfo(errorInfo);
    const char *const errorFileName = errorInfo.headerRead.fileName.toChar();
    // Enable read interceptor
    this->readInterceptor.enable();
    // Read error reading header
    {
      // Set up fault injection state
      this->readInterceptor.waitCount = errorInfo.headerRead.waitCount;
      this->readInterceptor.fileStatus = Os::File::NO_SPACE;
      this->readInterceptor.errorType = Interceptors::Read::ErrorType::READ;
      Os::setLastError(Os::File::NO_SPACE);
      // Validate file
      const U32 validateCmdSeq = 14;
      this->sendCmd_CS_VALIDATE(0, validateCmdSeq, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          validateCmdSeq,
          Fw::CmdResponse::EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid(
          0,
          errorFileName,
          CmdSequencer_FileReadStage::READ_HEADER,
          Os::File::NO_SPACE
      );
    }
    // Disable read interceptor
    this->readInterceptor.disable();
  }

  void Tester ::
    parameterizedDataReadErrors(SequenceFiles::File& file)
  {
    // Set the time
    Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    const char *const fileName = file.getName().toChar();
    file.write();
    // Get error info
    SequenceFiles::File::ErrorInfo errorInfo;
    file.getErrorInfo(errorInfo);
    const char *const errorFileName = errorInfo.dataRead.fileName.toChar();
    // Enable read interceptor
    this->readInterceptor.enable();
    // Read error reading data
    {
      // Set up fault injection state
      this->readInterceptor.waitCount = errorInfo.dataRead.waitCount;
      this->readInterceptor.fileStatus = Os::File::NO_SPACE;
      this->readInterceptor.errorType = Interceptors::Read::ErrorType::READ;
      Os::setLastError(Os::File::NO_SPACE);
      // Validate file
      const U32 validateCmdSeq = 14;
      this->sendCmd_CS_VALIDATE(0, validateCmdSeq, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          validateCmdSeq,
          Fw::CmdResponse::EXECUTION_ERROR
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid(
          0,
          errorFileName,
          CmdSequencer_FileReadStage::READ_SEQ_DATA,
          Os::File::NO_SPACE
      );
    }
    // Size error reading data
    {
      // Set up fault injection state
      this->readInterceptor.waitCount = errorInfo.dataRead.waitCount;
      this->readInterceptor.fileStatus = Os::File::OP_OK;
      this->readInterceptor.errorType = Interceptors::Read::ErrorType::SIZE;
      this->readInterceptor.size = 2;
      // Validate file
      const U32 validateCmdSeq = 14;
      this->sendCmd_CS_VALIDATE(0, validateCmdSeq, fileName);
      this->clearAndDispatch();
      // Assert command response
      ASSERT_CMD_RESPONSE_SIZE(1);
      ASSERT_CMD_RESPONSE(
          0,
          CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
          validateCmdSeq,
          Fw::CmdResponse(Fw::CmdResponse::EXECUTION_ERROR)
      );
      // Assert events
      ASSERT_EVENTS_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid_SIZE(1);
      ASSERT_EVENTS_CS_FileInvalid(
          0,
          errorFileName,
          CmdSequencer_FileReadStage::READ_SEQ_DATA_SIZE,
          2
      );
    }
    // Disable read interceptor
    this->readInterceptor.disable();
  }

  void Tester ::
    parameterizedNeverLoaded()
  {
    // Try to run a sequence
    Fw::String fArg("");
    this->invoke_to_seqRunIn(0, fArg);
    this->clearAndDispatch();
    // Assert seqDone response
    ASSERT_from_seqDone_SIZE(1);
    ASSERT_from_seqDone(0U, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse(Fw::CmdResponse::EXECUTION_ERROR)));
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_NoSequenceActive_SIZE(1);
    // Assert telemetry
    ASSERT_TLM_SIZE(1);
    ASSERT_TLM_CS_Errors(0, 1);
  }

  void Tester ::
    parameterizedSequenceTimeout(SequenceFiles::File& file)
  {

    REQUIREMENT("ISF-CMDS-006");

    // Set the time
    Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    const char *const fileName = file.getName().toChar();
    file.write();
    // Validate the file
    this->validateFile(0, fileName);
    // Assert that timer is clear
    ASSERT_EQ(
        CmdSequencerComponentImpl::Timer::CLEAR,
        this->component.m_cmdTimeoutTimer.m_state
    );
    // Run the sequence
    this->runSequence(0, fileName);
    // Check command buffers
    Fw::ComBuffer comBuff;
    CommandBuffers::create(comBuff, 0, 1);
    ASSERT_from_comCmdOut_SIZE(1);
    ASSERT_from_comCmdOut(0, comBuff, 0U);
    // Assert that timer is set
    ASSERT_EQ(
        CmdSequencerComponentImpl::Timer::SET,
        this->component.m_cmdTimeoutTimer.m_state
    );
    // Set the test time to be after the timeout
    testTime.set(TB_WORKSTATION_TIME, 2 * TIMEOUT, 1);
    this->setTestTime(testTime);
    // Call the schedule port
    this->invoke_to_schedIn(0, 0);
    this->clearAndDispatch();
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_SequenceTimeout(0, fileName, 0);
    // Verify that the sequencer is idle again
    ASSERT_EQ(
        CmdSequencerComponentImpl::STOPPED,
        this->component.m_runMode
    );
    ASSERT_EQ(
        CmdSequencerComponentImpl::Timer::CLEAR,
        this->component.m_cmdTimeoutTimer.m_state
    );
    ASSERT_EQ(
        CmdSequencerComponentImpl::Timer::CLEAR,
        this->component.m_cmdTimer.m_state
    );
    ASSERT_EQ(0U, this->component.m_executedCount);
    // Assert command response on seqDone
    ASSERT_from_seqDone_SIZE(1);
    ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse(Fw::CmdResponse::EXECUTION_ERROR)));

  }

  void Tester ::
    parameterizedUnexpectedCommandResponse(
        SequenceFiles::File& file,
        const U32 numCommands,
        const U32 bound
    )
  {
    // Run the sequence
    this->parameterizedAutoByCommand(file, numCommands, bound);
    // Send unexpected command response
    this->invoke_to_cmdResponseIn(0, 0x10, 0, Fw::CmdResponse(Fw::CmdResponse::OK));
    this->clearAndDispatch();
    // Check events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_UnexpectedCompletion_SIZE(1);
    ASSERT_EVENTS_CS_UnexpectedCompletion(0, 0x10);
  }

  void Tester ::
    parameterizedValidate(SequenceFiles::File& file)
  {
    // Set the time
    Fw::Time testTime(TB_WORKSTATION_TIME, 1, 1);
    this->setTestTime(testTime);
    // Write the file
    const char *const fileName = file.getName().toChar();
    file.write();
    // Validate the file
    this->validateFile(0, fileName);
  }

  // ----------------------------------------------------------------------
  // Instance helper methods
  // ----------------------------------------------------------------------

  void Tester ::
    connectPorts()
  {

    // LogText
    this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));

    // cmdIn
    this->connect_to_cmdIn(0, this->component.get_cmdIn_InputPort(0));

    // cmdRegOut
    this->component.set_cmdRegOut_OutputPort(0,
        this->get_from_cmdRegOut(0));

    // cmdResponseIn
    this->connect_to_cmdResponseIn(
        0,
        this->component.get_cmdResponseIn_InputPort(0)
    );

    // cmdResponseOut
    this->component.set_cmdResponseOut_OutputPort(0,
        this->get_from_cmdResponseOut(0));

    // comCmdOut
    this->component.set_comCmdOut_OutputPort(
        0,
        this->get_from_comCmdOut(0)
    );

    // logOut
    this->component.set_logOut_OutputPort(0, this->get_from_logOut(0));

    // pingIn
    this->connect_to_pingIn(
        0,
        this->component.get_pingIn_InputPort(0)
    );

    // pingOut
    this->component.set_pingOut_OutputPort(
      0,
      this->get_from_pingOut(0)
    );

    // schedIn
    this->connect_to_schedIn(0, this->component.get_schedIn_InputPort(0));

    // seqDone
    this->component.set_seqDone_OutputPort(
        0,
        this->get_from_seqDone(0)
    );

    // seqRunIn
    this->connect_to_seqRunIn(
        0,
        this->component.get_seqRunIn_InputPort(0)
    );

    // timeCaller
    this->component.set_timeCaller_OutputPort(0,
        this->get_from_timeCaller(0));

    // tlmOut
    this->component.set_tlmOut_OutputPort(0, this->get_from_tlmOut(0));

  }

#if VERBOSE
  void Tester ::
    textLogIn(
        const FwEventIdType id, //!< The event ID
        Fw::Time& timeTag, //!< The time
        const Fw::LogSeverity severity, //!< The severity
        const Fw::TextLogString& text //!< The event string
    )
  {
    TextLogEntry e = { id, timeTag, severity, text };
    printTextLogHistoryEntry(e, stdout);
  }
#endif

  void Tester ::
    initComponents()
  {
    this->init();
    this->component.init(QUEUE_DEPTH, INSTANCE);
  }

  void Tester ::
    setComponentSequenceFormat()
  {
    switch (this->format) {
      case SequenceFiles::File::Format::F_PRIME:
        // Use default format
        break;
      case SequenceFiles::File::Format::AMPCS:
        this->component.setSequenceFormat(this->sequences.ampcsSequence);
        break;
      default:
        ASSERT_TRUE(0) << "Invalid sequence format " << format << "\n";
        break;
    }
  }

  void Tester ::
    clearAndDispatch()
  {
    this->clearHistory();
    ASSERT_EQ(
        Fw::QueuedComponentBase::MSG_DISPATCH_OK,
        this->component.doDispatch()
    );
  }

  void Tester ::
    validateFile(const U32 cmdSeq, const char* const fileName)
  {
    // Validate the file
    this->sendCmd_CS_VALIDATE(0, cmdSeq, fileName);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
        cmdSeq,
        Fw::CmdResponse::OK
    );
    // Assert events
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_CS_SequenceValid(0, fileName);
    ASSERT_EVENTS_CS_SequenceLoaded(0, fileName);
  }

  void Tester ::
    loadSequence(const char* const fileName)
  {
    // Invoke the port
    Fw::String fArg(fileName);
    this->clearHistory();
    this->component.loadSequence(fileName);
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_SequenceLoaded(0, fileName);
  }

  void Tester ::
    runSequence(const U32 cmdSeq, const char* const fileName)
  {
    // Send run command
    this->sendCmd_CS_RUN(0, cmdSeq, fileName,Svc::CmdSequencer_BlockState::NO_BLOCK);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        CmdSequencerComponentBase::OPCODE_CS_RUN,
        cmdSeq,
        Fw::CmdResponse::OK
    );
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_SequenceLoaded(0, fileName);
  }

  void Tester ::
    runSequenceByPortCall(const char* const fileName)
  {
    // Invoke the port
    Fw::String fArg(fileName);
    this->invoke_to_seqRunIn(0, fArg);
    this->clearAndDispatch();
    // Assert no command response
    ASSERT_CMD_RESPONSE_SIZE(0);
    // Assert events
    ASSERT_EVENTS_SIZE(2);
    ASSERT_EVENTS_CS_SequenceLoaded(0, fileName);
    ASSERT_EVENTS_CS_PortSequenceStarted(0, fileName);
  }

  void Tester ::
    runLoadedSequence()
  {
    // Invoke the port
    Fw::String fArg("");
    this->invoke_to_seqRunIn(0, fArg);
    this->clearAndDispatch();
    // Assert no command response
    ASSERT_CMD_RESPONSE_SIZE(0);
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    const Fw::LogStringArg& fileName =
      this->component.m_sequence->getLogFileName();
    ASSERT_EVENTS_CS_PortSequenceStarted(0, fileName.toChar());
  }

  void Tester ::
    startNewSequence(const char *const fileName)
  {
    // Start the sequence
    this->sendCmd_CS_RUN(0, 0, fileName,Svc::CmdSequencer_BlockState::NO_BLOCK);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        CmdSequencerComponentBase::OPCODE_CS_RUN,
        0,
        Fw::CmdResponse::EXECUTION_ERROR
    );
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_InvalidMode_SIZE(1);
    // Validate the file
    this->sendCmd_CS_VALIDATE(0, 0, fileName);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        CmdSequencerComponentBase::OPCODE_CS_VALIDATE,
        0,
        Fw::CmdResponse::EXECUTION_ERROR
    );
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_InvalidMode_SIZE(1);
    // Invoke sequence port
    Fw::String fArg(fileName);
    this->invoke_to_seqRunIn(0, fArg);
    this->clearAndDispatch();
    // Assert response on seqDone
    ASSERT_from_seqDone_SIZE(1);
    ASSERT_from_seqDone(0, 0U, 0U, Fw::CmdResponse(Fw::CmdResponse::EXECUTION_ERROR));
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_InvalidMode_SIZE(1);
  }

  void Tester ::
    startSequence(const U32 cmdSeq, const char* const fileName)
  {
    // Send start command
    this->sendCmd_CS_START(0, cmdSeq);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        CmdSequencerComponentBase::OPCODE_CS_START,
        cmdSeq,
        Fw::CmdResponse::OK
    );
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_CmdStarted(0, fileName);
  }

  void Tester ::
    cancelSequence(const U32 cmdSeq, const char* const fileName)
  {
    // Send cancel command
    this->sendCmd_CS_CANCEL(0, cmdSeq);
    this->clearAndDispatch();
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_SequenceCanceled(0, fileName);
    // Verify state
    ASSERT_EQ(
        CmdSequencerComponentImpl::STOPPED,
        this->component.m_runMode
    );
    ASSERT_EQ(
        CmdSequencerComponentImpl::Timer::CLEAR,
        this->component.m_cmdTimeoutTimer.m_state
    );
  }

  void Tester ::
    goToManualMode(const U32 cmdSeq)
  {
    // Send manual command
    this->sendCmd_CS_MANUAL(0, cmdSeq);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        CmdSequencerComponentBase::OPCODE_CS_MANUAL,
        cmdSeq,
        Fw::CmdResponse(Fw::CmdResponse::OK)
    );
    // Assert events
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_CS_ModeSwitched(
        0,
        CmdSequencer_SeqMode::STEP
    );
  }

  void Tester ::
    goToAutoMode(const U32 cmdSeq)
  {
    // Send auto command
    this->sendCmd_CS_AUTO(0, cmdSeq);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        CmdSequencerComponentBase::OPCODE_CS_AUTO,
        cmdSeq,
        Fw::CmdResponse(Fw::CmdResponse::OK)
    );
  }

  void Tester ::
    stepSequence(const U32 cmdSeq)
  {
    // Send step command
    this->sendCmd_CS_STEP(0, cmdSeq);
    this->clearAndDispatch();
    // Assert command response
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(
        0,
        CmdSequencerComponentBase::OPCODE_CS_STEP,
        cmdSeq,
        Fw::CmdResponse(Fw::CmdResponse::OK)
    );
  }

}
