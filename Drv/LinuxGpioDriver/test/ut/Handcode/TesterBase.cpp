// ======================================================================
// \title  LinuxGpioDriver/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for LinuxGpioDriver component test harness base class
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

  LinuxGpioDriverTesterBase ::
    LinuxGpioDriverTesterBase(
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
    this->eventHistory_GP_OpenError =
      new History<EventEntry_GP_OpenError>(maxHistorySize);
    this->eventHistory_GP_ConfigError =
      new History<EventEntry_GP_ConfigError>(maxHistorySize);
    this->eventHistory_GP_WriteError =
      new History<EventEntry_GP_WriteError>(maxHistorySize);
    this->eventHistory_GP_ReadError =
      new History<EventEntry_GP_ReadError>(maxHistorySize);
    this->eventHistory_GP_PortOpened =
      new History<EventEntry_GP_PortOpened>(maxHistorySize);
    this->eventHistory_GP_IntStartError =
      new History<EventEntry_GP_IntStartError>(maxHistorySize);
    this->eventHistory_GP_IntWaitError =
      new History<EventEntry_GP_IntWaitError>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_intOut =
      new History<FromPortEntry_intOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  LinuxGpioDriverTesterBase ::
    ~LinuxGpioDriverTesterBase(void) 
  {
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_GP_OpenError;
    delete this->eventHistory_GP_ConfigError;
    delete this->eventHistory_GP_WriteError;
    delete this->eventHistory_GP_ReadError;
    delete this->eventHistory_GP_PortOpened;
    delete this->eventHistory_GP_IntStartError;
    delete this->eventHistory_GP_IntWaitError;
  }

  void LinuxGpioDriverTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

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

    // Attach input port intOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_intOut();
        ++_port
    ) {

      this->m_from_intOut[_port].init();
      this->m_from_intOut[_port].addCallComp(
          this,
          from_intOut_static
      );
      this->m_from_intOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_intOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_intOut[_port].setObjName(_portName);
#endif

    }

    // Initialize output port gpioWrite

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_gpioWrite();
        ++_port
    ) {
      this->m_to_gpioWrite[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_gpioWrite[%d]",
          this->m_objName,
          _port
      );
      this->m_to_gpioWrite[_port].setObjName(_portName);
#endif

    }

    // Initialize output port gpioRead

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_gpioRead();
        ++_port
    ) {
      this->m_to_gpioRead[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_gpioRead[%d]",
          this->m_objName,
          _port
      );
      this->m_to_gpioRead[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE LinuxGpioDriverTesterBase ::
    getNum_to_gpioWrite(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_gpioWrite);
  }

  NATIVE_INT_TYPE LinuxGpioDriverTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE LinuxGpioDriverTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  NATIVE_INT_TYPE LinuxGpioDriverTesterBase ::
    getNum_to_gpioRead(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_gpioRead);
  }

  NATIVE_INT_TYPE LinuxGpioDriverTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE LinuxGpioDriverTesterBase ::
    getNum_from_intOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_intOut);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    connect_to_gpioWrite(
        const NATIVE_INT_TYPE portNum,
        Drv::InputGpioWritePort *const gpioWrite
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_gpioWrite(),static_cast<AssertArg>(portNum));
    this->m_to_gpioWrite[portNum].addCallPort(gpioWrite);
  }

  void LinuxGpioDriverTesterBase ::
    connect_to_gpioRead(
        const NATIVE_INT_TYPE portNum,
        Drv::InputGpioReadPort *const gpioRead
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_gpioRead(),static_cast<AssertArg>(portNum));
    this->m_to_gpioRead[portNum].addCallPort(gpioRead);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    invoke_to_gpioWrite(
        const NATIVE_INT_TYPE portNum,
        bool state
    )
  {
    FW_ASSERT(portNum < this->getNum_to_gpioWrite(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_gpioWrite(),static_cast<AssertArg>(portNum));
    this->m_to_gpioWrite[portNum].invoke(
        state
    );
  }

  void LinuxGpioDriverTesterBase ::
    invoke_to_gpioRead(
        const NATIVE_INT_TYPE portNum,
        bool &state
    )
  {
    FW_ASSERT(portNum < this->getNum_to_gpioRead(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_gpioRead(),static_cast<AssertArg>(portNum));
    this->m_to_gpioRead[portNum].invoke(
        state
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool LinuxGpioDriverTesterBase ::
    isConnected_to_gpioWrite(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_gpioWrite(), static_cast<AssertArg>(portNum));
    return this->m_to_gpioWrite[portNum].isConnected();
  }

  bool LinuxGpioDriverTesterBase ::
    isConnected_to_gpioRead(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_gpioRead(), static_cast<AssertArg>(portNum));
    return this->m_to_gpioRead[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputLogPort *LinuxGpioDriverTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *LinuxGpioDriverTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  Fw::InputTimePort *LinuxGpioDriverTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Svc::InputCyclePort *LinuxGpioDriverTesterBase ::
    get_from_intOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_intOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_intOut[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    from_intOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    FW_ASSERT(callComp);
    LinuxGpioDriverTesterBase* _testerBase = 
      static_cast<LinuxGpioDriverTesterBase*>(callComp);
    _testerBase->from_intOut_handlerBase(
        portNum,
        cycleStart
    );
  }

  void LinuxGpioDriverTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    LinuxGpioDriverTesterBase* _testerBase =
      static_cast<LinuxGpioDriverTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void LinuxGpioDriverTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    LinuxGpioDriverTesterBase* _testerBase =
      static_cast<LinuxGpioDriverTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void LinuxGpioDriverTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    LinuxGpioDriverTesterBase* _testerBase =
      static_cast<LinuxGpioDriverTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_intOut->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: intOut
  // ---------------------------------------------------------------------- 

  void LinuxGpioDriverTesterBase ::
    pushFromPortEntry_intOut(
        Svc::TimerVal &cycleStart
    )
  {
    FromPortEntry_intOut _e = {
      cycleStart
    };
    this->fromPortHistory_intOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    from_intOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    FW_ASSERT(portNum < this->getNum_from_intOut(),static_cast<AssertArg>(portNum));
    this->from_intOut_handler(
        portNum,
        cycleStart
    );
  }

  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    clearHistory()
  {
    this->textLogHistory->clear();
    this->clearEvents();
    this->clearFromPortHistory();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
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

      case LinuxGpioDriverComponentBase::EVENTID_GP_OPENERROR: 
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
        I32 gpio;
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
        _status = args.deserialize(gpio);
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

        this->logIn_WARNING_HI_GP_OpenError(gpio, error);

        break;

      }

      case LinuxGpioDriverComponentBase::EVENTID_GP_CONFIGERROR: 
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
        I32 gpio;
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
        _status = args.deserialize(gpio);
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

        this->logIn_WARNING_HI_GP_ConfigError(gpio, error);

        break;

      }

      case LinuxGpioDriverComponentBase::EVENTID_GP_WRITEERROR: 
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
        I32 gpio;
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
        _status = args.deserialize(gpio);
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

        this->logIn_WARNING_HI_GP_WriteError(gpio, error);

        break;

      }

      case LinuxGpioDriverComponentBase::EVENTID_GP_READERROR: 
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
        I32 gpio;
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
        _status = args.deserialize(gpio);
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

        this->logIn_WARNING_HI_GP_ReadError(gpio, error);

        break;

      }

      case LinuxGpioDriverComponentBase::EVENTID_GP_PORTOPENED: 
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
        I32 gpio;
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
        _status = args.deserialize(gpio);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_GP_PortOpened(gpio);

        break;

      }

      case LinuxGpioDriverComponentBase::EVENTID_GP_INTSTARTERROR: 
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
        I32 gpio;
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
        _status = args.deserialize(gpio);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_GP_IntStartError(gpio);

        break;

      }

      case LinuxGpioDriverComponentBase::EVENTID_GP_INTWAITERROR: 
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
        I32 gpio;
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
        _status = args.deserialize(gpio);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_WARNING_HI_GP_IntWaitError(gpio);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void LinuxGpioDriverTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_GP_OpenError->clear();
    this->eventHistory_GP_ConfigError->clear();
    this->eventHistory_GP_WriteError->clear();
    this->eventHistory_GP_ReadError->clear();
    this->eventHistory_GP_PortOpened->clear();
    this->eventHistory_GP_IntStartError->clear();
    this->eventHistory_GP_IntWaitError->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
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

  void LinuxGpioDriverTesterBase ::
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

  void LinuxGpioDriverTesterBase ::
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
  // Event: GP_OpenError 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    logIn_WARNING_HI_GP_OpenError(
        I32 gpio,
        I32 error
    )
  {
    EventEntry_GP_OpenError e = {
      gpio, error
    };
    eventHistory_GP_OpenError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: GP_ConfigError 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    logIn_WARNING_HI_GP_ConfigError(
        I32 gpio,
        I32 error
    )
  {
    EventEntry_GP_ConfigError e = {
      gpio, error
    };
    eventHistory_GP_ConfigError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: GP_WriteError 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    logIn_WARNING_HI_GP_WriteError(
        I32 gpio,
        I32 error
    )
  {
    EventEntry_GP_WriteError e = {
      gpio, error
    };
    eventHistory_GP_WriteError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: GP_ReadError 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    logIn_WARNING_HI_GP_ReadError(
        I32 gpio,
        I32 error
    )
  {
    EventEntry_GP_ReadError e = {
      gpio, error
    };
    eventHistory_GP_ReadError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: GP_PortOpened 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    logIn_ACTIVITY_HI_GP_PortOpened(
        I32 gpio
    )
  {
    EventEntry_GP_PortOpened e = {
      gpio
    };
    eventHistory_GP_PortOpened->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: GP_IntStartError 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    logIn_WARNING_HI_GP_IntStartError(
        I32 gpio
    )
  {
    EventEntry_GP_IntStartError e = {
      gpio
    };
    eventHistory_GP_IntStartError->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: GP_IntWaitError 
  // ----------------------------------------------------------------------

  void LinuxGpioDriverTesterBase ::
    logIn_WARNING_HI_GP_IntWaitError(
        I32 gpio
    )
  {
    EventEntry_GP_IntWaitError e = {
      gpio
    };
    eventHistory_GP_IntWaitError->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Drv
