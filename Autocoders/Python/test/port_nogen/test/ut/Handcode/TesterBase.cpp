// ======================================================================
// \title  Example/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for Example component test harness base class
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

namespace ExampleComponents {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ExampleTesterBase ::
    ExampleTesterBase(
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
    // Initialize telemetry histories
    this->tlmHistory_somechan =
      new History<TlmEntry_somechan>(maxHistorySize);
    this->tlmHistory_anotherchan =
      new History<TlmEntry_anotherchan>(maxHistorySize);
    this->tlmHistory_stringchan =
      new History<TlmEntry_stringchan>(maxHistorySize);
    this->tlmHistory_enumchan =
      new History<TlmEntry_enumchan>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_SomeEvent =
      new History<EventEntry_SomeEvent>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_exampleOutput =
      new History<FromPortEntry_exampleOutput>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  ExampleTesterBase ::
    ~ExampleTesterBase(void)
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy telemetry histories
    delete this->tlmHistory_somechan;
    delete this->tlmHistory_anotherchan;
    delete this->tlmHistory_stringchan;
    delete this->tlmHistory_enumchan;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_SomeEvent;
  }

  void ExampleTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {
    this->m_param_someparam_valid = Fw::PARAM_UNINIT;
    this->m_param_anotherparam_valid = Fw::PARAM_UNINIT;
    this->m_param_stringparam_valid = Fw::PARAM_UNINIT;
    this->m_param_enumparam_valid = Fw::PARAM_UNINIT;

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port exampleOutput

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_exampleOutput();
        ++_port
    ) {

      this->m_from_exampleOutput[_port].init();
      this->m_from_exampleOutput[_port].addCallComp(
          this,
          from_exampleOutput_static
      );
      this->m_from_exampleOutput[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_exampleOutput[%d]",
          this->m_objName,
          _port
      );
      this->m_from_exampleOutput[_port].setObjName(_portName);
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

    // Attach input port ParamGet

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_ParamGet();
        ++_port
    ) {

      this->m_from_ParamGet[_port].init();
      this->m_from_ParamGet[_port].addCallComp(
          this,
          from_ParamGet_static
      );
      this->m_from_ParamGet[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_ParamGet[%d]",
          this->m_objName,
          _port
      );
      this->m_from_ParamGet[_port].setObjName(_portName);
#endif

    }

