// ======================================================================
// \title  BufferManager/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for BufferManager component test harness base class
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

  BufferManagerTesterBase ::
    BufferManagerTesterBase(
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
    this->tlmHistory_BufferManager_NumAllocatedBuffers = 
      new History<TlmEntry_BufferManager_NumAllocatedBuffers>(maxHistorySize);
    this->tlmHistory_BufferManager_AllocatedSize = 
      new History<TlmEntry_BufferManager_AllocatedSize>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    // Clear history
    this->clearHistory();
  }

  BufferManagerTesterBase ::
    ~BufferManagerTesterBase(void) 
  {
    // Destroy telemetry histories
    delete this->tlmHistory_BufferManager_NumAllocatedBuffers;
    delete this->tlmHistory_BufferManager_AllocatedSize;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
  }

  void BufferManagerTesterBase ::
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

    // Attach input port eventOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_eventOut();
        ++_port
    ) {

      this->m_from_eventOut[_port].init();
      this->m_from_eventOut[_port].addCallComp(
          this,
          from_eventOut_static
      );
      this->m_from_eventOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_eventOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_eventOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port textEventOut

#if FW_ENABLE_TEXT_LOGGING == 1
    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_textEventOut();
        ++_port
    ) {

      this->m_from_textEventOut[_port].init();
      this->m_from_textEventOut[_port].addCallComp(
          this,
          from_textEventOut_static
      );
      this->m_from_textEventOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_textEventOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_textEventOut[_port].setObjName(_portName);
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

    // Initialize output port bufferSendIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_bufferSendIn();
        ++_port
    ) {
      this->m_to_bufferSendIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_bufferSendIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_bufferSendIn[_port].setObjName(_portName);
#endif

    }

    // Initialize output port bufferGetCallee

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_bufferGetCallee();
        ++_port
    ) {
      this->m_to_bufferGetCallee[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_bufferGetCallee[%d]",
          this->m_objName,
          _port
      );
      this->m_to_bufferGetCallee[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE BufferManagerTesterBase ::
    getNum_from_timeCaller(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_timeCaller);
  }

  NATIVE_INT_TYPE BufferManagerTesterBase ::
    getNum_from_eventOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_eventOut);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE BufferManagerTesterBase ::
    getNum_from_textEventOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_textEventOut);
  }
