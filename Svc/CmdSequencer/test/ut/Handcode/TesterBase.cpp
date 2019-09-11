// ======================================================================
// \title  CmdSequencer/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for CmdSequencer component test harness base class
//
// \copyright
// Copyright 2009-2018, by the California Institute of Technology.
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

  CmdSequencerTesterBase ::
    CmdSequencerTesterBase(
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
    // Initialize telemetry histories
    this->tlmHistory_CS_LoadCommands = 
      new History<TlmEntry_CS_LoadCommands>(maxHistorySize);
    this->tlmHistory_CS_CancelCommands = 
      new History<TlmEntry_CS_CancelCommands>(maxHistorySize);
    this->tlmHistory_CS_Errors = 
      new History<TlmEntry_CS_Errors>(maxHistorySize);
    this->tlmHistory_CS_CommandsExecuted = 
      new History<TlmEntry_CS_CommandsExecuted>(maxHistorySize);
    this->tlmHistory_CS_SequencesCompleted = 
      new History<TlmEntry_CS_SequencesCompleted>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_CS_SequenceLoaded =
      new History<EventEntry_CS_SequenceLoaded>(maxHistorySize);
    this->eventHistory_CS_SequenceCanceled =
      new History<EventEntry_CS_SequenceCanceled>(maxHistorySize);
    this->eventHistory_CS_FileReadError =
      new History<EventEntry_CS_FileReadError>(maxHistorySize);
    this->eventHistory_CS_FileInvalid =
      new History<EventEntry_CS_FileInvalid>(maxHistorySize);
    this->eventHistory_CS_RecordInvalid =
      new History<EventEntry_CS_RecordInvalid>(maxHistorySize);
    this->eventHistory_CS_FileSizeError =
      new History<EventEntry_CS_FileSizeError>(maxHistorySize);
    this->eventHistory_CS_FileNotFound =
      new History<EventEntry_CS_FileNotFound>(maxHistorySize);
    this->eventHistory_CS_FileCrcFailure =
      new History<EventEntry_CS_FileCrcFailure>(maxHistorySize);
    this->eventHistory_CS_CommandComplete =
      new History<EventEntry_CS_CommandComplete>(maxHistorySize);
    this->eventHistory_CS_SequenceComplete =
      new History<EventEntry_CS_SequenceComplete>(maxHistorySize);
    this->eventHistory_CS_CommandError =
      new History<EventEntry_CS_CommandError>(maxHistorySize);
    this->eventHistory_CS_RecordMismatch =
      new History<EventEntry_CS_RecordMismatch>(maxHistorySize);
    this->eventHistory_CS_TimeBaseMismatch =
      new History<EventEntry_CS_TimeBaseMismatch>(maxHistorySize);
    this->eventHistory_CS_TimeContextMismatch =
      new History<EventEntry_CS_TimeContextMismatch>(maxHistorySize);
    this->eventHistory_CS_PortSequenceStarted =
      new History<EventEntry_CS_PortSequenceStarted>(maxHistorySize);
    this->eventHistory_CS_UnexpectedCompletion =
      new History<EventEntry_CS_UnexpectedCompletion>(maxHistorySize);
    this->eventHistory_CS_ModeSwitched =
      new History<EventEntry_CS_ModeSwitched>(maxHistorySize);
    this->eventHistory_CS_SequenceValid =
      new History<EventEntry_CS_SequenceValid>(maxHistorySize);
    this->eventHistory_CS_SequenceTimeout =
      new History<EventEntry_CS_SequenceTimeout>(maxHistorySize);
    this->eventHistory_CS_CmdStepped =
      new History<EventEntry_CS_CmdStepped>(maxHistorySize);
    this->eventHistory_CS_CmdStarted =
      new History<EventEntry_CS_CmdStarted>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_comCmdOut =
      new History<FromPortEntry_comCmdOut>(maxHistorySize);
    this->fromPortHistory_pingOut =
      new History<FromPortEntry_pingOut>(maxHistorySize);
    this->fromPortHistory_seqDone =
      new History<FromPortEntry_seqDone>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  CmdSequencerTesterBase ::
    ~CmdSequencerTesterBase(void) 
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy telemetry histories
    delete this->tlmHistory_CS_LoadCommands;
    delete this->tlmHistory_CS_CancelCommands;
    delete this->tlmHistory_CS_Errors;
    delete this->tlmHistory_CS_CommandsExecuted;
    delete this->tlmHistory_CS_SequencesCompleted;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_CS_SequenceLoaded;
    delete this->eventHistory_CS_SequenceCanceled;
    delete this->eventHistory_CS_FileReadError;
    delete this->eventHistory_CS_FileInvalid;
    delete this->eventHistory_CS_RecordInvalid;
    delete this->eventHistory_CS_FileSizeError;
    delete this->eventHistory_CS_FileNotFound;
    delete this->eventHistory_CS_FileCrcFailure;
    delete this->eventHistory_CS_CommandComplete;
    delete this->eventHistory_CS_SequenceComplete;
    delete this->eventHistory_CS_CommandError;
    delete this->eventHistory_CS_RecordMismatch;
    delete this->eventHistory_CS_TimeBaseMismatch;
    delete this->eventHistory_CS_TimeContextMismatch;
    delete this->eventHistory_CS_PortSequenceStarted;
    delete this->eventHistory_CS_UnexpectedCompletion;
    delete this->eventHistory_CS_ModeSwitched;
    delete this->eventHistory_CS_SequenceValid;
    delete this->eventHistory_CS_SequenceTimeout;
    delete this->eventHistory_CS_CmdStepped;
    delete this->eventHistory_CS_CmdStarted;
  }

  void CmdSequencerTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

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

    // Attach input port tlmOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_tlmOut();
        ++_port
    ) {

      this->m_from_tlmOut[_port].init();
      this->m_from_tlmOut[_port].addCallComp(
          this,
          from_tlmOut_static
      );
      this->m_from_tlmOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_tlmOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_tlmOut[_port].setObjName(_portName);
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

    // Attach input port comCmdOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_comCmdOut();
        ++_port
    ) {

      this->m_from_comCmdOut[_port].init();
      this->m_from_comCmdOut[_port].addCallComp(
          this,
          from_comCmdOut_static
      );
      this->m_from_comCmdOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_comCmdOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_comCmdOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port pingOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_pingOut();
        ++_port
    ) {

      this->m_from_pingOut[_port].init();
      this->m_from_pingOut[_port].addCallComp(
          this,
          from_pingOut_static
      );
      this->m_from_pingOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_pingOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_pingOut[_port].setObjName(_portName);
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

    // Attach input port seqDone

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_seqDone();
        ++_port
    ) {

      this->m_from_seqDone[_port].init();
      this->m_from_seqDone[_port].addCallComp(
          this,
          from_seqDone_static
      );
      this->m_from_seqDone[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_seqDone[%d]",
          this->m_objName,
          _port
      );
      this->m_from_seqDone[_port].setObjName(_portName);
#endif

    }

    // Initialize output port pingIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_pingIn();
        ++_port
    ) {
      this->m_to_pingIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_pingIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_pingIn[_port].setObjName(_portName);
#endif

    }

    // Initialize output port cmdResponseIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_cmdResponseIn();
        ++_port
    ) {
      this->m_to_cmdResponseIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_cmdResponseIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_cmdResponseIn[_port].setObjName(_portName);
#endif

    }

    // Initialize output port schedIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_schedIn();
        ++_port
    ) {
      this->m_to_schedIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_schedIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_schedIn[_port].setObjName(_portName);
#endif

    }

    // Initialize output port seqRunIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_seqRunIn();
        ++_port
    ) {
      this->m_to_seqRunIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_seqRunIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_seqRunIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_cmdRegOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdRegOut);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_to_pingIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_pingIn);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_tlmOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_tlmOut);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_cmdResponseOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdResponseOut);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_timeCaller(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_timeCaller);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_comCmdOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_comCmdOut);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_pingOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_pingOut);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_to_cmdResponseIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_cmdResponseIn);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_to_cmdIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_cmdIn);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_to_schedIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_schedIn);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_to_seqRunIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_seqRunIn);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_logOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_logOut);
  }

  NATIVE_INT_TYPE CmdSequencerTesterBase ::
    getNum_from_seqDone(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_seqDone);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    connect_to_pingIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputPingPort *const pingIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    this->m_to_pingIn[portNum].addCallPort(pingIn);
  }

  void CmdSequencerTesterBase ::
    connect_to_cmdResponseIn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdResponsePort *const cmdResponseIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_cmdResponseIn(),static_cast<AssertArg>(portNum));
    this->m_to_cmdResponseIn[portNum].addCallPort(cmdResponseIn);
  }

  void CmdSequencerTesterBase ::
    connect_to_cmdIn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdPort *const cmdIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_cmdIn(),static_cast<AssertArg>(portNum));
    this->m_to_cmdIn[portNum].addCallPort(cmdIn);
  }

  void CmdSequencerTesterBase ::
    connect_to_schedIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputSchedPort *const schedIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_schedIn(),static_cast<AssertArg>(portNum));
    this->m_to_schedIn[portNum].addCallPort(schedIn);
  }

  void CmdSequencerTesterBase ::
    connect_to_seqRunIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputCmdSeqInPort *const seqRunIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_seqRunIn(),static_cast<AssertArg>(portNum));
    this->m_to_seqRunIn[portNum].addCallPort(seqRunIn);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    invoke_to_pingIn(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    this->m_to_pingIn[portNum].invoke(
        key
    );
  }

  void CmdSequencerTesterBase ::
    invoke_to_cmdResponseIn(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CommandResponse response
    )
  {
    FW_ASSERT(portNum < this->getNum_to_cmdResponseIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_cmdResponseIn(),static_cast<AssertArg>(portNum));
    this->m_to_cmdResponseIn[portNum].invoke(
        opCode, cmdSeq, response
    );
  }

  void CmdSequencerTesterBase ::
    invoke_to_schedIn(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    FW_ASSERT(portNum < this->getNum_to_schedIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_schedIn(),static_cast<AssertArg>(portNum));
    this->m_to_schedIn[portNum].invoke(
        context
    );
  }

  void CmdSequencerTesterBase ::
    invoke_to_seqRunIn(
        const NATIVE_INT_TYPE portNum,
        Fw::EightyCharString &filename
    )
  {
    FW_ASSERT(portNum < this->getNum_to_seqRunIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_seqRunIn(),static_cast<AssertArg>(portNum));
    this->m_to_seqRunIn[portNum].invoke(
        filename
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool CmdSequencerTesterBase ::
    isConnected_to_pingIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(), static_cast<AssertArg>(portNum));
    return this->m_to_pingIn[portNum].isConnected();
  }

  bool CmdSequencerTesterBase ::
    isConnected_to_cmdResponseIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_cmdResponseIn(), static_cast<AssertArg>(portNum));
    return this->m_to_cmdResponseIn[portNum].isConnected();
  }

  bool CmdSequencerTesterBase ::
    isConnected_to_cmdIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_cmdIn(), static_cast<AssertArg>(portNum));
    return this->m_to_cmdIn[portNum].isConnected();
  }

  bool CmdSequencerTesterBase ::
    isConnected_to_schedIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_schedIn(), static_cast<AssertArg>(portNum));
    return this->m_to_schedIn[portNum].isConnected();
  }

  bool CmdSequencerTesterBase ::
    isConnected_to_seqRunIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_seqRunIn(), static_cast<AssertArg>(portNum));
    return this->m_to_seqRunIn[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputCmdRegPort *CmdSequencerTesterBase ::
    get_from_cmdRegOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdRegOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdRegOut[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *CmdSequencerTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  Fw::InputTlmPort *CmdSequencerTesterBase ::
    get_from_tlmOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_tlmOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_tlmOut[portNum];
  }

  Fw::InputCmdResponsePort *CmdSequencerTesterBase ::
    get_from_cmdResponseOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdResponseOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdResponseOut[portNum];
  }

  Fw::InputTimePort *CmdSequencerTesterBase ::
    get_from_timeCaller(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_timeCaller(),static_cast<AssertArg>(portNum));
    return &this->m_from_timeCaller[portNum];
  }

  Fw::InputComPort *CmdSequencerTesterBase ::
    get_from_comCmdOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_comCmdOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_comCmdOut[portNum];
  }

  Svc::InputPingPort *CmdSequencerTesterBase ::
    get_from_pingOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_pingOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_pingOut[portNum];
  }

  Fw::InputLogPort *CmdSequencerTesterBase ::
    get_from_logOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_logOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_logOut[portNum];
  }

  Fw::InputCmdResponsePort *CmdSequencerTesterBase ::
    get_from_seqDone(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_seqDone(),static_cast<AssertArg>(portNum));
    return &this->m_from_seqDone[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    from_comCmdOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FW_ASSERT(callComp);
    CmdSequencerTesterBase* _testerBase = 
      static_cast<CmdSequencerTesterBase*>(callComp);
    _testerBase->from_comCmdOut_handlerBase(
        portNum,
        data, context
    );
  }

  void CmdSequencerTesterBase ::
    from_pingOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(callComp);
    CmdSequencerTesterBase* _testerBase = 
      static_cast<CmdSequencerTesterBase*>(callComp);
    _testerBase->from_pingOut_handlerBase(
        portNum,
        key
    );
  }

  void CmdSequencerTesterBase ::
    from_seqDone_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CommandResponse response
    )
  {
    FW_ASSERT(callComp);
    CmdSequencerTesterBase* _testerBase = 
      static_cast<CmdSequencerTesterBase*>(callComp);
    _testerBase->from_seqDone_handlerBase(
        portNum,
        opCode, cmdSeq, response
    );
  }

  void CmdSequencerTesterBase ::
    from_cmdResponseOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    CmdSequencerTesterBase* _testerBase =
      static_cast<CmdSequencerTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void CmdSequencerTesterBase ::
    from_cmdRegOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void CmdSequencerTesterBase ::
    from_tlmOut_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    CmdSequencerTesterBase* _testerBase =
      static_cast<CmdSequencerTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void CmdSequencerTesterBase ::
    from_logOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    CmdSequencerTesterBase* _testerBase =
      static_cast<CmdSequencerTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void CmdSequencerTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    CmdSequencerTesterBase* _testerBase =
      static_cast<CmdSequencerTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void CmdSequencerTesterBase ::
    from_timeCaller_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    CmdSequencerTesterBase* _testerBase =
      static_cast<CmdSequencerTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_comCmdOut->clear();
    this->fromPortHistory_pingOut->clear();
    this->fromPortHistory_seqDone->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: comCmdOut
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    pushFromPortEntry_comCmdOut(
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FromPortEntry_comCmdOut _e = {
      data, context
    };
    this->fromPortHistory_comCmdOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ---------------------------------------------------------------------- 
  // From port: pingOut
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    pushFromPortEntry_pingOut(
        U32 key
    )
  {
    FromPortEntry_pingOut _e = {
      key
    };
    this->fromPortHistory_pingOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ---------------------------------------------------------------------- 
  // From port: seqDone
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    pushFromPortEntry_seqDone(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CommandResponse response
    )
  {
    FromPortEntry_seqDone _e = {
      opCode, cmdSeq, response
    };
    this->fromPortHistory_seqDone->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    from_comCmdOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FW_ASSERT(portNum < this->getNum_from_comCmdOut(),static_cast<AssertArg>(portNum));
    this->from_comCmdOut_handler(
        portNum,
        data, context
    );
  }

  void CmdSequencerTesterBase ::
    from_pingOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_from_pingOut(),static_cast<AssertArg>(portNum));
    this->from_pingOut_handler(
        portNum,
        key
    );
  }

  void CmdSequencerTesterBase ::
    from_seqDone_handlerBase(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CommandResponse response
    )
  {
    FW_ASSERT(portNum < this->getNum_from_seqDone(),static_cast<AssertArg>(portNum));
    this->from_seqDone_handler(
        portNum,
        opCode, cmdSeq, response
    );
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
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
  // Command: CS_RUN
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    sendCmd_CS_RUN(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        const Fw::CmdStringArg& fileName
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(fileName);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = CmdSequencerComponentBase::OPCODE_CS_RUN + idBase;

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

  // ---------------------------------------------------------------------- 
  // Command: CS_VALIDATE
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    sendCmd_CS_VALIDATE(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        const Fw::CmdStringArg& fileName
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(fileName);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = CmdSequencerComponentBase::OPCODE_CS_VALIDATE + idBase;

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

  // ---------------------------------------------------------------------- 
  // Command: CS_CANCEL
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    sendCmd_CS_CANCEL(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = CmdSequencerComponentBase::OPCODE_CS_CANCEL + idBase;

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

  // ---------------------------------------------------------------------- 
  // Command: CS_START
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    sendCmd_CS_START(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = CmdSequencerComponentBase::OPCODE_CS_START + idBase;

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

  // ---------------------------------------------------------------------- 
  // Command: CS_STEP
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    sendCmd_CS_STEP(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = CmdSequencerComponentBase::OPCODE_CS_STEP + idBase;

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

  // ---------------------------------------------------------------------- 
  // Command: CS_AUTO
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    sendCmd_CS_AUTO(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = CmdSequencerComponentBase::OPCODE_CS_AUTO + idBase;

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

  // ---------------------------------------------------------------------- 
  // Command: CS_MANUAL
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    sendCmd_CS_MANUAL(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = CmdSequencerComponentBase::OPCODE_CS_MANUAL + idBase;

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

  
  void CmdSequencerTesterBase ::
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

  void CmdSequencerTesterBase ::
    clearHistory()
  {
    this->cmdResponseHistory->clear();
    this->clearTlm();
    this->textLogHistory->clear();
    this->clearEvents();
    this->clearFromPortHistory();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    dispatchTlm(
        const FwChanIdType id,
        const Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {

    val.resetDeser();

    const U32 idBase = this->getIdBase();
    FW_ASSERT(id >= idBase, id, idBase);

    switch (id - idBase) {

      case CmdSequencerComponentBase::CHANNELID_CS_LOADCOMMANDS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing CS_LoadCommands: %d\n", _status);
          return;
        }
        this->tlmInput_CS_LoadCommands(timeTag, arg);
        break;
      }

      case CmdSequencerComponentBase::CHANNELID_CS_CANCELCOMMANDS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing CS_CancelCommands: %d\n", _status);
          return;
        }
        this->tlmInput_CS_CancelCommands(timeTag, arg);
        break;
      }

      case CmdSequencerComponentBase::CHANNELID_CS_ERRORS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing CS_Errors: %d\n", _status);
          return;
        }
        this->tlmInput_CS_Errors(timeTag, arg);
        break;
      }

      case CmdSequencerComponentBase::CHANNELID_CS_COMMANDSEXECUTED:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing CS_CommandsExecuted: %d\n", _status);
          return;
        }
        this->tlmInput_CS_CommandsExecuted(timeTag, arg);
        break;
      }

      case CmdSequencerComponentBase::CHANNELID_CS_SEQUENCESCOMPLETED:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing CS_SequencesCompleted: %d\n", _status);
          return;
        }
        this->tlmInput_CS_SequencesCompleted(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void CmdSequencerTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_CS_LoadCommands->clear();
    this->tlmHistory_CS_CancelCommands->clear();
    this->tlmHistory_CS_Errors->clear();
    this->tlmHistory_CS_CommandsExecuted->clear();
    this->tlmHistory_CS_SequencesCompleted->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: CS_LoadCommands
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    tlmInput_CS_LoadCommands(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_CS_LoadCommands e = { timeTag, val };
    this->tlmHistory_CS_LoadCommands->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: CS_CancelCommands
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    tlmInput_CS_CancelCommands(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_CS_CancelCommands e = { timeTag, val };
    this->tlmHistory_CS_CancelCommands->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: CS_Errors
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    tlmInput_CS_Errors(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_CS_Errors e = { timeTag, val };
    this->tlmHistory_CS_Errors->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: CS_CommandsExecuted
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    tlmInput_CS_CommandsExecuted(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_CS_CommandsExecuted e = { timeTag, val };
    this->tlmHistory_CS_CommandsExecuted->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: CS_SequencesCompleted
  // ---------------------------------------------------------------------- 

  void CmdSequencerTesterBase ::
    tlmInput_CS_SequencesCompleted(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_CS_SequencesCompleted e = { timeTag, val };
    this->tlmHistory_CS_SequencesCompleted->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
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

      case CmdSequencerComponentBase::EVENTID_CS_SEQUENCELOADED: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_LO_CS_SequenceLoaded(fileName);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_SEQUENCECANCELED: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_CS_SequenceCanceled(fileName);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_FILEREADERROR: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_FileReadError(fileName);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_FILEINVALID: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 3,_numArgs,3);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(FwEnumStoreType),_argSize,sizeof(FwEnumStoreType));
        }
#endif      
        FwEnumStoreType stageInt;
        _status = args.deserialize(stageInt);
        CmdSequencerComponentBase::FileReadStage stage = static_cast<CmdSequencerComponentBase::FileReadStage>(stageInt);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 error;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(I32),_argSize,sizeof(I32));
        }
#endif      
        _status = args.deserialize(error);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_FileInvalid(fileName, stage, error);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_RECORDINVALID: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 3,_numArgs,3);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 recordNumber;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(recordNumber);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 error;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(I32),_argSize,sizeof(I32));
        }
#endif      
        _status = args.deserialize(error);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_RecordInvalid(fileName, recordNumber, error);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_FILESIZEERROR: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 2,_numArgs,2);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 size;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(size);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_FileSizeError(fileName, size);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_FILENOTFOUND: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_FileNotFound(fileName);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_FILECRCFAILURE: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 3,_numArgs,3);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 storedCRC;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(storedCRC);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 computedCRC;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(computedCRC);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_FileCrcFailure(fileName, storedCRC, computedCRC);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_COMMANDCOMPLETE: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 3,_numArgs,3);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 recordNumber;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(recordNumber);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 opCode;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(opCode);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_LO_CS_CommandComplete(fileName, recordNumber, opCode);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_SEQUENCECOMPLETE: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_CS_SequenceComplete(fileName);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_COMMANDERROR: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 4,_numArgs,4);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 recordNumber;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(recordNumber);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 opCode;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(opCode);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 errorStatus;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(errorStatus);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_CommandError(fileName, recordNumber, opCode, errorStatus);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_INVALIDMODE: 
      {

#if FW_AMPCS_COMPATIBLE
        // For AMPCS, decode zero arguments
        Fw::SerializeStatus _zero_status = Fw::FW_SERIALIZE_OK;
        U8 _noArgs;
        _zero_status = args.deserialize(_noArgs);
        FW_ASSERT(
            _zero_status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_zero_status)
        );
