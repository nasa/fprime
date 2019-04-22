// ======================================================================
// \title  LinuxSpiDriver/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for LinuxSpiDriver component test harness base class
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

namespace Drv {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  LinuxSpiDriverTesterBase ::
    LinuxSpiDriverTesterBase(
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
    this->tlmHistory_SPI_Bytes = 
      new History<TlmEntry_SPI_Bytes>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_SPI_OpenError =
      new History<EventEntry_SPI_OpenError>(maxHistorySize);
    this->eventHistory_SPI_ConfigError =
      new History<EventEntry_SPI_ConfigError>(maxHistorySize);
    this->eventHistory_SPI_WriteError =
      new History<EventEntry_SPI_WriteError>(maxHistorySize);
    this->eventHistory_SPI_PortOpened =
      new History<EventEntry_SPI_PortOpened>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  LinuxSpiDriverTesterBase ::
    ~LinuxSpiDriverTesterBase(void) 
  {
    // Destroy telemetry histories
    delete this->tlmHistory_SPI_Bytes;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_SPI_OpenError;
    delete this->eventHistory_SPI_ConfigError;
    delete this->eventHistory_SPI_WriteError;
    delete this->eventHistory_SPI_PortOpened;
  }

  void LinuxSpiDriverTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

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

    // Initialize output port SpiReadWrite

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_SpiReadWrite();
        ++_port
    ) {
      this->m_to_SpiReadWrite[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_SpiReadWrite[%d]",
          this->m_objName,
          _port
      );
      this->m_to_SpiReadWrite[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE LinuxSpiDriverTesterBase ::
    getNum_from_Tlm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Tlm);
  }

  NATIVE_INT_TYPE LinuxSpiDriverTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

  NATIVE_INT_TYPE LinuxSpiDriverTesterBase ::
    getNum_to_SpiReadWrite(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_SpiReadWrite);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE LinuxSpiDriverTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  NATIVE_INT_TYPE LinuxSpiDriverTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    connect_to_SpiReadWrite(
        const NATIVE_INT_TYPE portNum,
        Drv::InputSpiReadWritePort *const SpiReadWrite
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_SpiReadWrite(),static_cast<AssertArg>(portNum));
    this->m_to_SpiReadWrite[portNum].addCallPort(SpiReadWrite);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    invoke_to_SpiReadWrite(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &writeBuffer,
        Fw::Buffer &readBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_SpiReadWrite(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_SpiReadWrite(),static_cast<AssertArg>(portNum));
    this->m_to_SpiReadWrite[portNum].invoke(
        writeBuffer, readBuffer
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool LinuxSpiDriverTesterBase ::
    isConnected_to_SpiReadWrite(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_SpiReadWrite(), static_cast<AssertArg>(portNum));
    return this->m_to_SpiReadWrite[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputTlmPort *LinuxSpiDriverTesterBase ::
    get_from_Tlm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Tlm(),static_cast<AssertArg>(portNum));
    return &this->m_from_Tlm[portNum];
  }

  Fw::InputLogPort *LinuxSpiDriverTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *LinuxSpiDriverTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  Fw::InputTimePort *LinuxSpiDriverTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    from_Tlm_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    LinuxSpiDriverTesterBase* _testerBase =
      static_cast<LinuxSpiDriverTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void LinuxSpiDriverTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    LinuxSpiDriverTesterBase* _testerBase =
      static_cast<LinuxSpiDriverTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void LinuxSpiDriverTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    LinuxSpiDriverTesterBase* _testerBase =
      static_cast<LinuxSpiDriverTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void LinuxSpiDriverTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    LinuxSpiDriverTesterBase* _testerBase =
      static_cast<LinuxSpiDriverTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    clearHistory()
  {
    this->clearTlm();
    this->textLogHistory->clear();
    this->clearEvents();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
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

      case LinuxSpiDriverComponentBase::CHANNELID_SPI_BYTES:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing SPI_Bytes: %d\n", _status);
          return;
        }
        this->tlmInput_SPI_Bytes(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void LinuxSpiDriverTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_SPI_Bytes->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: SPI_Bytes
  // ---------------------------------------------------------------------- 

  void LinuxSpiDriverTesterBase ::
    tlmInput_SPI_Bytes(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_SPI_Bytes e = { timeTag, val };
    this->tlmHistory_SPI_Bytes->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
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

      case LinuxSpiDriverComponentBase::EVENTID_SPI_OPENERROR: 
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
        I32 device;
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
        _status = args.deserialize(device);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 select;
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
        _status = args.deserialize(select);
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

        this->logIn_WARNING_HI_SPI_OpenError(device, select, error);

        break;

      }

      case LinuxSpiDriverComponentBase::EVENTID_SPI_CONFIGERROR: 
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
        I32 device;
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
        _status = args.deserialize(device);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 select;
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
        _status = args.deserialize(select);
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

        this->logIn_WARNING_HI_SPI_ConfigError(device, select, error);

        break;

      }

      case LinuxSpiDriverComponentBase::EVENTID_SPI_WRITEERROR: 
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
        I32 device;
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
        _status = args.deserialize(device);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 select;
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
        _status = args.deserialize(select);
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

        this->logIn_WARNING_HI_SPI_WriteError(device, select, error);

        break;

      }

      case LinuxSpiDriverComponentBase::EVENTID_SPI_PORTOPENED: 
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
        I32 device;
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
        _status = args.deserialize(device);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        I32 select;
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
        _status = args.deserialize(select);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_SPI_PortOpened(device, select);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void LinuxSpiDriverTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_SPI_OpenError->clear();
    this->eventHistory_SPI_ConfigError->clear();
    this->eventHistory_SPI_WriteError->clear();
    this->eventHistory_SPI_PortOpened->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
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

  void LinuxSpiDriverTesterBase ::
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

  void LinuxSpiDriverTesterBase ::
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
  // Event: SPI_OpenError 
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    logIn_WARNING_HI_SPI_OpenError(
        I32 device,
        I32 select,
        I32 error
    )
  {
    EventEntry_SPI_OpenError e = {
      device, select, error
    };
    eventHistory_SPI_OpenError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: SPI_ConfigError 
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    logIn_WARNING_HI_SPI_ConfigError(
        I32 device,
        I32 select,
        I32 error
    )
  {
    EventEntry_SPI_ConfigError e = {
      device, select, error
    };
    eventHistory_SPI_ConfigError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: SPI_WriteError 
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    logIn_WARNING_HI_SPI_WriteError(
        I32 device,
        I32 select,
        I32 error
    )
  {
    EventEntry_SPI_WriteError e = {
      device, select, error
    };
    eventHistory_SPI_WriteError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: SPI_PortOpened 
  // ----------------------------------------------------------------------

  void LinuxSpiDriverTesterBase ::
    logIn_ACTIVITY_HI_SPI_PortOpened(
        I32 device,
        I32 select
    )
  {
    EventEntry_SPI_PortOpened e = {
      device, select
    };
    eventHistory_SPI_PortOpened->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Drv