#endif

  NATIVE_INT_TYPE BufferManagerTesterBase ::
    getNum_to_bufferSendIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_bufferSendIn);
  }

  NATIVE_INT_TYPE BufferManagerTesterBase ::
    getNum_to_bufferGetCallee(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_bufferGetCallee);
  }

  NATIVE_INT_TYPE BufferManagerTesterBase ::
    getNum_from_tlmOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_tlmOut);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
    connect_to_bufferSendIn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputBufferSendPort *const bufferSendIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendIn(),static_cast<AssertArg>(portNum));
    this->m_to_bufferSendIn[portNum].addCallPort(bufferSendIn);
  }

  void BufferManagerTesterBase ::
    connect_to_bufferGetCallee(
        const NATIVE_INT_TYPE portNum,
        Fw::InputBufferGetPort *const bufferGetCallee
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_bufferGetCallee(),static_cast<AssertArg>(portNum));
    this->m_to_bufferGetCallee[portNum].addCallPort(bufferGetCallee);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
    invoke_to_bufferSendIn(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_bufferSendIn(),static_cast<AssertArg>(portNum));
    this->m_to_bufferSendIn[portNum].invoke(
        fwBuffer
    );
  }

  Fw::Buffer BufferManagerTesterBase ::
    invoke_to_bufferGetCallee(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    FW_ASSERT(portNum < this->getNum_to_bufferGetCallee(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_bufferGetCallee(),static_cast<AssertArg>(portNum));
    return this->m_to_bufferGetCallee[portNum].invoke(
        size
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool BufferManagerTesterBase ::
    isConnected_to_bufferSendIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendIn(), static_cast<AssertArg>(portNum));
    return this->m_to_bufferSendIn[portNum].isConnected();
  }

  bool BufferManagerTesterBase ::
    isConnected_to_bufferGetCallee(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_bufferGetCallee(), static_cast<AssertArg>(portNum));
    return this->m_to_bufferGetCallee[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputTimePort *BufferManagerTesterBase ::
    get_from_timeCaller(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_timeCaller(),static_cast<AssertArg>(portNum));
    return &this->m_from_timeCaller[portNum];
  }

  Fw::InputLogPort *BufferManagerTesterBase ::
    get_from_eventOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_eventOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_eventOut[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *BufferManagerTesterBase ::
    get_from_textEventOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_textEventOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_textEventOut[portNum];
  }
#endif

  Fw::InputTlmPort *BufferManagerTesterBase ::
    get_from_tlmOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_tlmOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_tlmOut[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
    from_tlmOut_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    BufferManagerTesterBase* _testerBase =
      static_cast<BufferManagerTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void BufferManagerTesterBase ::
    from_eventOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    BufferManagerTesterBase* _testerBase =
      static_cast<BufferManagerTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void BufferManagerTesterBase ::
    from_textEventOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    BufferManagerTesterBase* _testerBase =
      static_cast<BufferManagerTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void BufferManagerTesterBase ::
    from_timeCaller_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    BufferManagerTesterBase* _testerBase =
      static_cast<BufferManagerTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
    clearHistory()
  {
    this->clearTlm();
    this->textLogHistory->clear();
    this->clearEvents();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
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

      case BufferManagerComponentBase::CHANNELID_BUFFERMANAGER_NUMALLOCATEDBUFFERS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing BufferManager_NumAllocatedBuffers: %d\n", _status);
          return;
        }
        this->tlmInput_BufferManager_NumAllocatedBuffers(timeTag, arg);
        break;
      }

      case BufferManagerComponentBase::CHANNELID_BUFFERMANAGER_ALLOCATEDSIZE:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing BufferManager_AllocatedSize: %d\n", _status);
          return;
        }
        this->tlmInput_BufferManager_AllocatedSize(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void BufferManagerTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_BufferManager_NumAllocatedBuffers->clear();
    this->tlmHistory_BufferManager_AllocatedSize->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: BufferManager_NumAllocatedBuffers
  // ---------------------------------------------------------------------- 

  void BufferManagerTesterBase ::
    tlmInput_BufferManager_NumAllocatedBuffers(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_BufferManager_NumAllocatedBuffers e = { timeTag, val };
    this->tlmHistory_BufferManager_NumAllocatedBuffers->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: BufferManager_AllocatedSize
  // ---------------------------------------------------------------------- 

  void BufferManagerTesterBase ::
    tlmInput_BufferManager_AllocatedSize(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_BufferManager_AllocatedSize e = { timeTag, val };
    this->tlmHistory_BufferManager_AllocatedSize->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
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

      case BufferManagerComponentBase::EVENTID_CLEAREDERRORSTATE: 
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
        this->logIn_ACTIVITY_HI_ClearedErrorState();

        break;

      }

      case BufferManagerComponentBase::EVENTID_STORESIZEEXCEEDED: 
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
        this->logIn_WARNING_HI_StoreSizeExceeded();

        break;

      }

      case BufferManagerComponentBase::EVENTID_TOOMANYBUFFERS: 
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
        this->logIn_WARNING_HI_TooManyBuffers();

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void BufferManagerTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventsSize_ClearedErrorState = 0;
    this->eventsSize_StoreSizeExceeded = 0;
    this->eventsSize_TooManyBuffers = 0;
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
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

  void BufferManagerTesterBase ::
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

  void BufferManagerTesterBase ::
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
  // Event: ClearedErrorState 
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
    logIn_ACTIVITY_HI_ClearedErrorState(
        void
    )
  {
    ++this->eventsSize_ClearedErrorState;
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: StoreSizeExceeded 
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
    logIn_WARNING_HI_StoreSizeExceeded(
        void
    )
  {
    ++this->eventsSize_StoreSizeExceeded;
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: TooManyBuffers 
  // ----------------------------------------------------------------------

  void BufferManagerTesterBase ::
    logIn_WARNING_HI_TooManyBuffers(
        void
    )
  {
    ++this->eventsSize_TooManyBuffers;
    ++this->eventsSize;
  }

} // end namespace Svc
