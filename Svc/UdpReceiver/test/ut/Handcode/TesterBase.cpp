// ======================================================================
// \title  UdpReceiver/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for UdpReceiver component test harness base class
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

  UdpReceiverTesterBase ::
    UdpReceiverTesterBase(
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
    this->tlmHistory_UR_PacketsReceived = 
      new History<TlmEntry_UR_PacketsReceived>(maxHistorySize);
    this->tlmHistory_UR_BytesReceived = 
      new History<TlmEntry_UR_BytesReceived>(maxHistorySize);
    this->tlmHistory_UR_PacketsDropped = 
      new History<TlmEntry_UR_PacketsDropped>(maxHistorySize);
    this->tlmHistory_UR_DecodeErrors = 
      new History<TlmEntry_UR_DecodeErrors>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_UR_PortOpened =
      new History<EventEntry_UR_PortOpened>(maxHistorySize);
    this->eventHistory_UR_SocketError =
      new History<EventEntry_UR_SocketError>(maxHistorySize);
    this->eventHistory_UR_BindError =
      new History<EventEntry_UR_BindError>(maxHistorySize);
    this->eventHistory_UR_RecvError =
      new History<EventEntry_UR_RecvError>(maxHistorySize);
    this->eventHistory_UR_DecodeError =
      new History<EventEntry_UR_DecodeError>(maxHistorySize);
    this->eventHistory_UR_DroppedPacket =
      new History<EventEntry_UR_DroppedPacket>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  UdpReceiverTesterBase ::
    ~UdpReceiverTesterBase(void) 
  {
    // Destroy telemetry histories
    delete this->tlmHistory_UR_PacketsReceived;
    delete this->tlmHistory_UR_BytesReceived;
    delete this->tlmHistory_UR_PacketsDropped;
    delete this->tlmHistory_UR_DecodeErrors;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_UR_PortOpened;
    delete this->eventHistory_UR_SocketError;
    delete this->eventHistory_UR_BindError;
    delete this->eventHistory_UR_RecvError;
    delete this->eventHistory_UR_DecodeError;
    delete this->eventHistory_UR_DroppedPacket;
  }

  void UdpReceiverTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port PortsOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_PortsOut();
        ++_port
    ) {

      this->m_from_PortsOut[_port].init();
      this->m_from_PortsOut[_port].addCallComp(
          this,
          from_PortsOut_static
      );
      this->m_from_PortsOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_PortsOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_PortsOut[_port].setObjName(_portName);
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

    // Initialize output port Sched

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_Sched();
        ++_port
    ) {
      this->m_to_Sched[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_Sched[%d]",
          this->m_objName,
          _port
      );
      this->m_to_Sched[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE UdpReceiverTesterBase ::
    getNum_to_Sched(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_Sched);
  }

  NATIVE_INT_TYPE UdpReceiverTesterBase ::
    getNum_from_PortsOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_PortsOut);
  }

  NATIVE_INT_TYPE UdpReceiverTesterBase ::
    getNum_from_Tlm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Tlm);
  }

  NATIVE_INT_TYPE UdpReceiverTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE UdpReceiverTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE UdpReceiverTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    connect_to_Sched(
        const NATIVE_INT_TYPE portNum,
        Svc::InputSchedPort *const Sched
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_Sched(),static_cast<AssertArg>(portNum));
    this->m_to_Sched[portNum].addCallPort(Sched);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    invoke_to_Sched(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    FW_ASSERT(portNum < this->getNum_to_Sched(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_Sched(),static_cast<AssertArg>(portNum));
    this->m_to_Sched[portNum].invoke(
        context
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool UdpReceiverTesterBase ::
    isConnected_to_Sched(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_Sched(), static_cast<AssertArg>(portNum));
    return this->m_to_Sched[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputSerializePort *UdpReceiverTesterBase ::
    get_from_PortsOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_PortsOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_PortsOut[portNum];
  }

  Fw::InputTlmPort *UdpReceiverTesterBase ::
    get_from_Tlm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Tlm(),static_cast<AssertArg>(portNum));
    return &this->m_from_Tlm[portNum];
  }

  Fw::InputTimePort *UdpReceiverTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputLogPort *UdpReceiverTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *UdpReceiverTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    from_PortsOut_static(
      Fw::PassiveComponentBase *const callComp, //!< The component instance
      const NATIVE_INT_TYPE portNum, //!< The port number
      Fw::SerializeBufferBase& Buffer //!< serialized data buffer
    )
  {
    FW_ASSERT(callComp);
    UdpReceiverTesterBase* _testerBase = 
      static_cast<UdpReceiverTesterBase*>(callComp);

    _testerBase->from_PortsOut_handlerBase(
        portNum,
        Buffer
    );
  }  

  void UdpReceiverTesterBase ::
    from_PortsOut_handlerBase(
        NATIVE_INT_TYPE portNum, /*!< The port number*/
        Fw::SerializeBufferBase &Buffer /*!< The serialization buffer*/
    )
  {
    FW_ASSERT(portNum < this->getNum_from_PortsOut(),static_cast<AssertArg>(portNum));
    this->from_PortsOut_handler(
        portNum,
        Buffer
    );
  } 
   
  void UdpReceiverTesterBase ::
    from_Tlm_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    UdpReceiverTesterBase* _testerBase =
      static_cast<UdpReceiverTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void UdpReceiverTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    UdpReceiverTesterBase* _testerBase =
      static_cast<UdpReceiverTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void UdpReceiverTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    UdpReceiverTesterBase* _testerBase =
      static_cast<UdpReceiverTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void UdpReceiverTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    UdpReceiverTesterBase* _testerBase =
      static_cast<UdpReceiverTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    clearHistory()
  {
    this->clearTlm();
    this->textLogHistory->clear();
    this->clearEvents();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
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

      case UdpReceiverComponentBase::CHANNELID_UR_PACKETSRECEIVED:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing UR_PacketsReceived: %d\n", _status);
          return;
        }
        this->tlmInput_UR_PacketsReceived(timeTag, arg);
        break;
      }

      case UdpReceiverComponentBase::CHANNELID_UR_BYTESRECEIVED:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing UR_BytesReceived: %d\n", _status);
          return;
        }
        this->tlmInput_UR_BytesReceived(timeTag, arg);
        break;
      }

      case UdpReceiverComponentBase::CHANNELID_UR_PACKETSDROPPED:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing UR_PacketsDropped: %d\n", _status);
          return;
        }
        this->tlmInput_UR_PacketsDropped(timeTag, arg);
        break;
      }

      case UdpReceiverComponentBase::CHANNELID_UR_DECODEERRORS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing UR_DecodeErrors: %d\n", _status);
          return;
        }
        this->tlmInput_UR_DecodeErrors(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void UdpReceiverTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_UR_PacketsReceived->clear();
    this->tlmHistory_UR_BytesReceived->clear();
    this->tlmHistory_UR_PacketsDropped->clear();
    this->tlmHistory_UR_DecodeErrors->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: UR_PacketsReceived
  // ---------------------------------------------------------------------- 

  void UdpReceiverTesterBase ::
    tlmInput_UR_PacketsReceived(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_UR_PacketsReceived e = { timeTag, val };
    this->tlmHistory_UR_PacketsReceived->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: UR_BytesReceived
  // ---------------------------------------------------------------------- 

  void UdpReceiverTesterBase ::
    tlmInput_UR_BytesReceived(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_UR_BytesReceived e = { timeTag, val };
    this->tlmHistory_UR_BytesReceived->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: UR_PacketsDropped
  // ---------------------------------------------------------------------- 

  void UdpReceiverTesterBase ::
    tlmInput_UR_PacketsDropped(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_UR_PacketsDropped e = { timeTag, val };
    this->tlmHistory_UR_PacketsDropped->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: UR_DecodeErrors
  // ---------------------------------------------------------------------- 

  void UdpReceiverTesterBase ::
    tlmInput_UR_DecodeErrors(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_UR_DecodeErrors e = { timeTag, val };
    this->tlmHistory_UR_DecodeErrors->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
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

      case UdpReceiverComponentBase::EVENTID_UR_PORTOPENED: 
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
        U32 port;
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
        _status = args.deserialize(port);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_UR_PortOpened(port);

        break;

      }

      case UdpReceiverComponentBase::EVENTID_UR_SOCKETERROR: 
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
        Fw::LogStringArg error;
        _status = args.deserialize(error);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_UR_SocketError(error);

        break;

      }

      case UdpReceiverComponentBase::EVENTID_UR_BINDERROR: 
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
        Fw::LogStringArg error;
        _status = args.deserialize(error);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_UR_BindError(error);

        break;

      }

      case UdpReceiverComponentBase::EVENTID_UR_RECVERROR: 
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
        Fw::LogStringArg error;
        _status = args.deserialize(error);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_UR_RecvError(error);

        break;

      }

      case UdpReceiverComponentBase::EVENTID_UR_DECODEERROR: 
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
        UdpReceiverComponentBase::DecodeStage stage = static_cast<UdpReceiverComponentBase::DecodeStage>(stageInt);
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

        this->logIn_WARNING_HI_UR_DecodeError(stage, error);

        break;

      }

      case UdpReceiverComponentBase::EVENTID_UR_DROPPEDPACKET: 
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
        U32 diff;
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
        _status = args.deserialize(diff);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_UR_DroppedPacket(diff);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void UdpReceiverTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_UR_PortOpened->clear();
    this->eventHistory_UR_SocketError->clear();
    this->eventHistory_UR_BindError->clear();
    this->eventHistory_UR_RecvError->clear();
    this->eventHistory_UR_DecodeError->clear();
    this->eventHistory_UR_DroppedPacket->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
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

  void UdpReceiverTesterBase ::
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

  void UdpReceiverTesterBase ::
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
  // Event: UR_PortOpened 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    logIn_ACTIVITY_HI_UR_PortOpened(
        U32 port
    )
  {
    EventEntry_UR_PortOpened e = {
      port
    };
    eventHistory_UR_PortOpened->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: UR_SocketError 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    logIn_WARNING_HI_UR_SocketError(
        Fw::LogStringArg& error
    )
  {
    EventEntry_UR_SocketError e = {
      error
    };
    eventHistory_UR_SocketError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: UR_BindError 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    logIn_WARNING_HI_UR_BindError(
        Fw::LogStringArg& error
    )
  {
    EventEntry_UR_BindError e = {
      error
    };
    eventHistory_UR_BindError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: UR_RecvError 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    logIn_WARNING_HI_UR_RecvError(
        Fw::LogStringArg& error
    )
  {
    EventEntry_UR_RecvError e = {
      error
    };
    eventHistory_UR_RecvError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: UR_DecodeError 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    logIn_WARNING_HI_UR_DecodeError(
        UdpReceiverComponentBase::DecodeStage stage,
        I32 error
    )
  {
    EventEntry_UR_DecodeError e = {
      stage, error
    };
    eventHistory_UR_DecodeError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: UR_DroppedPacket 
  // ----------------------------------------------------------------------

  void UdpReceiverTesterBase ::
    logIn_WARNING_HI_UR_DroppedPacket(
        U32 diff
    )
  {
    EventEntry_UR_DroppedPacket e = {
      diff
    };
    eventHistory_UR_DroppedPacket->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Svc
