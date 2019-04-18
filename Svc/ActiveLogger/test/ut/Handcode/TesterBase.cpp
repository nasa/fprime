// ======================================================================
// \title  ActiveLogger/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for ActiveLogger component test harness base class
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

  ActiveLoggerTesterBase ::
    ActiveLoggerTesterBase(
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
    this->eventHistory_ALOG_FILE_WRITE_ERR =
      new History<EventEntry_ALOG_FILE_WRITE_ERR>(maxHistorySize);
    this->eventHistory_ALOG_FILE_WRITE_COMPLETE =
      new History<EventEntry_ALOG_FILE_WRITE_COMPLETE>(maxHistorySize);
    this->eventHistory_ALOG_SEVERITY_FILTER_STATE =
      new History<EventEntry_ALOG_SEVERITY_FILTER_STATE>(maxHistorySize);
    this->eventHistory_ALOG_ID_FILTER_ENABLED =
      new History<EventEntry_ALOG_ID_FILTER_ENABLED>(maxHistorySize);
    this->eventHistory_ALOG_ID_FILTER_LIST_FULL =
      new History<EventEntry_ALOG_ID_FILTER_LIST_FULL>(maxHistorySize);
    this->eventHistory_ALOG_ID_FILTER_REMOVED =
      new History<EventEntry_ALOG_ID_FILTER_REMOVED>(maxHistorySize);
    this->eventHistory_ALOG_ID_FILTER_NOT_FOUND =
      new History<EventEntry_ALOG_ID_FILTER_NOT_FOUND>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_PktSend =
      new History<FromPortEntry_PktSend>(maxHistorySize);
    this->fromPortHistory_FatalAnnounce =
      new History<FromPortEntry_FatalAnnounce>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  ActiveLoggerTesterBase ::
    ~ActiveLoggerTesterBase(void) 
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_ALOG_FILE_WRITE_ERR;
    delete this->eventHistory_ALOG_FILE_WRITE_COMPLETE;
    delete this->eventHistory_ALOG_SEVERITY_FILTER_STATE;
    delete this->eventHistory_ALOG_ID_FILTER_ENABLED;
    delete this->eventHistory_ALOG_ID_FILTER_LIST_FULL;
    delete this->eventHistory_ALOG_ID_FILTER_REMOVED;
    delete this->eventHistory_ALOG_ID_FILTER_NOT_FOUND;
  }

  void ActiveLoggerTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port PktSend

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_PktSend();
        ++_port
    ) {

      this->m_from_PktSend[_port].init();
      this->m_from_PktSend[_port].addCallComp(
          this,
          from_PktSend_static
      );
      this->m_from_PktSend[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_PktSend[%d]",
          this->m_objName,
          _port
      );
      this->m_from_PktSend[_port].setObjName(_portName);
#endif

    }

    // Attach input port FatalAnnounce

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_FatalAnnounce();
        ++_port
    ) {

      this->m_from_FatalAnnounce[_port].init();
      this->m_from_FatalAnnounce[_port].addCallComp(
          this,
          from_FatalAnnounce_static
      );
      this->m_from_FatalAnnounce[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_FatalAnnounce[%d]",
          this->m_objName,
          _port
      );
      this->m_from_FatalAnnounce[_port].setObjName(_portName);
#endif

    }

    // Attach input port CmdStatus

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_CmdStatus();
        ++_port
    ) {

      this->m_from_CmdStatus[_port].init();
      this->m_from_CmdStatus[_port].addCallComp(
          this,
          from_CmdStatus_static
      );
      this->m_from_CmdStatus[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_CmdStatus[%d]",
          this->m_objName,
          _port
      );
      this->m_from_CmdStatus[_port].setObjName(_portName);
#endif

    }

    // Attach input port CmdReg

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_CmdReg();
        ++_port
    ) {

      this->m_from_CmdReg[_port].init();
      this->m_from_CmdReg[_port].addCallComp(
          this,
          from_CmdReg_static
      );
      this->m_from_CmdReg[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_CmdReg[%d]",
          this->m_objName,
          _port
      );
      this->m_from_CmdReg[_port].setObjName(_portName);
#endif

    }

    // Attach input port Time

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_Time();
        ++_port
    ) {

      this->m_from_Time[_port].init();
      this->m_from_Time[_port].addCallComp(
          this,
          from_Time_static
      );
      this->m_from_Time[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_Time[%d]",
          this->m_objName,
          _port
      );
      this->m_from_Time[_port].setObjName(_portName);
#endif

    }

    // Attach input port Log

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_Log();
        ++_port
    ) {

      this->m_from_Log[_port].init();
      this->m_from_Log[_port].addCallComp(
          this,
          from_Log_static
      );
      this->m_from_Log[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_Log[%d]",
          this->m_objName,
          _port
      );
      this->m_from_Log[_port].setObjName(_portName);
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

    // Initialize output port LogRecv

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_LogRecv();
        ++_port
    ) {
      this->m_to_LogRecv[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_LogRecv[%d]",
          this->m_objName,
          _port
      );
      this->m_to_LogRecv[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_to_LogRecv(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_LogRecv);
  }

  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_from_PktSend(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_PktSend);
  }

  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_from_FatalAnnounce(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_FatalAnnounce);
  }

  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_to_CmdDisp(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CmdDisp);
  }

  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_from_CmdStatus(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdStatus);
  }

  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_from_CmdReg(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdReg);
  }

  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE ActiveLoggerTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    connect_to_LogRecv(
        const NATIVE_INT_TYPE portNum,
        Fw::InputLogPort *const LogRecv
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_LogRecv(),static_cast<AssertArg>(portNum));
    this->m_to_LogRecv[portNum].addCallPort(LogRecv);
  }

  void ActiveLoggerTesterBase ::
    connect_to_CmdDisp(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdPort *const CmdDisp
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(),static_cast<AssertArg>(portNum));
    this->m_to_CmdDisp[portNum].addCallPort(CmdDisp);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    invoke_to_LogRecv(
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    FW_ASSERT(portNum < this->getNum_to_LogRecv(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_LogRecv(),static_cast<AssertArg>(portNum));
    this->m_to_LogRecv[portNum].invoke(
        id, timeTag, severity, args
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool ActiveLoggerTesterBase ::
    isConnected_to_LogRecv(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_LogRecv(), static_cast<AssertArg>(portNum));
    return this->m_to_LogRecv[portNum].isConnected();
  }

  bool ActiveLoggerTesterBase ::
    isConnected_to_CmdDisp(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(), static_cast<AssertArg>(portNum));
    return this->m_to_CmdDisp[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputComPort *ActiveLoggerTesterBase ::
    get_from_PktSend(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_PktSend(),static_cast<AssertArg>(portNum));
    return &this->m_from_PktSend[portNum];
  }

  Svc::InputFatalEventPort *ActiveLoggerTesterBase ::
    get_from_FatalAnnounce(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_FatalAnnounce(),static_cast<AssertArg>(portNum));
    return &this->m_from_FatalAnnounce[portNum];
  }

  Fw::InputCmdResponsePort *ActiveLoggerTesterBase ::
    get_from_CmdStatus(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdStatus(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdStatus[portNum];
  }

  Fw::InputCmdRegPort *ActiveLoggerTesterBase ::
    get_from_CmdReg(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdReg(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdReg[portNum];
  }

  Fw::InputTimePort *ActiveLoggerTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputLogPort *ActiveLoggerTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *ActiveLoggerTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    from_PktSend_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FW_ASSERT(callComp);
    ActiveLoggerTesterBase* _testerBase = 
      static_cast<ActiveLoggerTesterBase*>(callComp);
    _testerBase->from_PktSend_handlerBase(
        portNum,
        data, context
    );
  }

  void ActiveLoggerTesterBase ::
    from_FatalAnnounce_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType Id
    )
  {
    FW_ASSERT(callComp);
    ActiveLoggerTesterBase* _testerBase = 
      static_cast<ActiveLoggerTesterBase*>(callComp);
    _testerBase->from_FatalAnnounce_handlerBase(
        portNum,
        Id
    );
  }

  void ActiveLoggerTesterBase ::
    from_CmdStatus_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    ActiveLoggerTesterBase* _testerBase =
      static_cast<ActiveLoggerTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void ActiveLoggerTesterBase ::
    from_CmdReg_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void ActiveLoggerTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    ActiveLoggerTesterBase* _testerBase =
      static_cast<ActiveLoggerTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void ActiveLoggerTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    ActiveLoggerTesterBase* _testerBase =
      static_cast<ActiveLoggerTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void ActiveLoggerTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    ActiveLoggerTesterBase* _testerBase =
      static_cast<ActiveLoggerTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_PktSend->clear();
    this->fromPortHistory_FatalAnnounce->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: PktSend
  // ---------------------------------------------------------------------- 

  void ActiveLoggerTesterBase ::
    pushFromPortEntry_PktSend(
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FromPortEntry_PktSend _e = {
      data, context
    };
    this->fromPortHistory_PktSend->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ---------------------------------------------------------------------- 
  // From port: FatalAnnounce
  // ---------------------------------------------------------------------- 

  void ActiveLoggerTesterBase ::
    pushFromPortEntry_FatalAnnounce(
        FwEventIdType Id
    )
  {
    FromPortEntry_FatalAnnounce _e = {
      Id
    };
    this->fromPortHistory_FatalAnnounce->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    from_PktSend_handlerBase(
        const NATIVE_INT_TYPE portNum,
        Fw::ComBuffer &data,
        U32 context
    )
  {
    FW_ASSERT(portNum < this->getNum_from_PktSend(),static_cast<AssertArg>(portNum));
    this->from_PktSend_handler(
        portNum,
        data, context
    );
  }

  void ActiveLoggerTesterBase ::
    from_FatalAnnounce_handlerBase(
        const NATIVE_INT_TYPE portNum,
        FwEventIdType Id
    )
  {
    FW_ASSERT(portNum < this->getNum_from_FatalAnnounce(),static_cast<AssertArg>(portNum));
    this->from_FatalAnnounce_handler(
        portNum,
        Id
    );
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
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
  // Command: ALOG_SET_EVENT_REPORT_FILTER
  // ---------------------------------------------------------------------- 

  void ActiveLoggerTesterBase ::
    sendCmd_ALOG_SET_EVENT_REPORT_FILTER(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        ActiveLoggerComponentBase::InputFilterLevel FilterLevel,
        ActiveLoggerComponentBase::InputFilterEnabled FilterEnable
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize((FwEnumStoreType) FilterLevel);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize((FwEnumStoreType) FilterEnable);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = ActiveLoggerComponentBase::OPCODE_ALOG_SET_EVENT_REPORT_FILTER + idBase;

    if (this->m_to_CmdDisp[0].isConnected()) {
      this->m_to_CmdDisp[0].invoke(
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
  // Command: ALOG_SET_EVENT_SEND_FILTER
  // ---------------------------------------------------------------------- 

  void ActiveLoggerTesterBase ::
    sendCmd_ALOG_SET_EVENT_SEND_FILTER(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        ActiveLoggerComponentBase::SendFilterLevel FilterLevel,
        ActiveLoggerComponentBase::SendFilterEnabled FilterEnable
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize((FwEnumStoreType) FilterLevel);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize((FwEnumStoreType) FilterEnable);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = ActiveLoggerComponentBase::OPCODE_ALOG_SET_EVENT_SEND_FILTER + idBase;

    if (this->m_to_CmdDisp[0].isConnected()) {
      this->m_to_CmdDisp[0].invoke(
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
  // Command: ALOG_DUMP_EVENT_LOG
  // ---------------------------------------------------------------------- 

  void ActiveLoggerTesterBase ::
    sendCmd_ALOG_DUMP_EVENT_LOG(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        const Fw::CmdStringArg& filename
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(filename);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = ActiveLoggerComponentBase::OPCODE_ALOG_DUMP_EVENT_LOG + idBase;

    if (this->m_to_CmdDisp[0].isConnected()) {
      this->m_to_CmdDisp[0].invoke(
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
  // Command: ALOG_SET_ID_FILTER
  // ---------------------------------------------------------------------- 

  void ActiveLoggerTesterBase ::
    sendCmd_ALOG_SET_ID_FILTER(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        U32 ID,
        ActiveLoggerComponentBase::IdFilterEnabled IdFilterEnable
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(ID);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize((FwEnumStoreType) IdFilterEnable);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = ActiveLoggerComponentBase::OPCODE_ALOG_SET_ID_FILTER + idBase;

    if (this->m_to_CmdDisp[0].isConnected()) {
      this->m_to_CmdDisp[0].invoke(
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
  // Command: ALOG_DUMP_FILTER_STATE
  // ---------------------------------------------------------------------- 

  void ActiveLoggerTesterBase ::
    sendCmd_ALOG_DUMP_FILTER_STATE(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = ActiveLoggerComponentBase::OPCODE_ALOG_DUMP_FILTER_STATE + idBase;

    if (this->m_to_CmdDisp[0].isConnected()) {
      this->m_to_CmdDisp[0].invoke(
          _opcode,
          cmdSeq,
          buff
      );
    }
    else {
      printf("Test Command Output port not connected!\n");
    }

  }

  
  void ActiveLoggerTesterBase ::
    sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args) {
       
    const U32 idBase = this->getIdBase();   
    FwOpcodeType _opcode = opcode + idBase;
    if (this->m_to_CmdDisp[0].isConnected()) {
      this->m_to_CmdDisp[0].invoke(
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

  void ActiveLoggerTesterBase ::
    clearHistory()
  {
    this->cmdResponseHistory->clear();
    this->textLogHistory->clear();
    this->clearEvents();
    this->clearFromPortHistory();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
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

      case ActiveLoggerComponentBase::EVENTID_ALOG_FILE_WRITE_ERR: 
      {

        Fw::SerializeStatus _status;
        FwEnumStoreType stageInt;
        _status = args.deserialize(stageInt);
        ActiveLoggerComponentBase::LogWriteError stage = static_cast<ActiveLoggerComponentBase::LogWriteError>(stageInt);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 error;
        _status = args.deserialize(error);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_ALOG_FILE_WRITE_ERR(stage, error);

        break;

      }

      case ActiveLoggerComponentBase::EVENTID_ALOG_FILE_WRITE_COMPLETE: 
      {

        Fw::SerializeStatus _status;
        U32 records;
        _status = args.deserialize(records);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_ALOG_FILE_WRITE_COMPLETE(records);

        break;

      }

      case ActiveLoggerComponentBase::EVENTID_ALOG_SEVERITY_FILTER_STATE: 
      {

        Fw::SerializeStatus _status;
        FwEnumStoreType severityInt;
        _status = args.deserialize(severityInt);
        ActiveLoggerComponentBase::EventFilterState severity = static_cast<ActiveLoggerComponentBase::EventFilterState>(severityInt);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        bool recvEnabled;
        _status = args.deserialize(recvEnabled);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        bool sendEnabled;
        _status = args.deserialize(sendEnabled);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_LO_ALOG_SEVERITY_FILTER_STATE(severity, recvEnabled, sendEnabled);

        break;

      }

      case ActiveLoggerComponentBase::EVENTID_ALOG_ID_FILTER_ENABLED: 
      {

        Fw::SerializeStatus _status;
        U32 ID;
        _status = args.deserialize(ID);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_ALOG_ID_FILTER_ENABLED(ID);

        break;

      }

      case ActiveLoggerComponentBase::EVENTID_ALOG_ID_FILTER_LIST_FULL: 
      {

        Fw::SerializeStatus _status;
        U32 ID;
        _status = args.deserialize(ID);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_LO_ALOG_ID_FILTER_LIST_FULL(ID);

        break;

      }

      case ActiveLoggerComponentBase::EVENTID_ALOG_ID_FILTER_REMOVED: 
      {

        Fw::SerializeStatus _status;
        U32 ID;
        _status = args.deserialize(ID);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_ALOG_ID_FILTER_REMOVED(ID);

        break;

      }

      case ActiveLoggerComponentBase::EVENTID_ALOG_ID_FILTER_NOT_FOUND: 
      {

        Fw::SerializeStatus _status;
        U32 ID;
        _status = args.deserialize(ID);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_LO_ALOG_ID_FILTER_NOT_FOUND(ID);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void ActiveLoggerTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_ALOG_FILE_WRITE_ERR->clear();
    this->eventHistory_ALOG_FILE_WRITE_COMPLETE->clear();
    this->eventHistory_ALOG_SEVERITY_FILTER_STATE->clear();
    this->eventHistory_ALOG_ID_FILTER_ENABLED->clear();
    this->eventHistory_ALOG_ID_FILTER_LIST_FULL->clear();
    this->eventHistory_ALOG_ID_FILTER_REMOVED->clear();
    this->eventHistory_ALOG_ID_FILTER_NOT_FOUND->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
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

  void ActiveLoggerTesterBase ::
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

  void ActiveLoggerTesterBase ::
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
  // Event: ALOG_FILE_WRITE_ERR 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    logIn_WARNING_HI_ALOG_FILE_WRITE_ERR(
        ActiveLoggerComponentBase::LogWriteError stage,
        I32 error
    )
  {
    EventEntry_ALOG_FILE_WRITE_ERR e = {
      stage, error
    };
    eventHistory_ALOG_FILE_WRITE_ERR->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_FILE_WRITE_COMPLETE 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    logIn_ACTIVITY_HI_ALOG_FILE_WRITE_COMPLETE(
        U32 records
    )
  {
    EventEntry_ALOG_FILE_WRITE_COMPLETE e = {
      records
    };
    eventHistory_ALOG_FILE_WRITE_COMPLETE->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_SEVERITY_FILTER_STATE 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    logIn_ACTIVITY_LO_ALOG_SEVERITY_FILTER_STATE(
        ActiveLoggerComponentBase::EventFilterState severity,
        bool recvEnabled,
        bool sendEnabled
    )
  {
    EventEntry_ALOG_SEVERITY_FILTER_STATE e = {
      severity, recvEnabled, sendEnabled
    };
    eventHistory_ALOG_SEVERITY_FILTER_STATE->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_ID_FILTER_ENABLED 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    logIn_ACTIVITY_HI_ALOG_ID_FILTER_ENABLED(
        U32 ID
    )
  {
    EventEntry_ALOG_ID_FILTER_ENABLED e = {
      ID
    };
    eventHistory_ALOG_ID_FILTER_ENABLED->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_ID_FILTER_LIST_FULL 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    logIn_WARNING_LO_ALOG_ID_FILTER_LIST_FULL(
        U32 ID
    )
  {
    EventEntry_ALOG_ID_FILTER_LIST_FULL e = {
      ID
    };
    eventHistory_ALOG_ID_FILTER_LIST_FULL->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_ID_FILTER_REMOVED 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    logIn_ACTIVITY_HI_ALOG_ID_FILTER_REMOVED(
        U32 ID
    )
  {
    EventEntry_ALOG_ID_FILTER_REMOVED e = {
      ID
    };
    eventHistory_ALOG_ID_FILTER_REMOVED->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: ALOG_ID_FILTER_NOT_FOUND 
  // ----------------------------------------------------------------------

  void ActiveLoggerTesterBase ::
    logIn_WARNING_LO_ALOG_ID_FILTER_NOT_FOUND(
        U32 ID
    )
  {
    EventEntry_ALOG_ID_FILTER_NOT_FOUND e = {
      ID
    };
    eventHistory_ALOG_ID_FILTER_NOT_FOUND->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Svc
