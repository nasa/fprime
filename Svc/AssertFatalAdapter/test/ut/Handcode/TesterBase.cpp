// ======================================================================
// \title  AssertFatalAdapter/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for AssertFatalAdapter component test harness base class
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

  AssertFatalAdapterTesterBase ::
    AssertFatalAdapterTesterBase(
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
    this->eventHistory_AF_ASSERT_0 =
      new History<EventEntry_AF_ASSERT_0>(maxHistorySize);
    this->eventHistory_AF_ASSERT_1 =
      new History<EventEntry_AF_ASSERT_1>(maxHistorySize);
    this->eventHistory_AF_ASSERT_2 =
      new History<EventEntry_AF_ASSERT_2>(maxHistorySize);
    this->eventHistory_AF_ASSERT_3 =
      new History<EventEntry_AF_ASSERT_3>(maxHistorySize);
    this->eventHistory_AF_ASSERT_4 =
      new History<EventEntry_AF_ASSERT_4>(maxHistorySize);
    this->eventHistory_AF_ASSERT_5 =
      new History<EventEntry_AF_ASSERT_5>(maxHistorySize);
    this->eventHistory_AF_ASSERT_6 =
      new History<EventEntry_AF_ASSERT_6>(maxHistorySize);
    this->eventHistory_AF_UNEXPECTED_ASSERT =
      new History<EventEntry_AF_UNEXPECTED_ASSERT>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  AssertFatalAdapterTesterBase ::
    ~AssertFatalAdapterTesterBase(void) 
  {
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_AF_ASSERT_0;
    delete this->eventHistory_AF_ASSERT_1;
    delete this->eventHistory_AF_ASSERT_2;
    delete this->eventHistory_AF_ASSERT_3;
    delete this->eventHistory_AF_ASSERT_4;
    delete this->eventHistory_AF_ASSERT_5;
    delete this->eventHistory_AF_ASSERT_6;
    delete this->eventHistory_AF_UNEXPECTED_ASSERT;
  }

  void AssertFatalAdapterTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
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

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE AssertFatalAdapterTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE AssertFatalAdapterTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE AssertFatalAdapterTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif


  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputTimePort *AssertFatalAdapterTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputLogPort *AssertFatalAdapterTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *AssertFatalAdapterTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    AssertFatalAdapterTesterBase* _testerBase =
      static_cast<AssertFatalAdapterTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void AssertFatalAdapterTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    AssertFatalAdapterTesterBase* _testerBase =
      static_cast<AssertFatalAdapterTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void AssertFatalAdapterTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    AssertFatalAdapterTesterBase* _testerBase =
      static_cast<AssertFatalAdapterTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    clearHistory()
  {
    this->textLogHistory->clear();
    this->clearEvents();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
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

      case AssertFatalAdapterComponentBase::EVENTID_AF_ASSERT_0: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 line;
        _status = args.deserialize(line);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_AF_ASSERT_0(file, line);

        break;

      }

      case AssertFatalAdapterComponentBase::EVENTID_AF_ASSERT_1: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 line;
        _status = args.deserialize(line);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg1;
        _status = args.deserialize(arg1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_AF_ASSERT_1(file, line, arg1);

        break;

      }

      case AssertFatalAdapterComponentBase::EVENTID_AF_ASSERT_2: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 line;
        _status = args.deserialize(line);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg1;
        _status = args.deserialize(arg1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg2;
        _status = args.deserialize(arg2);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_AF_ASSERT_2(file, line, arg1, arg2);

        break;

      }

      case AssertFatalAdapterComponentBase::EVENTID_AF_ASSERT_3: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 line;
        _status = args.deserialize(line);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg1;
        _status = args.deserialize(arg1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg2;
        _status = args.deserialize(arg2);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg3;
        _status = args.deserialize(arg3);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_AF_ASSERT_3(file, line, arg1, arg2, arg3);

        break;

      }

      case AssertFatalAdapterComponentBase::EVENTID_AF_ASSERT_4: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 line;
        _status = args.deserialize(line);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg1;
        _status = args.deserialize(arg1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg2;
        _status = args.deserialize(arg2);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg3;
        _status = args.deserialize(arg3);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg4;
        _status = args.deserialize(arg4);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_AF_ASSERT_4(file, line, arg1, arg2, arg3, arg4);

        break;

      }

      case AssertFatalAdapterComponentBase::EVENTID_AF_ASSERT_5: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 line;
        _status = args.deserialize(line);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg1;
        _status = args.deserialize(arg1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg2;
        _status = args.deserialize(arg2);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg3;
        _status = args.deserialize(arg3);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg4;
        _status = args.deserialize(arg4);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg5;
        _status = args.deserialize(arg5);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_AF_ASSERT_5(file, line, arg1, arg2, arg3, arg4, arg5);

        break;

      }

      case AssertFatalAdapterComponentBase::EVENTID_AF_ASSERT_6: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 line;
        _status = args.deserialize(line);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg1;
        _status = args.deserialize(arg1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg2;
        _status = args.deserialize(arg2);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg3;
        _status = args.deserialize(arg3);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg4;
        _status = args.deserialize(arg4);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg5;
        _status = args.deserialize(arg5);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 arg6;
        _status = args.deserialize(arg6);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_AF_ASSERT_6(file, line, arg1, arg2, arg3, arg4, arg5, arg6);

        break;

      }

      case AssertFatalAdapterComponentBase::EVENTID_AF_UNEXPECTED_ASSERT: 
      {

        Fw::SerializeStatus _status;
        Fw::LogStringArg file;
        _status = args.deserialize(file);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 line;
        _status = args.deserialize(line);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        U32 numArgs;
        _status = args.deserialize(numArgs);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_AF_UNEXPECTED_ASSERT(file, line, numArgs);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void AssertFatalAdapterTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_AF_ASSERT_0->clear();
    this->eventHistory_AF_ASSERT_1->clear();
    this->eventHistory_AF_ASSERT_2->clear();
    this->eventHistory_AF_ASSERT_3->clear();
    this->eventHistory_AF_ASSERT_4->clear();
    this->eventHistory_AF_ASSERT_5->clear();
    this->eventHistory_AF_ASSERT_6->clear();
    this->eventHistory_AF_UNEXPECTED_ASSERT->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
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

  void AssertFatalAdapterTesterBase ::
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

  void AssertFatalAdapterTesterBase ::
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
  // Event: AF_ASSERT_0 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    logIn_FATAL_AF_ASSERT_0(
        Fw::LogStringArg& file,
        U32 line
    )
  {
    EventEntry_AF_ASSERT_0 e = {
      file, line
    };
    eventHistory_AF_ASSERT_0->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: AF_ASSERT_1 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    logIn_FATAL_AF_ASSERT_1(
        Fw::LogStringArg& file,
        U32 line,
        U32 arg1
    )
  {
    EventEntry_AF_ASSERT_1 e = {
      file, line, arg1
    };
    eventHistory_AF_ASSERT_1->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: AF_ASSERT_2 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    logIn_FATAL_AF_ASSERT_2(
        Fw::LogStringArg& file,
        U32 line,
        U32 arg1,
        U32 arg2
    )
  {
    EventEntry_AF_ASSERT_2 e = {
      file, line, arg1, arg2
    };
    eventHistory_AF_ASSERT_2->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: AF_ASSERT_3 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    logIn_FATAL_AF_ASSERT_3(
        Fw::LogStringArg& file,
        U32 line,
        U32 arg1,
        U32 arg2,
        U32 arg3
    )
  {
    EventEntry_AF_ASSERT_3 e = {
      file, line, arg1, arg2, arg3
    };
    eventHistory_AF_ASSERT_3->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: AF_ASSERT_4 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    logIn_FATAL_AF_ASSERT_4(
        Fw::LogStringArg& file,
        U32 line,
        U32 arg1,
        U32 arg2,
        U32 arg3,
        U32 arg4
    )
  {
    EventEntry_AF_ASSERT_4 e = {
      file, line, arg1, arg2, arg3, arg4
    };
    eventHistory_AF_ASSERT_4->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: AF_ASSERT_5 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    logIn_FATAL_AF_ASSERT_5(
        Fw::LogStringArg& file,
        U32 line,
        U32 arg1,
        U32 arg2,
        U32 arg3,
        U32 arg4,
        U32 arg5
    )
  {
    EventEntry_AF_ASSERT_5 e = {
      file, line, arg1, arg2, arg3, arg4, arg5
    };
    eventHistory_AF_ASSERT_5->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: AF_ASSERT_6 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    logIn_FATAL_AF_ASSERT_6(
        Fw::LogStringArg& file,
        U32 line,
        U32 arg1,
        U32 arg2,
        U32 arg3,
        U32 arg4,
        U32 arg5,
        U32 arg6
    )
  {
    EventEntry_AF_ASSERT_6 e = {
      file, line, arg1, arg2, arg3, arg4, arg5, arg6
    };
    eventHistory_AF_ASSERT_6->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: AF_UNEXPECTED_ASSERT 
  // ----------------------------------------------------------------------

  void AssertFatalAdapterTesterBase ::
    logIn_FATAL_AF_UNEXPECTED_ASSERT(
        Fw::LogStringArg& file,
        U32 line,
        U32 numArgs
    )
  {
    EventEntry_AF_UNEXPECTED_ASSERT e = {
      file, line, numArgs
    };
    eventHistory_AF_UNEXPECTED_ASSERT->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Svc