#endif    
        this->logIn_WARNING_HI_CS_InvalidMode();

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_RECORDMISMATCH: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 3,_numArgs,3);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 header_records;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(header_records);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 extra_bytes;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(extra_bytes);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_RecordMismatch(fileName, header_records, extra_bytes);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_TIMEBASEMISMATCH: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 3,_numArgs,3);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U16 time_base;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U16),_argSize,sizeof(U16));
        }
#endif      
        _status = args.deserialize(time_base);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U16 seq_time_base;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U16),_argSize,sizeof(U16));
        }
#endif      
        _status = args.deserialize(seq_time_base);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_TimeBaseMismatch(fileName, time_base, seq_time_base);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_TIMECONTEXTMISMATCH: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 3,_numArgs,3);
        
#endif    
        Fw::LogStringArg fileName;
        _status = args.deserialize(fileName);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U8 currTimeBase;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U8),_argSize,sizeof(U8));
        }
#endif      
        _status = args.deserialize(currTimeBase);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U8 seqTimeBase;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U8),_argSize,sizeof(U8));
        }
#endif      
        _status = args.deserialize(seqTimeBase);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_TimeContextMismatch(fileName, currTimeBase, seqTimeBase);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_PORTSEQUENCESTARTED: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg filename;
        _status = args.deserialize(filename);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_CS_PortSequenceStarted(filename);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_UNEXPECTEDCOMPLETION: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        U32 opcode;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(opcode);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_UnexpectedCompletion(opcode);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_MODESWITCHED: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(FwEnumStoreType),_argSize,sizeof(FwEnumStoreType));
        }
