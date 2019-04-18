// ======================================================================
// \title  MathReceiver/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for MathReceiver component test harness base class
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

namespace Ref {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  MathReceiverTesterBase ::
    MathReceiverTesterBase(
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
    this->tlmHistory_MR_OPERATION = 
      new History<TlmEntry_MR_OPERATION>(maxHistorySize);
    this->tlmHistory_MR_FACTOR1S = 
      new History<TlmEntry_MR_FACTOR1S>(maxHistorySize);
    this->tlmHistory_MR_FACTOR1 = 
      new History<TlmEntry_MR_FACTOR1>(maxHistorySize);
    this->tlmHistory_MR_FACTOR2 = 
      new History<TlmEntry_MR_FACTOR2>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_MR_SET_FACTOR1 =
      new History<EventEntry_MR_SET_FACTOR1>(maxHistorySize);
    this->eventHistory_MR_UPDATED_FACTOR2 =
      new History<EventEntry_MR_UPDATED_FACTOR2>(maxHistorySize);
    this->eventHistory_MR_OPERATION_PERFORMED =
      new History<EventEntry_MR_OPERATION_PERFORMED>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_mathOut =
      new History<FromPortEntry_mathOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  MathReceiverTesterBase ::
    ~MathReceiverTesterBase(void) 
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy telemetry histories
    delete this->tlmHistory_MR_OPERATION;
    delete this->tlmHistory_MR_FACTOR1S;
    delete this->tlmHistory_MR_FACTOR1;
    delete this->tlmHistory_MR_FACTOR2;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_MR_SET_FACTOR1;
    delete this->eventHistory_MR_UPDATED_FACTOR2;
    delete this->eventHistory_MR_OPERATION_PERFORMED;
  }

