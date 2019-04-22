// ======================================================================
// \title  PrmDb/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for PrmDb component test harness base class
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

  PrmDbTesterBase ::
    PrmDbTesterBase(
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
    this->eventHistory_PrmIdNotFound =
      new History<EventEntry_PrmIdNotFound>(maxHistorySize);
    this->eventHistory_PrmIdUpdated =
      new History<EventEntry_PrmIdUpdated>(maxHistorySize);
    this->eventHistory_PrmDbFull =
      new History<EventEntry_PrmDbFull>(maxHistorySize);
    this->eventHistory_PrmIdAdded =
      new History<EventEntry_PrmIdAdded>(maxHistorySize);
    this->eventHistory_PrmFileWriteError =
      new History<EventEntry_PrmFileWriteError>(maxHistorySize);
    this->eventHistory_PrmFileSaveComplete =
      new History<EventEntry_PrmFileSaveComplete>(maxHistorySize);
    this->eventHistory_PrmFileReadError =
      new History<EventEntry_PrmFileReadError>(maxHistorySize);
    this->eventHistory_PrmFileLoadComplete =
      new History<EventEntry_PrmFileLoadComplete>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  PrmDbTesterBase ::
    ~PrmDbTesterBase(void) 
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_PrmIdNotFound;
    delete this->eventHistory_PrmIdUpdated;
    delete this->eventHistory_PrmDbFull;
    delete this->eventHistory_PrmIdAdded;
    delete this->eventHistory_PrmFileWriteError;
    delete this->eventHistory_PrmFileSaveComplete;
    delete this->eventHistory_PrmFileReadError;
    delete this->eventHistory_PrmFileLoadComplete;
  }

  void PrmDbTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

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

    // Initialize output port getPrm

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_getPrm();
        ++_port
    ) {
      this->m_to_getPrm[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_getPrm[%d]",
          this->m_objName,
          _port
      );
      this->m_to_getPrm[_port].setObjName(_portName);
#endif

    }

    // Initialize output port setPrm

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_setPrm();
        ++_port
    ) {
      this->m_to_setPrm[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_setPrm[%d]",
          this->m_objName,
          _port
      );
      this->m_to_setPrm[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE PrmDbTesterBase ::
    getNum_to_getPrm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_getPrm);
  }

  NATIVE_INT_TYPE PrmDbTesterBase ::
    getNum_to_setPrm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_setPrm);
  }

  NATIVE_INT_TYPE PrmDbTesterBase ::
    getNum_to_CmdDisp(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CmdDisp);
  }

  NATIVE_INT_TYPE PrmDbTesterBase ::
    getNum_from_CmdStatus(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdStatus);
  }

  NATIVE_INT_TYPE PrmDbTesterBase ::
    getNum_from_CmdReg(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdReg);
  }

  NATIVE_INT_TYPE PrmDbTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE PrmDbTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE PrmDbTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    connect_to_getPrm(
        const NATIVE_INT_TYPE portNum,
        Fw::InputPrmGetPort *const getPrm
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_getPrm(),static_cast<AssertArg>(portNum));
    this->m_to_getPrm[portNum].addCallPort(getPrm);
  }

  void PrmDbTesterBase ::
    connect_to_setPrm(
        const NATIVE_INT_TYPE portNum,
        Fw::InputPrmSetPort *const setPrm
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_setPrm(),static_cast<AssertArg>(portNum));
    this->m_to_setPrm[portNum].addCallPort(setPrm);
  }

  void PrmDbTesterBase ::
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

  Fw::ParamValid PrmDbTesterBase ::
    invoke_to_getPrm(
        const NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    FW_ASSERT(portNum < this->getNum_to_getPrm(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_getPrm(),static_cast<AssertArg>(portNum));
    return this->m_to_getPrm[portNum].invoke(
        id, val
    );
  }

  void PrmDbTesterBase ::
    invoke_to_setPrm(
        const NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    FW_ASSERT(portNum < this->getNum_to_setPrm(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_setPrm(),static_cast<AssertArg>(portNum));
    this->m_to_setPrm[portNum].invoke(
        id, val
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool PrmDbTesterBase ::
    isConnected_to_getPrm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_getPrm(), static_cast<AssertArg>(portNum));
    return this->m_to_getPrm[portNum].isConnected();
  }

  bool PrmDbTesterBase ::
    isConnected_to_setPrm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_setPrm(), static_cast<AssertArg>(portNum));
    return this->m_to_setPrm[portNum].isConnected();
  }

  bool PrmDbTesterBase ::
    isConnected_to_CmdDisp(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(), static_cast<AssertArg>(portNum));
    return this->m_to_CmdDisp[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputCmdResponsePort *PrmDbTesterBase ::
    get_from_CmdStatus(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdStatus(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdStatus[portNum];
  }

  Fw::InputCmdRegPort *PrmDbTesterBase ::
    get_from_CmdReg(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdReg(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdReg[portNum];
  }

  Fw::InputTimePort *PrmDbTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputLogPort *PrmDbTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *PrmDbTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    from_CmdStatus_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    PrmDbTesterBase* _testerBase =
      static_cast<PrmDbTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void PrmDbTesterBase ::
    from_CmdReg_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void PrmDbTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    PrmDbTesterBase* _testerBase =
      static_cast<PrmDbTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void PrmDbTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    PrmDbTesterBase* _testerBase =
      static_cast<PrmDbTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void PrmDbTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    PrmDbTesterBase* _testerBase =
      static_cast<PrmDbTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
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
  // Command: PRM_SAVE_FILE
  // ---------------------------------------------------------------------- 

  void PrmDbTesterBase ::
    sendCmd_PRM_SAVE_FILE(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = PrmDbComponentBase::OPCODE_PRM_SAVE_FILE + idBase;

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

  
  void PrmDbTesterBase ::
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

  void PrmDbTesterBase ::
    clearHistory()
  {
    this->cmdResponseHistory->clear();
    this->textLogHistory->clear();
    this->clearEvents();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
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

      case PrmDbComponentBase::EVENTID_PRMIDNOTFOUND: 
      {

        Fw::SerializeStatus _status;
        U32 Id;
        _status = args.deserialize(Id);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_LO_PrmIdNotFound(Id);

        break;

      }

      case PrmDbComponentBase::EVENTID_PRMIDUPDATED: 
      {

        Fw::SerializeStatus _status;
        U32 Id;
        _status = args.deserialize(Id);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_PrmIdUpdated(Id);

        break;

      }

      case PrmDbComponentBase::EVENTID_PRMDBFULL: 
      {

        Fw::SerializeStatus _status;
        U32 Id;
        _status = args.deserialize(Id);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_PrmDbFull(Id);

        break;

      }

      case PrmDbComponentBase::EVENTID_PRMIDADDED: 
      {

        Fw::SerializeStatus _status;
        U32 Id;
        _status = args.deserialize(Id);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_PrmIdAdded(Id);

        break;

      }

      case PrmDbComponentBase::EVENTID_PRMFILEWRITEERROR: 
      {

        Fw::SerializeStatus _status;
        FwEnumStoreType stageInt;
        _status = args.deserialize(stageInt);
        PrmDbComponentBase::PrmWriteError stage = static_cast<PrmDbComponentBase::PrmWriteError>(stageInt);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 record;
        _status = args.deserialize(record);
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

        this->logIn_WARNING_HI_PrmFileWriteError(stage, record, error);

        break;

      }

      case PrmDbComponentBase::EVENTID_PRMFILESAVECOMPLETE: 
      {

        Fw::SerializeStatus _status;
        U32 records;
        _status = args.deserialize(records);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_PrmFileSaveComplete(records);

        break;

      }

      case PrmDbComponentBase::EVENTID_PRMFILEREADERROR: 
      {

        Fw::SerializeStatus _status;
        FwEnumStoreType stageInt;
        _status = args.deserialize(stageInt);
        PrmDbComponentBase::PrmReadError stage = static_cast<PrmDbComponentBase::PrmReadError>(stageInt);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 record;
        _status = args.deserialize(record);
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

        this->logIn_WARNING_HI_PrmFileReadError(stage, record, error);

        break;

      }

      case PrmDbComponentBase::EVENTID_PRMFILELOADCOMPLETE: 
      {

        Fw::SerializeStatus _status;
        U32 records;
        _status = args.deserialize(records);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_PrmFileLoadComplete(records);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void PrmDbTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_PrmIdNotFound->clear();
    this->eventHistory_PrmIdUpdated->clear();
    this->eventHistory_PrmDbFull->clear();
    this->eventHistory_PrmIdAdded->clear();
    this->eventHistory_PrmFileWriteError->clear();
    this->eventHistory_PrmFileSaveComplete->clear();
    this->eventHistory_PrmFileReadError->clear();
    this->eventHistory_PrmFileLoadComplete->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
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

  void PrmDbTesterBase ::
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

  void PrmDbTesterBase ::
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
  // Event: PrmIdNotFound 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    logIn_WARNING_LO_PrmIdNotFound(
        U32 Id
    )
  {
    EventEntry_PrmIdNotFound e = {
      Id
    };
    eventHistory_PrmIdNotFound->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: PrmIdUpdated 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    logIn_ACTIVITY_HI_PrmIdUpdated(
        U32 Id
    )
  {
    EventEntry_PrmIdUpdated e = {
      Id
    };
    eventHistory_PrmIdUpdated->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: PrmDbFull 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    logIn_FATAL_PrmDbFull(
        U32 Id
    )
  {
    EventEntry_PrmDbFull e = {
      Id
    };
    eventHistory_PrmDbFull->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: PrmIdAdded 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    logIn_ACTIVITY_HI_PrmIdAdded(
        U32 Id
    )
  {
    EventEntry_PrmIdAdded e = {
      Id
    };
    eventHistory_PrmIdAdded->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: PrmFileWriteError 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    logIn_WARNING_HI_PrmFileWriteError(
        PrmDbComponentBase::PrmWriteError stage,
        I32 record,
        I32 error
    )
  {
    EventEntry_PrmFileWriteError e = {
      stage, record, error
    };
    eventHistory_PrmFileWriteError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: PrmFileSaveComplete 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    logIn_ACTIVITY_HI_PrmFileSaveComplete(
        U32 records
    )
  {
    EventEntry_PrmFileSaveComplete e = {
      records
    };
    eventHistory_PrmFileSaveComplete->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: PrmFileReadError 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    logIn_WARNING_HI_PrmFileReadError(
        PrmDbComponentBase::PrmReadError stage,
        I32 record,
        I32 error
    )
  {
    EventEntry_PrmFileReadError e = {
      stage, record, error
    };
    eventHistory_PrmFileReadError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: PrmFileLoadComplete 
  // ----------------------------------------------------------------------

  void PrmDbTesterBase ::
    logIn_ACTIVITY_HI_PrmFileLoadComplete(
        U32 records
    )
  {
    EventEntry_PrmFileLoadComplete e = {
      records
    };
    eventHistory_PrmFileLoadComplete->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Svc
