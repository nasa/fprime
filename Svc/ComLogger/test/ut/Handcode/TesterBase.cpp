// ======================================================================
// \title  ComLogger/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for ComLogger component test harness base class
//
// \copyright
// Copyright 2009-2016, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <stdlib.h>
#include <string.h>
#include "TesterBase.hpp"

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ComLoggerTesterBase ::
    ComLoggerTesterBase(
#if FW_OBJECT_NAMES == 1
        const char *const compName,
        const U32 maxHistorySize
#else
        const U32 maxHistorySize
#endif
    ) :
#if FW_OBJECT_NAMES == 1
      Fw::PassiveComponentBase(compName)
#else
      Fw::PassiveComponentBase()
#endif
  {
    // Initialize command history
    this->cmdResponseHistory = new History<CmdResponse>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_FileOpenError =
      new History<EventEntry_FileOpenError>(maxHistorySize);
    this->eventHistory_FileWriteError =
      new History<EventEntry_FileWriteError>(maxHistorySize);
    this->eventHistory_FileValidationError =
      new History<EventEntry_FileValidationError>(maxHistorySize);
    this->eventHistory_FileClosed =
      new History<EventEntry_FileClosed>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  ComLoggerTesterBase ::
    ~ComLoggerTesterBase(void) 
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_FileOpenError;
    delete this->eventHistory_FileWriteError;
    delete this->eventHistory_FileValidationError;
    delete this->eventHistory_FileClosed;
  }

  void ComLoggerTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port timeCaller

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_timeCaller();
        ++_port
    ) {

      this->m_from_timeCaller[_port].init();
      this->m_from_timeCaller[_port].addCallComp(
          this,
          from_timeCaller_static
      );
      this->m_from_timeCaller[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_timeCaller[%d]",
          this->m_objName,
          _port
      );
      this->m_from_timeCaller[_port].setObjName(_portName);
#endif

    }

    // Attach input port cmdRegOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_cmdRegOut();
        ++_port
    ) {

      this->m_from_cmdRegOut[_port].init();
      this->m_from_cmdRegOut[_port].addCallComp(
          this,
          from_cmdRegOut_static
      );
      this->m_from_cmdRegOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_cmdRegOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_cmdRegOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port logOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_logOut();
        ++_port
    ) {

      this->m_from_logOut[_port].init();
      this->m_from_logOut[_port].addCallComp(
          this,
          from_logOut_static
      );
      this->m_from_logOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_logOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_logOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port cmdResponseOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_cmdResponseOut();
        ++_port
    ) {

      this->m_from_cmdResponseOut[_port].init();
      this->m_from_cmdResponseOut[_port].addCallComp(
          this,
          from_cmdResponseOut_static
      );
      this->m_from_cmdResponseOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_cmdResponseOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_cmdResponseOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port LogText

#if FW_ENABLE_TEXT_LOGGING == 1
    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_LogText();
        ++_port
    ) {

      this->m_from_LogText[_port].init();
      this->m_from_LogText[_port].addCallComp(
          this,
          from_LogText_static
      );
      this->m_from_LogText[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_LogText[%d]",
          this->m_objName,
          _port
      );
      this->m_from_LogText[_port].setObjName(_portName);
#endif

    }
