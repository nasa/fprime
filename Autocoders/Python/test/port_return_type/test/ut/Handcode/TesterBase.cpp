// ======================================================================
// \title  TestEvent/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for TestEvent component test harness base class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <stdlib.h>
#include <string.h>
#include "TesterBase.hpp"

namespace Tlm {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestEventTesterBase ::
    TestEventTesterBase(
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
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_SomeEvent =
      new History<EventEntry_SomeEvent>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  TestEventTesterBase ::
    ~TestEventTesterBase(void)
  {
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_SomeEvent;
  }

  void TestEventTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

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

    // Initialize output port aport

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_aport();
        ++_port
    ) {
      this->m_to_aport[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_aport[%d]",
          this->m_objName,
          _port
      );
      this->m_to_aport[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TestEventTesterBase ::
    getNum_to_aport(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_aport);
  }

  NATIVE_INT_TYPE TestEventTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE TestEventTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE TestEventTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void TestEventTesterBase ::
    connect_to_aport(
        const NATIVE_INT_TYPE portNum,
        Another::InputTest4Port *const aport
    )
  {
    FW_ASSERT(portNum < this->getNum_to_aport(),static_cast<AssertArg>(portNum));
    this->m_to_aport[portNum].addCallPort(aport);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void TestEventTesterBase ::
    invoke_to_aport(
        const NATIVE_INT_TYPE portNum,
        I32 arg4,
        F32 arg5,
        U8 arg6
    )
  {
    FW_ASSERT(portNum < this->getNum_to_aport(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_aport(),static_cast<AssertArg>(portNum));
    this->m_to_aport[portNum].invoke(
        arg4, arg5, arg6
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool TestEventTesterBase ::
    isConnected_to_aport(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_aport(), static_cast<AssertArg>(portNum));
    return this->m_to_aport[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  Fw::InputTimePort *TestEventTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputLogPort *TestEventTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *TestEventTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void TestEventTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    TestEventTesterBase* _testerBase =
      static_cast<TestEventTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void TestEventTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    TestEventTesterBase* _testerBase =
      static_cast<TestEventTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  Fw::Time TestEventTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum
    )
  {
    TestEventTesterBase* _testerBase =
      static_cast<TestEventTesterBase*>(component);
    return _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // History
  // ----------------------------------------------------------------------

  void TestEventTesterBase ::
    clearHistory()
  {
    this->textLogHistory->clear();
    this->clearEvents();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void TestEventTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void TestEventTesterBase ::
    dispatchEvents(
        const FwEventIdType id,
        Fw::Time &timeTag,
        const Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {

    args.resetDeser();

    switch (id) {

      case TestEventComponentBase::EVENTID_SOMEEVENT:
      {

        Fw::SerializeStatus _status;
        U32 arg1;
        _status = args.deserialize(arg1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_SomeEvent(arg1);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void TestEventTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_SomeEvent->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events
  // ----------------------------------------------------------------------

  void TestEventTesterBase ::
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

  void TestEventTesterBase ::
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

  void TestEventTesterBase ::
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
  // Event: SomeEvent
  // ----------------------------------------------------------------------

  void TestEventTesterBase ::
    logIn_ACTIVITY_HI_SomeEvent(
        U32 arg1
    )
  {
    EventEntry_SomeEvent e = {
      arg1
    };
    eventHistory_SomeEvent->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Tlm
