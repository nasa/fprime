// ======================================================================
// \title  ActiveRateGroup/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for ActiveRateGroup component test harness base class
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

  ActiveRateGroupTesterBase ::
    ActiveRateGroupTesterBase(
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
    // Initialize telemetry histories
    this->tlmHistory_RgMaxTime = 
      new History<TlmEntry_RgMaxTime>(maxHistorySize);
    this->tlmHistory_RgCycleSlips = 
      new History<TlmEntry_RgCycleSlips>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_RateGroupCycleSlip =
      new History<EventEntry_RateGroupCycleSlip>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_RateGroupMemberOut =
      new History<FromPortEntry_RateGroupMemberOut>(maxHistorySize);
    this->fromPortHistory_PingOut =
      new History<FromPortEntry_PingOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  ActiveRateGroupTesterBase ::
    ~ActiveRateGroupTesterBase(void) 
  {
    // Destroy telemetry histories
    delete this->tlmHistory_RgMaxTime;
    delete this->tlmHistory_RgCycleSlips;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_RateGroupCycleSlip;
  }

  void ActiveRateGroupTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port RateGroupMemberOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_RateGroupMemberOut();
        ++_port
    ) {

      this->m_from_RateGroupMemberOut[_port].init();
      this->m_from_RateGroupMemberOut[_port].addCallComp(
          this,
          from_RateGroupMemberOut_static
      );
      this->m_from_RateGroupMemberOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_RateGroupMemberOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_RateGroupMemberOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port PingOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_PingOut();
        ++_port
    ) {

      this->m_from_PingOut[_port].init();
      this->m_from_PingOut[_port].addCallComp(
          this,
          from_PingOut_static
      );
      this->m_from_PingOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_PingOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_PingOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port Tlm

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_Tlm();
        ++_port
    ) {

      this->m_from_Tlm[_port].init();
      this->m_from_Tlm[_port].addCallComp(
          this,
          from_Tlm_static
      );
      this->m_from_Tlm[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_Tlm[%d]",
          this->m_objName,
          _port
      );
      this->m_from_Tlm[_port].setObjName(_portName);
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

    // Initialize output port CycleIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_CycleIn();
        ++_port
    ) {
      this->m_to_CycleIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_CycleIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_CycleIn[_port].setObjName(_portName);
#endif

    }

    // Initialize output port PingIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_PingIn();
        ++_port
    ) {
      this->m_to_PingIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_PingIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_PingIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE ActiveRateGroupTesterBase ::
    getNum_to_CycleIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CycleIn);
  }

  NATIVE_INT_TYPE ActiveRateGroupTesterBase ::
    getNum_from_RateGroupMemberOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_RateGroupMemberOut);
  }

  NATIVE_INT_TYPE ActiveRateGroupTesterBase ::
    getNum_to_PingIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_PingIn);
  }

  NATIVE_INT_TYPE ActiveRateGroupTesterBase ::
    getNum_from_PingOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_PingOut);
  }

  NATIVE_INT_TYPE ActiveRateGroupTesterBase ::
    getNum_from_Tlm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Tlm);
  }

  NATIVE_INT_TYPE ActiveRateGroupTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE ActiveRateGroupTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE ActiveRateGroupTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    connect_to_CycleIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputCyclePort *const CycleIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_CycleIn(),static_cast<AssertArg>(portNum));
    this->m_to_CycleIn[portNum].addCallPort(CycleIn);
  }

  void ActiveRateGroupTesterBase ::
    connect_to_PingIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputPingPort *const PingIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_PingIn(),static_cast<AssertArg>(portNum));
    this->m_to_PingIn[portNum].addCallPort(PingIn);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    invoke_to_CycleIn(
        const NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    FW_ASSERT(portNum < this->getNum_to_CycleIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_CycleIn(),static_cast<AssertArg>(portNum));
    this->m_to_CycleIn[portNum].invoke(
        cycleStart
    );
  }

  void ActiveRateGroupTesterBase ::
    invoke_to_PingIn(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_to_PingIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_PingIn(),static_cast<AssertArg>(portNum));
    this->m_to_PingIn[portNum].invoke(
        key
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool ActiveRateGroupTesterBase ::
    isConnected_to_CycleIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CycleIn(), static_cast<AssertArg>(portNum));
    return this->m_to_CycleIn[portNum].isConnected();
  }

  bool ActiveRateGroupTesterBase ::
    isConnected_to_PingIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_PingIn(), static_cast<AssertArg>(portNum));
    return this->m_to_PingIn[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Svc::InputSchedPort *ActiveRateGroupTesterBase ::
    get_from_RateGroupMemberOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_RateGroupMemberOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_RateGroupMemberOut[portNum];
  }

  Svc::InputPingPort *ActiveRateGroupTesterBase ::
    get_from_PingOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_PingOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_PingOut[portNum];
  }

  Fw::InputTlmPort *ActiveRateGroupTesterBase ::
    get_from_Tlm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Tlm(),static_cast<AssertArg>(portNum));
    return &this->m_from_Tlm[portNum];
  }

  Fw::InputTimePort *ActiveRateGroupTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputLogPort *ActiveRateGroupTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *ActiveRateGroupTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    from_RateGroupMemberOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    FW_ASSERT(callComp);
    ActiveRateGroupTesterBase* _testerBase = 
      static_cast<ActiveRateGroupTesterBase*>(callComp);
    _testerBase->from_RateGroupMemberOut_handlerBase(
        portNum,
        context
    );
  }

  void ActiveRateGroupTesterBase ::
    from_PingOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(callComp);
    ActiveRateGroupTesterBase* _testerBase = 
      static_cast<ActiveRateGroupTesterBase*>(callComp);
    _testerBase->from_PingOut_handlerBase(
        portNum,
        key
    );
  }

  void ActiveRateGroupTesterBase ::
    from_Tlm_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    ActiveRateGroupTesterBase* _testerBase =
      static_cast<ActiveRateGroupTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void ActiveRateGroupTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    ActiveRateGroupTesterBase* _testerBase =
      static_cast<ActiveRateGroupTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void ActiveRateGroupTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    ActiveRateGroupTesterBase* _testerBase =
      static_cast<ActiveRateGroupTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void ActiveRateGroupTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    ActiveRateGroupTesterBase* _testerBase =
      static_cast<ActiveRateGroupTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_RateGroupMemberOut->clear();
    this->fromPortHistory_PingOut->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: RateGroupMemberOut
  // ---------------------------------------------------------------------- 

  void ActiveRateGroupTesterBase ::
    pushFromPortEntry_RateGroupMemberOut(
        NATIVE_UINT_TYPE context
    )
  {
    FromPortEntry_RateGroupMemberOut _e = {
      context
    };
    this->fromPortHistory_RateGroupMemberOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ---------------------------------------------------------------------- 
  // From port: PingOut
  // ---------------------------------------------------------------------- 

  void ActiveRateGroupTesterBase ::
    pushFromPortEntry_PingOut(
        U32 key
    )
  {
    FromPortEntry_PingOut _e = {
      key
    };
    this->fromPortHistory_PingOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    from_RateGroupMemberOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    FW_ASSERT(portNum < this->getNum_from_RateGroupMemberOut(),static_cast<AssertArg>(portNum));
    this->from_RateGroupMemberOut_handler(
        portNum,
        context
    );
  }

  void ActiveRateGroupTesterBase ::
    from_PingOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_from_PingOut(),static_cast<AssertArg>(portNum));
    this->from_PingOut_handler(
        portNum,
        key
    );
  }

  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    clearHistory()
  {
    this->clearTlm();
    this->textLogHistory->clear();
    this->clearEvents();
    this->clearFromPortHistory();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
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

      case ActiveRateGroupComponentBase::CHANNELID_RGMAXTIME:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing RgMaxTime: %d\n", _status);
          return;
        }
        this->tlmInput_RgMaxTime(timeTag, arg);
        break;
      }

      case ActiveRateGroupComponentBase::CHANNELID_RGCYCLESLIPS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing RgCycleSlips: %d\n", _status);
          return;
        }
        this->tlmInput_RgCycleSlips(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void ActiveRateGroupTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_RgMaxTime->clear();
    this->tlmHistory_RgCycleSlips->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: RgMaxTime
  // ---------------------------------------------------------------------- 

  void ActiveRateGroupTesterBase ::
    tlmInput_RgMaxTime(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_RgMaxTime e = { timeTag, val };
    this->tlmHistory_RgMaxTime->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: RgCycleSlips
  // ---------------------------------------------------------------------- 

  void ActiveRateGroupTesterBase ::
    tlmInput_RgCycleSlips(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_RgCycleSlips e = { timeTag, val };
    this->tlmHistory_RgCycleSlips->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
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

      case ActiveRateGroupComponentBase::EVENTID_RATEGROUPSTARTED: 
      {

        this->logIn_DIAGNOSTIC_RateGroupStarted();

        break;

      }

      case ActiveRateGroupComponentBase::EVENTID_RATEGROUPCYCLESLIP: 
      {

        Fw::SerializeStatus _status;
        U32 cycle;
        _status = args.deserialize(cycle);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_RateGroupCycleSlip(cycle);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void ActiveRateGroupTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventsSize_RateGroupStarted = 0;
    this->eventHistory_RateGroupCycleSlip->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
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

  void ActiveRateGroupTesterBase ::
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

  void ActiveRateGroupTesterBase ::
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
  // Event: RateGroupStarted 
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    logIn_DIAGNOSTIC_RateGroupStarted(
        void
    )
  {
    ++this->eventsSize_RateGroupStarted;
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: RateGroupCycleSlip 
  // ----------------------------------------------------------------------

  void ActiveRateGroupTesterBase ::
    logIn_WARNING_HI_RateGroupCycleSlip(
        U32 cycle
    )
  {
    EventEntry_RateGroupCycleSlip e = {
      cycle
    };
    eventHistory_RateGroupCycleSlip->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Svc
