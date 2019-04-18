// ======================================================================
// \title  LinuxSerialDriver/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for LinuxSerialDriver component test harness base class
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

  LinuxSerialDriverTesterBase ::
    LinuxSerialDriverTesterBase(
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
    this->tlmHistory_DR_BytesSent = 
      new History<TlmEntry_DR_BytesSent>(maxHistorySize);
    this->tlmHistory_DR_BytesRecv = 
      new History<TlmEntry_DR_BytesRecv>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_DR_OpenError =
      new History<EventEntry_DR_OpenError>(maxHistorySize);
    this->eventHistory_DR_ConfigError =
      new History<EventEntry_DR_ConfigError>(maxHistorySize);
    this->eventHistory_DR_WriteError =
      new History<EventEntry_DR_WriteError>(maxHistorySize);
    this->eventHistory_DR_ReadError =
      new History<EventEntry_DR_ReadError>(maxHistorySize);
    this->eventHistory_DR_PortOpened =
      new History<EventEntry_DR_PortOpened>(maxHistorySize);
    this->eventHistory_DR_NoBuffers =
      new History<EventEntry_DR_NoBuffers>(maxHistorySize);
    this->eventHistory_DR_BufferTooSmall =
      new History<EventEntry_DR_BufferTooSmall>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_serialRecv =
      new History<FromPortEntry_serialRecv>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  LinuxSerialDriverTesterBase ::
    ~LinuxSerialDriverTesterBase(void) 
  {
    // Destroy telemetry histories
    delete this->tlmHistory_DR_BytesSent;
    delete this->tlmHistory_DR_BytesRecv;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_DR_OpenError;
    delete this->eventHistory_DR_ConfigError;
    delete this->eventHistory_DR_WriteError;
    delete this->eventHistory_DR_ReadError;
    delete this->eventHistory_DR_PortOpened;
    delete this->eventHistory_DR_NoBuffers;
    delete this->eventHistory_DR_BufferTooSmall;
  }

  void LinuxSerialDriverTesterBase ::
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

    // Attach input port serialRecv

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_serialRecv();
        ++_port
    ) {

      this->m_from_serialRecv[_port].init();
      this->m_from_serialRecv[_port].addCallComp(
          this,
          from_serialRecv_static
      );
      this->m_from_serialRecv[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_serialRecv[%d]",
          this->m_objName,
          _port
      );
      this->m_from_serialRecv[_port].setObjName(_portName);
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

    // Initialize output port readBufferSend

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_readBufferSend();
        ++_port
    ) {
      this->m_to_readBufferSend[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_readBufferSend[%d]",
          this->m_objName,
          _port
      );
      this->m_to_readBufferSend[_port].setObjName(_portName);
#endif

    }

    // Initialize output port serialSend

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_serialSend();
        ++_port
    ) {
      this->m_to_serialSend[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_serialSend[%d]",
          this->m_objName,
          _port
      );
      this->m_to_serialSend[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE LinuxSerialDriverTesterBase ::
    getNum_from_Tlm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Tlm);
  }

  NATIVE_INT_TYPE LinuxSerialDriverTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

  NATIVE_INT_TYPE LinuxSerialDriverTesterBase ::
    getNum_to_readBufferSend(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_readBufferSend);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE LinuxSerialDriverTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  NATIVE_INT_TYPE LinuxSerialDriverTesterBase ::
    getNum_from_serialRecv(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_serialRecv);
  }

  NATIVE_INT_TYPE LinuxSerialDriverTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE LinuxSerialDriverTesterBase ::
    getNum_to_serialSend(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_serialSend);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    connect_to_readBufferSend(
        const NATIVE_INT_TYPE portNum,
        Fw::InputBufferSendPort *const readBufferSend
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_readBufferSend(),static_cast<AssertArg>(portNum));
    this->m_to_readBufferSend[portNum].addCallPort(readBufferSend);
  }

  void LinuxSerialDriverTesterBase ::
    connect_to_serialSend(
        const NATIVE_INT_TYPE portNum,
        Drv::InputSerialWritePort *const serialSend
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_serialSend(),static_cast<AssertArg>(portNum));
    this->m_to_serialSend[portNum].addCallPort(serialSend);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    invoke_to_readBufferSend(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_readBufferSend(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_readBufferSend(),static_cast<AssertArg>(portNum));
    this->m_to_readBufferSend[portNum].invoke(
        fwBuffer
    );
  }

  void LinuxSerialDriverTesterBase ::
    invoke_to_serialSend(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_serialSend(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_serialSend(),static_cast<AssertArg>(portNum));
    this->m_to_serialSend[portNum].invoke(
        serBuffer
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool LinuxSerialDriverTesterBase ::
    isConnected_to_readBufferSend(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_readBufferSend(), static_cast<AssertArg>(portNum));
    return this->m_to_readBufferSend[portNum].isConnected();
  }

  bool LinuxSerialDriverTesterBase ::
    isConnected_to_serialSend(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_serialSend(), static_cast<AssertArg>(portNum));
    return this->m_to_serialSend[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputTlmPort *LinuxSerialDriverTesterBase ::
    get_from_Tlm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Tlm(),static_cast<AssertArg>(portNum));
    return &this->m_from_Tlm[portNum];
  }

  Fw::InputLogPort *LinuxSerialDriverTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *LinuxSerialDriverTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  Drv::InputSerialReadPort *LinuxSerialDriverTesterBase ::
    get_from_serialRecv(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_serialRecv(),static_cast<AssertArg>(portNum));
    return &this->m_from_serialRecv[portNum];
  }

  Fw::InputTimePort *LinuxSerialDriverTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    from_serialRecv_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer,
        SerialReadStatus &status
    )
  {
    FW_ASSERT(callComp);
    LinuxSerialDriverTesterBase* _testerBase = 
      static_cast<LinuxSerialDriverTesterBase*>(callComp);
    _testerBase->from_serialRecv_handlerBase(
        portNum,
        serBuffer, status
    );
  }

  void LinuxSerialDriverTesterBase ::
    from_Tlm_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    LinuxSerialDriverTesterBase* _testerBase =
      static_cast<LinuxSerialDriverTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void LinuxSerialDriverTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    LinuxSerialDriverTesterBase* _testerBase =
      static_cast<LinuxSerialDriverTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void LinuxSerialDriverTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    LinuxSerialDriverTesterBase* _testerBase =
      static_cast<LinuxSerialDriverTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void LinuxSerialDriverTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    LinuxSerialDriverTesterBase* _testerBase =
      static_cast<LinuxSerialDriverTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_serialRecv->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: serialRecv
  // ---------------------------------------------------------------------- 

  void LinuxSerialDriverTesterBase ::
    pushFromPortEntry_serialRecv(
        Fw::Buffer &serBuffer,
        SerialReadStatus &status
    )
  {
    FromPortEntry_serialRecv _e = {
      serBuffer, status
    };
    this->fromPortHistory_serialRecv->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    from_serialRecv_handlerBase(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &serBuffer,
        SerialReadStatus &status
    )
  {
    FW_ASSERT(portNum < this->getNum_from_serialRecv(),static_cast<AssertArg>(portNum));
    this->from_serialRecv_handler(
        portNum,
        serBuffer, status
    );
  }

  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
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

  void LinuxSerialDriverTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
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

      case LinuxSerialDriverComponentBase::CHANNELID_DR_BYTESSENT:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing DR_BytesSent: %d\n", _status);
          return;
        }
        this->tlmInput_DR_BytesSent(timeTag, arg);
        break;
      }

      case LinuxSerialDriverComponentBase::CHANNELID_DR_BYTESRECV:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing DR_BytesRecv: %d\n", _status);
          return;
        }
        this->tlmInput_DR_BytesRecv(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void LinuxSerialDriverTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_DR_BytesSent->clear();
    this->tlmHistory_DR_BytesRecv->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: DR_BytesSent
  // ---------------------------------------------------------------------- 

  void LinuxSerialDriverTesterBase ::
    tlmInput_DR_BytesSent(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_DR_BytesSent e = { timeTag, val };
    this->tlmHistory_DR_BytesSent->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: DR_BytesRecv
  // ---------------------------------------------------------------------- 

  void LinuxSerialDriverTesterBase ::
    tlmInput_DR_BytesRecv(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_DR_BytesRecv e = { timeTag, val };
    this->tlmHistory_DR_BytesRecv->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
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

      case LinuxSerialDriverComponentBase::EVENTID_DR_OPENERROR: 
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

        this->logIn_WARNING_HI_DR_OpenError(device, error);

        break;

      }

      case LinuxSerialDriverComponentBase::EVENTID_DR_CONFIGERROR: 
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

        this->logIn_WARNING_HI_DR_ConfigError(device, error);

        break;

      }

      case LinuxSerialDriverComponentBase::EVENTID_DR_WRITEERROR: 
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

        this->logIn_WARNING_HI_DR_WriteError(device, error);

        break;

      }

      case LinuxSerialDriverComponentBase::EVENTID_DR_READERROR: 
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

        this->logIn_WARNING_HI_DR_ReadError(device, error);

        break;

      }

      case LinuxSerialDriverComponentBase::EVENTID_DR_PORTOPENED: 
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

        this->logIn_ACTIVITY_HI_DR_PortOpened(device);

        break;

      }

      case LinuxSerialDriverComponentBase::EVENTID_DR_NOBUFFERS: 
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

        this->logIn_WARNING_HI_DR_NoBuffers(device);

        break;

      }

      case LinuxSerialDriverComponentBase::EVENTID_DR_BUFFERTOOSMALL: 
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

        U32 needed;
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
        _status = args.deserialize(needed);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_DR_BufferTooSmall(device, size, needed);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void LinuxSerialDriverTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_DR_OpenError->clear();
    this->eventHistory_DR_ConfigError->clear();
    this->eventHistory_DR_WriteError->clear();
    this->eventHistory_DR_ReadError->clear();
    this->eventHistory_DR_PortOpened->clear();
    this->eventHistory_DR_NoBuffers->clear();
    this->eventHistory_DR_BufferTooSmall->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
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

  void LinuxSerialDriverTesterBase ::
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

  void LinuxSerialDriverTesterBase ::
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
  // Event: DR_OpenError 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    logIn_WARNING_HI_DR_OpenError(
        I32 device,
        I32 error
    )
  {
    EventEntry_DR_OpenError e = {
      device, error
    };
    eventHistory_DR_OpenError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: DR_ConfigError 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    logIn_WARNING_HI_DR_ConfigError(
        I32 device,
        I32 error
    )
  {
    EventEntry_DR_ConfigError e = {
      device, error
    };
    eventHistory_DR_ConfigError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: DR_WriteError 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    logIn_WARNING_HI_DR_WriteError(
        I32 device,
        I32 error
    )
  {
    EventEntry_DR_WriteError e = {
      device, error
    };
    eventHistory_DR_WriteError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: DR_ReadError 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    logIn_WARNING_HI_DR_ReadError(
        I32 device,
        I32 error
    )
  {
    EventEntry_DR_ReadError e = {
      device, error
    };
    eventHistory_DR_ReadError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: DR_PortOpened 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    logIn_ACTIVITY_HI_DR_PortOpened(
        I32 device
    )
  {
    EventEntry_DR_PortOpened e = {
      device
    };
    eventHistory_DR_PortOpened->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: DR_NoBuffers 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    logIn_WARNING_HI_DR_NoBuffers(
        I32 device
    )
  {
    EventEntry_DR_NoBuffers e = {
      device
    };
    eventHistory_DR_NoBuffers->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: DR_BufferTooSmall 
  // ----------------------------------------------------------------------

  void LinuxSerialDriverTesterBase ::
    logIn_WARNING_HI_DR_BufferTooSmall(
        I32 device,
        U32 size,
        U32 needed
    )
  {
    EventEntry_DR_BufferTooSmall e = {
      device, size, needed
    };
    eventHistory_DR_BufferTooSmall->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Drv