    // Attach input port ParamSet

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_ParamSet();
        ++_port
    ) {

      this->m_from_ParamSet[_port].init();
      this->m_from_ParamSet[_port].addCallComp(
          this,
          from_ParamSet_static
      );
      this->m_from_ParamSet[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_ParamSet[%d]",
          this->m_objName,
          _port
      );
      this->m_from_ParamSet[_port].setObjName(_portName);
#endif

    }

    // Initialize output port exampleInput

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_exampleInput();
        ++_port
    ) {
      this->m_to_exampleInput[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_exampleInput[%d]",
          this->m_objName,
          _port
      );
      this->m_to_exampleInput[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_to_exampleInput(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_exampleInput);
  }

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_exampleOutput(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_exampleOutput);
  }

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_to_CmdDisp(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CmdDisp);
  }

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_CmdStatus(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdStatus);
  }

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_CmdReg(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdReg);
  }

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_Tlm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Tlm);
  }

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_ParamGet(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_ParamGet);
  }

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_from_ParamSet(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_ParamSet);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    connect_to_exampleInput(
        const NATIVE_INT_TYPE portNum,
        AnotherExample::InputExamplePort *const exampleInput
    )
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    this->m_to_exampleInput[portNum].addCallPort(exampleInput);
  }

  void ExampleTesterBase ::
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

  void ExampleTesterBase ::
    invoke_to_exampleInput(
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        ANameSpace::mytype arg2,
        U8 arg3,
        Example3::ExampleSerializable arg4,
        AnotherExample::SomeEnum arg5
    )
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    this->m_to_exampleInput[portNum].invoke(
        arg1, arg2, arg3, arg4, arg5
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool ExampleTesterBase ::
    isConnected_to_exampleInput(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(), static_cast<AssertArg>(portNum));
    return this->m_to_exampleInput[portNum].isConnected();
  }

  bool ExampleTesterBase ::
    isConnected_to_CmdDisp(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(), static_cast<AssertArg>(portNum));
    return this->m_to_CmdDisp[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  AnotherExample::InputExamplePort *ExampleTesterBase ::
    get_from_exampleOutput(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_exampleOutput(),static_cast<AssertArg>(portNum));
    return &this->m_from_exampleOutput[portNum];
  }

  Fw::InputCmdResponsePort *ExampleTesterBase ::
    get_from_CmdStatus(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdStatus(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdStatus[portNum];
  }

  Fw::InputCmdRegPort *ExampleTesterBase ::
    get_from_CmdReg(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdReg(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdReg[portNum];
  }

  Fw::InputTlmPort *ExampleTesterBase ::
    get_from_Tlm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Tlm(),static_cast<AssertArg>(portNum));
    return &this->m_from_Tlm[portNum];
  }

  Fw::InputLogPort *ExampleTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *ExampleTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  Fw::InputTimePort *ExampleTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputPrmGetPort *ExampleTesterBase ::
    get_from_ParamGet(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_ParamGet(),static_cast<AssertArg>(portNum));
    return &this->m_from_ParamGet[portNum];
  }

  Fw::InputPrmSetPort *ExampleTesterBase ::
    get_from_ParamSet(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_ParamSet(),static_cast<AssertArg>(portNum));
    return &this->m_from_ParamSet[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    from_exampleOutput_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        ANameSpace::mytype arg2,
        U8 arg3,
        Example3::ExampleSerializable arg4,
        AnotherExample::SomeEnum arg5
    )
  {
    FW_ASSERT(callComp);
    ExampleTesterBase* _testerBase =
      static_cast<ExampleTesterBase*>(callComp);
    _testerBase->from_exampleOutput_handlerBase(
        portNum,
        arg1, arg2, arg3, arg4, arg5
    );
  }

  void ExampleTesterBase ::
    from_CmdStatus_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    ExampleTesterBase* _testerBase =
      static_cast<ExampleTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void ExampleTesterBase ::
    from_CmdReg_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void ExampleTesterBase ::
    from_Tlm_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    ExampleTesterBase* _testerBase =
      static_cast<ExampleTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void ExampleTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    ExampleTesterBase* _testerBase =
      static_cast<ExampleTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void ExampleTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    ExampleTesterBase* _testerBase =
      static_cast<ExampleTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  Fw::Time ExampleTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum
    )
  {
    ExampleTesterBase* _testerBase =
      static_cast<ExampleTesterBase*>(component);
    return _testerBase->m_testTime;
  }


  Fw::ParamValid ExampleTesterBase ::
    from_ParamGet_static(
        Fw::PassiveComponentBase* component,
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    ExampleTesterBase* _testerBase =
      static_cast<ExampleTesterBase*>(component);

    Fw::SerializeStatus _status;
    Fw::ParamValid _ret = Fw::PARAM_VALID;
    val.resetSer();

    switch (id) {
      case 100:
      {
        _status = val.serialize(_testerBase->m_param_someparam);
        _ret = _testerBase->m_param_someparam_valid;
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );
      }
        break;
      case 101:
      {
        _status = val.serialize(_testerBase->m_param_anotherparam);
        _ret = _testerBase->m_param_anotherparam_valid;
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );
      }
        break;
      case 102:
      {
        _status = val.serialize(_testerBase->m_param_stringparam);
        _ret = _testerBase->m_param_stringparam_valid;
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );
      }
        break;
      case 103:
      {
        _status = val.serialize(_testerBase->m_param_enumparam);
        _ret = _testerBase->m_param_enumparam_valid;
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );
      }
        break;
      default:
        FW_ASSERT(id);
        break;
    }

    return _ret;
  }

  void ExampleTesterBase ::
    from_ParamSet_static(
        Fw::PassiveComponentBase* component,
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    ExampleTesterBase* _testerBase =
      static_cast<ExampleTesterBase*>(component);

    Fw::SerializeStatus _status;
    val.resetDeser();

    // This is exercised completely in autocode,
    // so just verify that it works. If it doesn't
    // it probably is an autocoder error.

    switch (id) {
      case 100:
      {
        U32 someparamVal;
        _status = val.deserialize(someparamVal);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );
        FW_ASSERT(
            someparamVal ==
            _testerBase->m_param_someparam
        );
        break;
      }

      case 101:
      {
        Example4::Example2 anotherparamVal;
        _status = val.deserialize(anotherparamVal);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );
        FW_ASSERT(
            anotherparamVal ==
            _testerBase->m_param_anotherparam
        );
        break;
      }

      case 102:
      {
        Fw::ParamString stringparamVal;
        _status = val.deserialize(stringparamVal);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status)
        );
        FW_ASSERT(
            stringparamVal ==
            _testerBase->m_param_stringparam
        );
        break;
      }

      case 103:
      {
        FwEnumStoreType enumparamVal;
        _status = val.deserialize(enumparamVal);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );
        FW_ASSERT(
            enumparamVal ==
            static_cast<FwEnumStoreType>(_testerBase->m_param_enumparam)
        );
        break;
      }

      default: {
        FW_ASSERT(id);
        break;
      }

    }
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_exampleOutput->clear();
  }

  // ----------------------------------------------------------------------
  // From port: exampleOutput
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    pushFromPortEntry_exampleOutput(
        I32 arg1,
        ANameSpace::mytype arg2,
        U8 arg3,
        Example3::ExampleSerializable arg4,
        AnotherExample::SomeEnum arg5
    )
  {
    FromPortEntry_exampleOutput _e = {
      arg1, arg2, arg3, arg4, arg5
    };
    this->fromPortHistory_exampleOutput->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    from_exampleOutput_handlerBase(
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        ANameSpace::mytype arg2,
        U8 arg3,
        Example3::ExampleSerializable arg4,
        AnotherExample::SomeEnum arg5
    )
  {
    FW_ASSERT(portNum < this->getNum_from_exampleOutput(),static_cast<AssertArg>(portNum));
    this->from_exampleOutput_handler(
        portNum,
        arg1, arg2, arg3, arg4, arg5
    );
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
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
  // Command: TEST_CMD_1
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    sendCmd_TEST_CMD_1(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        I32 arg1,
        ExampleComponentBase::CmdEnum arg2,
        const Fw::CmdStringArg& arg3
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(arg1);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize((FwEnumStoreType) arg2);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize(arg3);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port

    FwOpcodeType _opcode;
    _opcode = ExampleComponentBase::OPCODE_TEST_CMD_1;

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
  // Command: TEST_CMD_2
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    sendCmd_TEST_CMD_2(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        I32 arg1,
        F32 arg2
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(arg1);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize(arg2);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port

    FwOpcodeType _opcode;
    _opcode = ExampleComponentBase::OPCODE_TEST_CMD_2;

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
  // History
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    clearHistory()
  {
    this->cmdResponseHistory->clear();
    this->clearTlm();
    this->textLogHistory->clear();
    this->clearEvents();
    this->clearFromPortHistory();
  }

  // ----------------------------------------------------------------------
  // Time
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    dispatchTlm(
        const FwChanIdType id,
        const Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {

    val.resetDeser();

    switch (id) {

      case ExampleComponentBase::CHANNELID_SOMECHAN:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing somechan: %d\n", _status);
          return;
        }
        this->tlmInput_somechan(timeTag, arg);
        break;
      }

      case ExampleComponentBase::CHANNELID_ANOTHERCHAN:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing anotherchan: %d\n", _status);
          return;
        }
        this->tlmInput_anotherchan(timeTag, arg);
        break;
      }

      case ExampleComponentBase::CHANNELID_STRINGCHAN:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing stringchan: %d\n", _status);
          return;
        }
        this->tlmInput_stringchan(timeTag, arg);
        break;
      }

      case ExampleComponentBase::CHANNELID_ENUMCHAN:
      {
        FwEnumStoreType enumchanarg;
        const Fw::SerializeStatus _status = val.deserialize(enumchanarg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing enumchan: %d\n", _status);
          return;
        }
        ExampleComponentBase::SomeTlmEnum arg =
          static_cast<ExampleComponentBase::SomeTlmEnum>(enumchanarg);
        this->tlmInput_enumchan(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void ExampleTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_somechan->clear();
    this->tlmHistory_anotherchan->clear();
    this->tlmHistory_stringchan->clear();
    this->tlmHistory_enumchan->clear();
  }

  // ----------------------------------------------------------------------
  // Channel: somechan
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    tlmInput_somechan(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_somechan e = { timeTag, val };
    this->tlmHistory_somechan->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Channel: anotherchan
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    tlmInput_anotherchan(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_anotherchan e = { timeTag, val };
    this->tlmHistory_anotherchan->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Channel: stringchan
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    tlmInput_stringchan(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_stringchan e = { timeTag, val };
    this->tlmHistory_stringchan->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Channel: enumchan
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    tlmInput_enumchan(
        const Fw::Time& timeTag,
        const ExampleComponentBase::SomeTlmEnum& val
    )
  {
    TlmEntry_enumchan e = { timeTag, val };
    this->tlmHistory_enumchan->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    dispatchEvents(
        const FwEventIdType id,
        Fw::Time &timeTag,
        const Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {

    args.resetDeser();

    switch (id) {

      case ExampleComponentBase::EVENTID_SOMEEVENT:
      {

        Fw::SerializeStatus _status;
        I32 arg1;
        _status = args.deserialize(arg1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        F32 arg2;
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

        Fw::LogStringArg arg4;
        _status = args.deserialize(arg4);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        FwEnumStoreType arg5Int;
        _status = args.deserialize(arg5Int);
        ExampleComponentBase::SomeEventEnum arg5 = static_cast<ExampleComponentBase::SomeEventEnum>(arg5Int);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_FATAL_SomeEvent(arg1, arg2, arg3, arg4, arg5);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void ExampleTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_SomeEvent->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
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

  void ExampleTesterBase ::
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

  void ExampleTesterBase ::
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

  void ExampleTesterBase ::
    logIn_FATAL_SomeEvent(
        I32 arg1,
        F32 arg2,
        U32 arg3,
        Fw::LogStringArg& arg4,
        ExampleComponentBase::SomeEventEnum arg5
    )
  {
    EventEntry_SomeEvent e = {
      arg1, arg2, arg3, arg4, arg5
    };
    eventHistory_SomeEvent->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Parameter someparam
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    paramSet_someparam(
        const U32& val,
        Fw::ParamValid valid
    )
  {
    this->m_param_someparam = val;
    this->m_param_someparam_valid = valid;
  }

  void ExampleTesterBase ::
    paramSend_someparam(
        NATIVE_INT_TYPE instance,
        U32 cmdSeq
    )
  {

    // Build command for parameter set

    Fw::CmdArgBuffer args;
    FW_ASSERT(
        args.serialize(
            this->m_param_someparam
        ) == Fw::FW_SERIALIZE_OK
    );
    FwOpcodeType _prmOpcode;
    _prmOpcode =  ExampleComponentBase::OPCODE_SOMEPARAM_SET;
    if (not this->m_to_CmdDisp[0].isConnected()) {
      printf("Test Command Output port not connected!\n");
    }
    else {
      this->m_to_CmdDisp[0].invoke(
          _prmOpcode,
          cmdSeq,
          args
      );
    }

  }

  void ExampleTesterBase ::
    paramSave_someparam(U32 cmdSeq)
  {
    Fw::CmdArgBuffer args;
    FwOpcodeType _prmOpcode;
    _prmOpcode = ExampleComponentBase::OPCODE_SOMEPARAM_SAVE;
    if (not this->m_to_CmdDisp[0].isConnected()) {
      printf("Test Command Output port not connected!\n");
    }
    else {
      this->m_to_CmdDisp[0].invoke(
          _prmOpcode,
          cmdSeq,
          args
      );
    }
  }

  // ----------------------------------------------------------------------
  // Parameter anotherparam
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    paramSet_anotherparam(
        const Example4::Example2& val,
        Fw::ParamValid valid
    )
  {
    this->m_param_anotherparam = val;
    this->m_param_anotherparam_valid = valid;
  }

  void ExampleTesterBase ::
    paramSend_anotherparam(
        NATIVE_INT_TYPE instance,
        U32 cmdSeq
    )
  {

    // Build command for parameter set

    Fw::CmdArgBuffer args;
    FW_ASSERT(
        args.serialize(
            this->m_param_anotherparam
        ) == Fw::FW_SERIALIZE_OK
    );
    FwOpcodeType _prmOpcode;
    _prmOpcode =  ExampleComponentBase::OPCODE_ANOTHERPARAM_SET;
    if (not this->m_to_CmdDisp[0].isConnected()) {
      printf("Test Command Output port not connected!\n");
    }
    else {
      this->m_to_CmdDisp[0].invoke(
          _prmOpcode,
          cmdSeq,
          args
      );
    }

  }

  void ExampleTesterBase ::
    paramSave_anotherparam(U32 cmdSeq)
  {
    Fw::CmdArgBuffer args;
    FwOpcodeType _prmOpcode;
    _prmOpcode = ExampleComponentBase::OPCODE_ANOTHERPARAM_SAVE;
    if (not this->m_to_CmdDisp[0].isConnected()) {
      printf("Test Command Output port not connected!\n");
    }
    else {
      this->m_to_CmdDisp[0].invoke(
          _prmOpcode,
          cmdSeq,
          args
      );
    }
  }

  // ----------------------------------------------------------------------
  // Parameter stringparam
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    paramSet_stringparam(
        const Fw::ParamString& val,
        Fw::ParamValid valid
    )
  {
    this->m_param_stringparam = val;
    this->m_param_stringparam_valid = valid;
  }

  void ExampleTesterBase ::
    paramSend_stringparam(
        NATIVE_INT_TYPE instance,
        U32 cmdSeq
    )
  {

    // Build command for parameter set

    Fw::CmdArgBuffer args;
    FW_ASSERT(
        args.serialize(
            this->m_param_stringparam
        ) == Fw::FW_SERIALIZE_OK
    );
    FwOpcodeType _prmOpcode;
    _prmOpcode =  ExampleComponentBase::OPCODE_STRINGPARAM_SET;
    if (not this->m_to_CmdDisp[0].isConnected()) {
      printf("Test Command Output port not connected!\n");
    }
    else {
      this->m_to_CmdDisp[0].invoke(
          _prmOpcode,
          cmdSeq,
          args
      );
    }

  }

  void ExampleTesterBase ::
    paramSave_stringparam(U32 cmdSeq)
  {
    Fw::CmdArgBuffer args;
    FwOpcodeType _prmOpcode;
    _prmOpcode = ExampleComponentBase::OPCODE_STRINGPARAM_SAVE;
    if (not this->m_to_CmdDisp[0].isConnected()) {
      printf("Test Command Output port not connected!\n");
    }
    else {
      this->m_to_CmdDisp[0].invoke(
          _prmOpcode,
          cmdSeq,
          args
      );
    }
  }

  // ----------------------------------------------------------------------
  // Parameter enumparam
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    paramSet_enumparam(
        const ExampleComponentBase::SomeParamEnum& val,
        Fw::ParamValid valid
    )
  {
    this->m_param_enumparam = val;
    this->m_param_enumparam_valid = valid;
  }

  void ExampleTesterBase ::
    paramSend_enumparam(
        NATIVE_INT_TYPE instance,
        U32 cmdSeq
    )
  {

    // Build command for parameter set

    Fw::CmdArgBuffer args;
    FW_ASSERT(
        args.serialize(
            static_cast<FwEnumStoreType>(this->m_param_enumparam)
        ) == Fw::FW_SERIALIZE_OK
    );
    FwOpcodeType _prmOpcode;
    _prmOpcode =  ExampleComponentBase::OPCODE_ENUMPARAM_SET;
    if (not this->m_to_CmdDisp[0].isConnected()) {
      printf("Test Command Output port not connected!\n");
    }
    else {
      this->m_to_CmdDisp[0].invoke(
          _prmOpcode,
          cmdSeq,
          args
      );
    }

  }

  void ExampleTesterBase ::
    paramSave_enumparam(U32 cmdSeq)
  {
    Fw::CmdArgBuffer args;
    FwOpcodeType _prmOpcode;
    _prmOpcode = ExampleComponentBase::OPCODE_ENUMPARAM_SAVE;
    if (not this->m_to_CmdDisp[0].isConnected()) {
      printf("Test Command Output port not connected!\n");
    }
    else {
      this->m_to_CmdDisp[0].invoke(
          _prmOpcode,
          cmdSeq,
          args
      );
    }
  }

} // end namespace ExampleComponents