  void MathReceiverTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    this->m_param_factor2_valid = Fw::PARAM_UNINIT;

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port mathOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_mathOut();
        ++_port
    ) {

      this->m_from_mathOut[_port].init();
      this->m_from_mathOut[_port].addCallComp(
          this,
          from_mathOut_static
      );
      this->m_from_mathOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_mathOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_mathOut[_port].setObjName(_portName);
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

    // Initialize output port mathIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_mathIn();
        ++_port
    ) {
      this->m_to_mathIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_mathIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_mathIn[_port].setObjName(_portName);
#endif

    }

    // Initialize output port SchedIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_SchedIn();
        ++_port
    ) {
      this->m_to_SchedIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_SchedIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_SchedIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_to_mathIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_mathIn);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_mathOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_mathOut);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_to_SchedIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_SchedIn);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_to_CmdDisp(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CmdDisp);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_CmdStatus(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdStatus);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_CmdReg(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdReg);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_ParamGet(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_ParamGet);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_ParamSet(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_ParamSet);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_Tlm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Tlm);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE MathReceiverTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
    connect_to_mathIn(
        const NATIVE_INT_TYPE portNum,
        Ref::InputMathOpPort *const mathIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_mathIn(),static_cast<AssertArg>(portNum));
    this->m_to_mathIn[portNum].addCallPort(mathIn);
  }

  void MathReceiverTesterBase ::
    connect_to_SchedIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputSchedPort *const SchedIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_SchedIn(),static_cast<AssertArg>(portNum));
    this->m_to_SchedIn[portNum].addCallPort(SchedIn);
  }

  void MathReceiverTesterBase ::
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

  void MathReceiverTesterBase ::
    invoke_to_mathIn(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        F32 val2,
        MathOperation operation
    )
  {
    FW_ASSERT(portNum < this->getNum_to_mathIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_mathIn(),static_cast<AssertArg>(portNum));
    this->m_to_mathIn[portNum].invoke(
        val1, val2, operation
    );
  }

  void MathReceiverTesterBase ::
    invoke_to_SchedIn(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    FW_ASSERT(portNum < this->getNum_to_SchedIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_SchedIn(),static_cast<AssertArg>(portNum));
    this->m_to_SchedIn[portNum].invoke(
        context
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool MathReceiverTesterBase ::
    isConnected_to_mathIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_mathIn(), static_cast<AssertArg>(portNum));
    return this->m_to_mathIn[portNum].isConnected();
  }

  bool MathReceiverTesterBase ::
    isConnected_to_SchedIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_SchedIn(), static_cast<AssertArg>(portNum));
    return this->m_to_SchedIn[portNum].isConnected();
  }

  bool MathReceiverTesterBase ::
    isConnected_to_CmdDisp(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(), static_cast<AssertArg>(portNum));
    return this->m_to_CmdDisp[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Ref::InputMathResultPort *MathReceiverTesterBase ::
    get_from_mathOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_mathOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_mathOut[portNum];
  }

  Fw::InputCmdResponsePort *MathReceiverTesterBase ::
    get_from_CmdStatus(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdStatus(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdStatus[portNum];
  }

  Fw::InputCmdRegPort *MathReceiverTesterBase ::
    get_from_CmdReg(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdReg(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdReg[portNum];
  }

  Fw::InputPrmGetPort *MathReceiverTesterBase ::
    get_from_ParamGet(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_ParamGet(),static_cast<AssertArg>(portNum));
    return &this->m_from_ParamGet[portNum];
  }

  Fw::InputPrmSetPort *MathReceiverTesterBase ::
    get_from_ParamSet(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_ParamSet(),static_cast<AssertArg>(portNum));
    return &this->m_from_ParamSet[portNum];
  }

  Fw::InputTlmPort *MathReceiverTesterBase ::
    get_from_Tlm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Tlm(),static_cast<AssertArg>(portNum));
    return &this->m_from_Tlm[portNum];
  }

  Fw::InputTimePort *MathReceiverTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputLogPort *MathReceiverTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *MathReceiverTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
    from_mathOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
    FW_ASSERT(callComp);
    MathReceiverTesterBase* _testerBase = 
      static_cast<MathReceiverTesterBase*>(callComp);
    _testerBase->from_mathOut_handlerBase(
        portNum,
        result
    );
  }

  void MathReceiverTesterBase ::
    from_CmdStatus_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    MathReceiverTesterBase* _testerBase =
      static_cast<MathReceiverTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void MathReceiverTesterBase ::
    from_CmdReg_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void MathReceiverTesterBase ::
    from_Tlm_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    MathReceiverTesterBase* _testerBase =
      static_cast<MathReceiverTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void MathReceiverTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    MathReceiverTesterBase* _testerBase =
      static_cast<MathReceiverTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void MathReceiverTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    MathReceiverTesterBase* _testerBase =
      static_cast<MathReceiverTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void MathReceiverTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    MathReceiverTesterBase* _testerBase =
      static_cast<MathReceiverTesterBase*>(component);
    time = _testerBase->m_testTime;
  }


  Fw::ParamValid MathReceiverTesterBase ::
    from_ParamGet_static(
        Fw::PassiveComponentBase* component,
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    MathReceiverTesterBase* _testerBase = 
      static_cast<MathReceiverTesterBase*>(component);

    Fw::SerializeStatus _status;
    Fw::ParamValid _ret = Fw::PARAM_VALID;
    val.resetSer();

    const U32 idBase = _testerBase->getIdBase();
    FW_ASSERT(id >= idBase, id, idBase);

    switch (id - idBase) {
      case 0:
      {
        _status = val.serialize(_testerBase->m_param_factor2);
        _ret = _testerBase->m_param_factor2_valid;
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

  void MathReceiverTesterBase ::
    from_ParamSet_static(
        Fw::PassiveComponentBase* component,
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    MathReceiverTesterBase* _testerBase =
      static_cast<MathReceiverTesterBase*>(component);

    Fw::SerializeStatus _status;
    val.resetDeser();

    // This is exercised completely in autocode,
    // so just verify that it works. If it doesn't
    // it probably is an autocoder error.

    const U32 idBase = _testerBase->getIdBase();
    FW_ASSERT(id >= idBase, id, idBase);

    switch (id - idBase) {
      case 0:
      {
        F32 factor2Val;
        _status = val.deserialize(factor2Val);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );
        FW_ASSERT(
            factor2Val == 
            _testerBase->m_param_factor2
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

  void MathReceiverTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_mathOut->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: mathOut
  // ---------------------------------------------------------------------- 

  void MathReceiverTesterBase ::
    pushFromPortEntry_mathOut(
        F32 result
    )
  {
    FromPortEntry_mathOut _e = {
      result
    };
    this->fromPortHistory_mathOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
    from_mathOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
    FW_ASSERT(portNum < this->getNum_from_mathOut(),static_cast<AssertArg>(portNum));
    this->from_mathOut_handler(
        portNum,
        result
    );
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
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
  // Command: MR_SET_FACTOR1
  // ---------------------------------------------------------------------- 

  void MathReceiverTesterBase ::
    sendCmd_MR_SET_FACTOR1(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        F32 val
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(val);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = MathReceiverComponentBase::OPCODE_MR_SET_FACTOR1 + idBase;

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
  // Command: MR_CLEAR_EVENT_THROTTLE
  // ---------------------------------------------------------------------- 

  void MathReceiverTesterBase ::
    sendCmd_MR_CLEAR_EVENT_THROTTLE(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = MathReceiverComponentBase::OPCODE_MR_CLEAR_EVENT_THROTTLE + idBase;

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

  
  void MathReceiverTesterBase ::
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

  void MathReceiverTesterBase ::
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

  void MathReceiverTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
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

      case MathReceiverComponentBase::CHANNELID_MR_OPERATION:
      {
        Ref::MathOp arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing MR_OPERATION: %d\n", _status);
          return;
        }
        this->tlmInput_MR_OPERATION(timeTag, arg);
        break;
      }

      case MathReceiverComponentBase::CHANNELID_MR_FACTOR1S:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing MR_FACTOR1S: %d\n", _status);
          return;
        }
        this->tlmInput_MR_FACTOR1S(timeTag, arg);
        break;
      }

      case MathReceiverComponentBase::CHANNELID_MR_FACTOR1:
      {
        F32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing MR_FACTOR1: %d\n", _status);
          return;
        }
        this->tlmInput_MR_FACTOR1(timeTag, arg);
        break;
      }

      case MathReceiverComponentBase::CHANNELID_MR_FACTOR2:
      {
        F32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing MR_FACTOR2: %d\n", _status);
          return;
        }
        this->tlmInput_MR_FACTOR2(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void MathReceiverTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_MR_OPERATION->clear();
    this->tlmHistory_MR_FACTOR1S->clear();
    this->tlmHistory_MR_FACTOR1->clear();
    this->tlmHistory_MR_FACTOR2->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: MR_OPERATION
  // ---------------------------------------------------------------------- 

  void MathReceiverTesterBase ::
    tlmInput_MR_OPERATION(
        const Fw::Time& timeTag,
        const Ref::MathOp& val
    )
  {
    TlmEntry_MR_OPERATION e = { timeTag, val };
    this->tlmHistory_MR_OPERATION->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: MR_FACTOR1S
  // ---------------------------------------------------------------------- 

  void MathReceiverTesterBase ::
    tlmInput_MR_FACTOR1S(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_MR_FACTOR1S e = { timeTag, val };
    this->tlmHistory_MR_FACTOR1S->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: MR_FACTOR1
  // ---------------------------------------------------------------------- 

  void MathReceiverTesterBase ::
    tlmInput_MR_FACTOR1(
        const Fw::Time& timeTag,
        const F32& val
    )
  {
    TlmEntry_MR_FACTOR1 e = { timeTag, val };
    this->tlmHistory_MR_FACTOR1->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: MR_FACTOR2
  // ---------------------------------------------------------------------- 

  void MathReceiverTesterBase ::
    tlmInput_MR_FACTOR2(
        const Fw::Time& timeTag,
        const F32& val
    )
  {
    TlmEntry_MR_FACTOR2 e = { timeTag, val };
    this->tlmHistory_MR_FACTOR2->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
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

      case MathReceiverComponentBase::EVENTID_MR_SET_FACTOR1: 
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
        F32 val;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(F32),_argSize,sizeof(F32));
        }
#endif      
        _status = args.deserialize(val);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_MR_SET_FACTOR1(val);

        break;

      }

      case MathReceiverComponentBase::EVENTID_MR_UPDATED_FACTOR2: 
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
        F32 val;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(F32),_argSize,sizeof(F32));
        }
#endif      
        _status = args.deserialize(val);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_MR_UPDATED_FACTOR2(val);

        break;

      }

      case MathReceiverComponentBase::EVENTID_MR_OPERATION_PERFORMED: 
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
        Ref::MathOp val;
#if FW_AMPCS_COMPATIBLE
        {
          // Deserialize the argument size
          U8 _argSize;
          _status = args.deserialize(_argSize);
          FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
          );
          FW_ASSERT(_argSize == sizeof(Ref::MathOp),_argSize,sizeof(Ref::MathOp));
        }
#endif      
        _status = args.deserialize(val);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_MR_OPERATION_PERFORMED(val);

        break;

      }

      case MathReceiverComponentBase::EVENTID_MR_THROTTLE_CLEARED: 
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
        this->logIn_ACTIVITY_HI_MR_THROTTLE_CLEARED();

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void MathReceiverTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_MR_SET_FACTOR1->clear();
    this->eventHistory_MR_UPDATED_FACTOR2->clear();
    this->eventHistory_MR_OPERATION_PERFORMED->clear();
    this->eventsSize_MR_THROTTLE_CLEARED = 0;
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
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

  void MathReceiverTesterBase ::
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

  void MathReceiverTesterBase ::
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
  // Event: MR_SET_FACTOR1 
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
    logIn_ACTIVITY_HI_MR_SET_FACTOR1(
        F32 val
    )
  {
    EventEntry_MR_SET_FACTOR1 e = {
      val
    };
    eventHistory_MR_SET_FACTOR1->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: MR_UPDATED_FACTOR2 
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
    logIn_ACTIVITY_HI_MR_UPDATED_FACTOR2(
        F32 val
    )
  {
    EventEntry_MR_UPDATED_FACTOR2 e = {
      val
    };
    eventHistory_MR_UPDATED_FACTOR2->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: MR_OPERATION_PERFORMED 
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
    logIn_ACTIVITY_HI_MR_OPERATION_PERFORMED(
        Ref::MathOp val
    )
  {
    EventEntry_MR_OPERATION_PERFORMED e = {
      val
    };
    eventHistory_MR_OPERATION_PERFORMED->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: MR_THROTTLE_CLEARED 
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
    logIn_ACTIVITY_HI_MR_THROTTLE_CLEARED(
        void
    )
  {
    ++this->eventsSize_MR_THROTTLE_CLEARED;
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Parameter factor2 
  // ----------------------------------------------------------------------

  void MathReceiverTesterBase ::
    paramSet_factor2(
        const F32& val,
        Fw::ParamValid valid
    )
  {
    this->m_param_factor2 = val;
    this->m_param_factor2_valid = valid;
  }

  void MathReceiverTesterBase ::
    paramSend_factor2(
        NATIVE_INT_TYPE instance, 
        U32 cmdSeq
    )
  {

    // Build command for parameter set

    Fw::CmdArgBuffer args;
    FW_ASSERT(
        args.serialize(
            this->m_param_factor2
        ) == Fw::FW_SERIALIZE_OK
    );
    const U32 idBase = this->getIdBase();
    FwOpcodeType _prmOpcode;
    _prmOpcode =  MathReceiverComponentBase::OPCODE_FACTOR2_SET + idBase;
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

  void MathReceiverTesterBase ::
    paramSave_factor2 (
        NATIVE_INT_TYPE instance, 
        U32 cmdSeq
    )
    
  {
    Fw::CmdArgBuffer args;
    FwOpcodeType _prmOpcode;
    const U32 idBase = this->getIdBase();
    _prmOpcode = MathReceiverComponentBase::OPCODE_FACTOR2_SAVE + idBase;
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

} // end namespace Ref