#endif

    // Initialize output port comIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_comIn();
        ++_port
    ) {
      this->m_to_comIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_comIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_comIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE ComLoggerTesterBase ::
    getNum_to_comIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_comIn);
  }

  NATIVE_INT_TYPE ComLoggerTesterBase ::
    getNum_from_timeCaller(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_timeCaller);
  }

  NATIVE_INT_TYPE ComLoggerTesterBase ::
    getNum_from_cmdRegOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdRegOut);
  }

  NATIVE_INT_TYPE ComLoggerTesterBase ::
    getNum_to_cmdIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_cmdIn);
  }

  NATIVE_INT_TYPE ComLoggerTesterBase ::
    getNum_from_logOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_logOut);
  }

  NATIVE_INT_TYPE ComLoggerTesterBase ::
    getNum_from_cmdResponseOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdResponseOut);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE ComLoggerTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    connect_to_comIn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputComPort *const comIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_comIn(),static_cast<AssertArg>(portNum));
    this->m_to_comIn[portNum].addCallPort(comIn);
  }

  void ComLoggerTesterBase ::
    connect_to_cmdIn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdPort *const cmdIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_cmdIn(),static_cast<AssertArg>(portNum));
    this->m_to_cmdIn[portNum].addCallPort(cmdIn);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    invoke_to_comIn(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FW_ASSERT(portNum < this->getNum_to_comIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_comIn(),static_cast<AssertArg>(portNum));
    this->m_to_comIn[portNum].invoke(
        data, context
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool ComLoggerTesterBase ::
    isConnected_to_comIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_comIn(), static_cast<AssertArg>(portNum));
    return this->m_to_comIn[portNum].isConnected();
  }

  bool ComLoggerTesterBase ::
    isConnected_to_cmdIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_cmdIn(), static_cast<AssertArg>(portNum));
    return this->m_to_cmdIn[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputTimePort *ComLoggerTesterBase ::
    get_from_timeCaller(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_timeCaller(),static_cast<AssertArg>(portNum));
    return &this->m_from_timeCaller[portNum];
  }

  Fw::InputCmdRegPort *ComLoggerTesterBase ::
    get_from_cmdRegOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdRegOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdRegOut[portNum];
  }

  Fw::InputLogPort *ComLoggerTesterBase ::
    get_from_logOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_logOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_logOut[portNum];
  }

  Fw::InputCmdResponsePort *ComLoggerTesterBase ::
    get_from_cmdResponseOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdResponseOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdResponseOut[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *ComLoggerTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    from_cmdResponseOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    ComLoggerTesterBase* _testerBase =
      static_cast<ComLoggerTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void ComLoggerTesterBase ::
    from_cmdRegOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void ComLoggerTesterBase ::
    from_logOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    ComLoggerTesterBase* _testerBase =
      static_cast<ComLoggerTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void ComLoggerTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    ComLoggerTesterBase* _testerBase =
      static_cast<ComLoggerTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void ComLoggerTesterBase ::
    from_timeCaller_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    ComLoggerTesterBase* _testerBase =
      static_cast<ComLoggerTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    cmdResponseIn(
        const FwOpcodeType opCode,
        const U32 seq,
        const Fw::CommandResponse response
    )
  {
    CmdResponse e = { opCode, seq, response };
    this->cmdResponseHistory->push_back(e);
  }

  // ---------------------------------------------------------------------- 
  // Command: CloseFile
  // ---------------------------------------------------------------------- 

  void ComLoggerTesterBase ::
    sendCmd_CloseFile(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = ComLoggerComponentBase::OPCODE_CLOSEFILE + idBase;

    if (this->m_to_cmdIn[0].isConnected()) {
      this->m_to_cmdIn[0].invoke(
          _opcode,
          cmdSeq,
          buff
      );
    }
    else {
      printf("Test Command Output port not connected!\n");
    }

  }

  
  void ComLoggerTesterBase ::
    sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args) {
       
    const U32 idBase = this->getIdBase();   
    FwOpcodeType _opcode = opcode + idBase;
    if (this->m_to_cmdIn[0].isConnected()) {
      this->m_to_cmdIn[0].invoke(
          _opcode,
          cmdSeq,
          args
      );
    }
    else {
      printf("Test Command Output port not connected!\n");
    }
        
  }
  
  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    clearHistory()
  {
    this->cmdResponseHistory->clear();
    this->textLogHistory->clear();
    this->clearEvents();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    dispatchEvents(
        const FwEventIdType id,
        Fw::Time &timeTag,
        const Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {

    args.resetDeser();

    const U32 idBase = this->getIdBase();
    FW_ASSERT(id >= idBase, id, idBase);
    switch (id - idBase) {

      case ComLoggerComponentBase::EVENTID_FILEOPENERROR: 
      {

        Fw::SerializeStatus _status;
        U32 errornum;
        _status = args.deserialize(errornum);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_FileOpenError(errornum, file);

        break;

      }

      case ComLoggerComponentBase::EVENTID_FILEWRITEERROR: 
      {

        Fw::SerializeStatus _status;
        U32 errornum;
        _status = args.deserialize(errornum);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 bytesWritten;
        _status = args.deserialize(bytesWritten);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 bytesToWrite;
        _status = args.deserialize(bytesToWrite);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_FileWriteError(errornum, bytesWritten, bytesToWrite, file);

        break;

      }

      case ComLoggerComponentBase::EVENTID_FILEVALIDATIONERROR: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg validationFile;
        _status = args.deserialize(validationFile);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 status;
        _status = args.deserialize(status);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_LO_FileValidationError(validationFile, file, status);

        break;

      }

      case ComLoggerComponentBase::EVENTID_FILECLOSED: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_DIAGNOSTIC_FileClosed(file);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void ComLoggerTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_FileOpenError->clear();
    this->eventHistory_FileWriteError->clear();
    this->eventHistory_FileValidationError->clear();
    this->eventHistory_FileClosed->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    textLogIn(
        const U32 id,
        Fw::Time &timeTag,
        const Fw::TextLogSeverity severity,
        const Fw::TextLogString &text
    )
  {
    TextLogEntry e = { id, timeTag, severity, text };
    textLogHistory->push_back(e);
  }

  void ComLoggerTesterBase ::
    printTextLogHistoryEntry(
        const TextLogEntry& e,
        FILE* file
    )
  {
    const char *severityString = "UNKNOWN";
    switch (e.severity) {
      case Fw::LOG_FATAL:
        severityString = "FATAL";
        break;
      case Fw::LOG_WARNING_HI:
        severityString = "WARNING_HI";
        break;
      case Fw::LOG_WARNING_LO:
        severityString = "WARNING_LO";
        break;
      case Fw::LOG_COMMAND:
        severityString = "COMMAND";
        break;
      case Fw::LOG_ACTIVITY_HI:
        severityString = "ACTIVITY_HI";
        break;
      case Fw::LOG_ACTIVITY_LO:
        severityString = "ACTIVITY_LO";
        break;
      case Fw::LOG_DIAGNOSTIC:
       severityString = "DIAGNOSTIC";
        break;
      default:
        severityString = "SEVERITY ERROR";
        break;
    }

    fprintf(
        file,
        "EVENT: (%d) (%d:%d,%d) %s: %s\n",
        e.id,
        const_cast<TextLogEntry&>(e).timeTag.getTimeBase(),
        const_cast<TextLogEntry&>(e).timeTag.getSeconds(),
        const_cast<TextLogEntry&>(e).timeTag.getUSeconds(),
        severityString,
        e.text.toChar()
    );

  }

  void ComLoggerTesterBase ::
    printTextLogHistory(FILE *file) 
  {
    for (U32 i = 0; i < this->textLogHistory->size(); ++i) {
      this->printTextLogHistoryEntry(
          this->textLogHistory->at(i), 
          file
      );
    }
  }

#endif

  // ----------------------------------------------------------------------
  // Event: FileOpenError 
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    logIn_WARNING_HI_FileOpenError(
        U32 errornum,
        Fw::LogStringArg& file
    )
  {
    EventEntry_FileOpenError e = {
      errornum, file
    };
    eventHistory_FileOpenError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileWriteError 
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    logIn_WARNING_HI_FileWriteError(
        U32 errornum,
        U32 bytesWritten,
        U32 bytesToWrite,
        Fw::LogStringArg& file
    )
  {
    EventEntry_FileWriteError e = {
      errornum, bytesWritten, bytesToWrite, file
    };
    eventHistory_FileWriteError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileValidationError 
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    logIn_WARNING_LO_FileValidationError(
        Fw::LogStringArg& validationFile,
        Fw::LogStringArg& file,
        U32 status
    )
  {
    EventEntry_FileValidationError e = {
      validationFile, file, status
    };
    eventHistory_FileValidationError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: FileClosed 
  // ----------------------------------------------------------------------

  void ComLoggerTesterBase ::
    logIn_DIAGNOSTIC_FileClosed(
        Fw::LogStringArg& file
    )
  {
    EventEntry_FileClosed e = {
      file
    };
    eventHistory_FileClosed->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Svc