#endif      
        FwEnumStoreType modeInt;
        _status = args.deserialize(modeInt);
        CmdSequencerComponentBase::SeqMode mode = static_cast<CmdSequencerComponentBase::SeqMode>(modeInt);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_CS_ModeSwitched(mode);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_NOSEQUENCEACTIVE: 
      {

#if FW_AMPCS_COMPATIBLE
        // For AMPCS, decode zero arguments
        Fw::SerializeStatus _zero_status = Fw::FW_SERIALIZE_OK;
        U8 _noArgs;
        _zero_status = args.deserialize(_noArgs);
        FW_ASSERT(
            _zero_status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_zero_status)
        );
#endif    
        this->logIn_WARNING_LO_CS_NoSequenceActive();

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_SEQUENCEVALID: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg filename;
        _status = args.deserialize(filename);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_CS_SequenceValid(filename);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_SEQUENCETIMEOUT: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 2,_numArgs,2);
        
#endif    
        Fw::LogStringArg filename;
        _status = args.deserialize(filename);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 command;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(command);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_CS_SequenceTimeout(filename, command);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_CMDSTEPPED: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 2,_numArgs,2);
        
#endif    
        Fw::LogStringArg filename;
        _status = args.deserialize(filename);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 command;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(U32),_argSize,sizeof(U32));
        }
#endif      
        _status = args.deserialize(command);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_CS_CmdStepped(filename, command);

        break;

      }

      case CmdSequencerComponentBase::EVENTID_CS_CMDSTARTED: 
      {

        Fw::SerializeStatus _status = Fw::FW_SERIALIZE_OK;
#if FW_AMPCS_COMPATIBLE
        // Deserialize the number of arguments.
        U8 _numArgs;
        _status = args.deserialize(_numArgs);
        FW_ASSERT(
          _status == Fw::FW_SERIALIZE_OK,
          static_cast<AssertArg>(_status)
        );
        // verify they match expected.
        FW_ASSERT(_numArgs == 1,_numArgs,1);
        
#endif    
        Fw::LogStringArg filename;
        _status = args.deserialize(filename);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_CS_CmdStarted(filename);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void CmdSequencerTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_CS_SequenceLoaded->clear();
    this->eventHistory_CS_SequenceCanceled->clear();
    this->eventHistory_CS_FileReadError->clear();
    this->eventHistory_CS_FileInvalid->clear();
    this->eventHistory_CS_RecordInvalid->clear();
    this->eventHistory_CS_FileSizeError->clear();
    this->eventHistory_CS_FileNotFound->clear();
    this->eventHistory_CS_FileCrcFailure->clear();
    this->eventHistory_CS_CommandComplete->clear();
    this->eventHistory_CS_SequenceComplete->clear();
    this->eventHistory_CS_CommandError->clear();
    this->eventsSize_CS_InvalidMode = 0;
    this->eventHistory_CS_RecordMismatch->clear();
    this->eventHistory_CS_TimeBaseMismatch->clear();
    this->eventHistory_CS_TimeContextMismatch->clear();
    this->eventHistory_CS_PortSequenceStarted->clear();
    this->eventHistory_CS_UnexpectedCompletion->clear();
    this->eventHistory_CS_ModeSwitched->clear();
    this->eventsSize_CS_NoSequenceActive = 0;
    this->eventHistory_CS_SequenceValid->clear();
    this->eventHistory_CS_SequenceTimeout->clear();
    this->eventHistory_CS_CmdStepped->clear();
    this->eventHistory_CS_CmdStarted->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
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

  void CmdSequencerTesterBase ::
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

  void CmdSequencerTesterBase ::
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
  // Event: CS_SequenceLoaded 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_LO_CS_SequenceLoaded(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_CS_SequenceLoaded e = {
      fileName
    };
    eventHistory_CS_SequenceLoaded->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_SequenceCanceled 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_HI_CS_SequenceCanceled(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_CS_SequenceCanceled e = {
      fileName
    };
    eventHistory_CS_SequenceCanceled->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_FileReadError 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_FileReadError(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_CS_FileReadError e = {
      fileName
    };
    eventHistory_CS_FileReadError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_FileInvalid 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_FileInvalid(
        Fw::LogStringArg& fileName,
        CmdSequencerComponentBase::FileReadStage stage,
        I32 error
    )
  {
    EventEntry_CS_FileInvalid e = {
      fileName, stage, error
    };
    eventHistory_CS_FileInvalid->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_RecordInvalid 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_RecordInvalid(
        Fw::LogStringArg& fileName,
        U32 recordNumber,
        I32 error
    )
  {
    EventEntry_CS_RecordInvalid e = {
      fileName, recordNumber, error
    };
    eventHistory_CS_RecordInvalid->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_FileSizeError 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_FileSizeError(
        Fw::LogStringArg& fileName,
        U32 size
    )
  {
    EventEntry_CS_FileSizeError e = {
      fileName, size
    };
    eventHistory_CS_FileSizeError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_FileNotFound 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_FileNotFound(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_CS_FileNotFound e = {
      fileName
    };
    eventHistory_CS_FileNotFound->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_FileCrcFailure 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_FileCrcFailure(
        Fw::LogStringArg& fileName,
        U32 storedCRC,
        U32 computedCRC
    )
  {
    EventEntry_CS_FileCrcFailure e = {
      fileName, storedCRC, computedCRC
    };
    eventHistory_CS_FileCrcFailure->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_CommandComplete 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_LO_CS_CommandComplete(
        Fw::LogStringArg& fileName,
        U32 recordNumber,
        U32 opCode
    )
  {
    EventEntry_CS_CommandComplete e = {
      fileName, recordNumber, opCode
    };
    eventHistory_CS_CommandComplete->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_SequenceComplete 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_HI_CS_SequenceComplete(
        Fw::LogStringArg& fileName
    )
  {
    EventEntry_CS_SequenceComplete e = {
      fileName
    };
    eventHistory_CS_SequenceComplete->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_CommandError 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_CommandError(
        Fw::LogStringArg& fileName,
        U32 recordNumber,
        U32 opCode,
        U32 errorStatus
    )
  {
    EventEntry_CS_CommandError e = {
      fileName, recordNumber, opCode, errorStatus
    };
    eventHistory_CS_CommandError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_InvalidMode 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_InvalidMode(
        void
    )
  {
    ++this->eventsSize_CS_InvalidMode;
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_RecordMismatch 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_RecordMismatch(
        Fw::LogStringArg& fileName,
        U32 header_records,
        U32 extra_bytes
    )
  {
    EventEntry_CS_RecordMismatch e = {
      fileName, header_records, extra_bytes
    };
    eventHistory_CS_RecordMismatch->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_TimeBaseMismatch 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_TimeBaseMismatch(
        Fw::LogStringArg& fileName,
        U16 time_base,
        U16 seq_time_base
    )
  {
    EventEntry_CS_TimeBaseMismatch e = {
      fileName, time_base, seq_time_base
    };
    eventHistory_CS_TimeBaseMismatch->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_TimeContextMismatch 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_TimeContextMismatch(
        Fw::LogStringArg& fileName,
        U8 currTimeBase,
        U8 seqTimeBase
    )
  {
    EventEntry_CS_TimeContextMismatch e = {
      fileName, currTimeBase, seqTimeBase
    };
    eventHistory_CS_TimeContextMismatch->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_PortSequenceStarted 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_HI_CS_PortSequenceStarted(
        Fw::LogStringArg& filename
    )
  {
    EventEntry_CS_PortSequenceStarted e = {
      filename
    };
    eventHistory_CS_PortSequenceStarted->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_UnexpectedCompletion 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_UnexpectedCompletion(
        U32 opcode
    )
  {
    EventEntry_CS_UnexpectedCompletion e = {
      opcode
    };
    eventHistory_CS_UnexpectedCompletion->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_ModeSwitched 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_HI_CS_ModeSwitched(
        CmdSequencerComponentBase::SeqMode mode
    )
  {
    EventEntry_CS_ModeSwitched e = {
      mode
    };
    eventHistory_CS_ModeSwitched->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_NoSequenceActive 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_LO_CS_NoSequenceActive(
        void
    )
  {
    ++this->eventsSize_CS_NoSequenceActive;
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_SequenceValid 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_HI_CS_SequenceValid(
        Fw::LogStringArg& filename
    )
  {
    EventEntry_CS_SequenceValid e = {
      filename
    };
    eventHistory_CS_SequenceValid->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_SequenceTimeout 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_WARNING_HI_CS_SequenceTimeout(
        Fw::LogStringArg& filename,
        U32 command
    )
  {
    EventEntry_CS_SequenceTimeout e = {
      filename, command
    };
    eventHistory_CS_SequenceTimeout->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_CmdStepped 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_HI_CS_CmdStepped(
        Fw::LogStringArg& filename,
        U32 command
    )
  {
    EventEntry_CS_CmdStepped e = {
      filename, command
    };
    eventHistory_CS_CmdStepped->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: CS_CmdStarted 
  // ----------------------------------------------------------------------

  void CmdSequencerTesterBase ::
    logIn_ACTIVITY_HI_CS_CmdStarted(
        Fw::LogStringArg& filename
    )
  {
    EventEntry_CS_CmdStarted e = {
      filename
    };
    eventHistory_CS_CmdStarted->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Svc
