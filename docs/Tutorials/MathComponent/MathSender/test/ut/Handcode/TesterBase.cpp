// ======================================================================
// \title  MathSender/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for MathSender component test harness base class
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

  MathSenderTesterBase ::
    MathSenderTesterBase(
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
    this->tlmHistory_MS_VAL1 = 
      new History<TlmEntry_MS_VAL1>(maxHistorySize);
    this->tlmHistory_MS_VAL2 = 
      new History<TlmEntry_MS_VAL2>(maxHistorySize);
    this->tlmHistory_MS_OP = 
      new History<TlmEntry_MS_OP>(maxHistorySize);
    this->tlmHistory_MS_RES = 
      new History<TlmEntry_MS_RES>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    this->eventHistory_MS_COMMAND_RECV =
      new History<EventEntry_MS_COMMAND_RECV>(maxHistorySize);
    this->eventHistory_MS_RESULT =
      new History<EventEntry_MS_RESULT>(maxHistorySize);
    // Initialize histories for typed user output ports
    this->fromPortHistory_mathOut =
      new History<FromPortEntry_mathOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  MathSenderTesterBase ::
    ~MathSenderTesterBase(void) 
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy telemetry histories
    delete this->tlmHistory_MS_VAL1;
    delete this->tlmHistory_MS_VAL2;
    delete this->tlmHistory_MS_OP;
    delete this->tlmHistory_MS_RES;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
    delete this->eventHistory_MS_COMMAND_RECV;
    delete this->eventHistory_MS_RESULT;
  }

  void MathSenderTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

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

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_from_mathOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_mathOut);
  }

  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_to_mathIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_mathIn);
  }

  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_to_CmdDisp(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CmdDisp);
  }

  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_from_CmdStatus(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdStatus);
  }

  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_from_CmdReg(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdReg);
  }

  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_from_Tlm(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Tlm);
  }

  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_from_Time(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Time);
  }

  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_from_Log(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_Log);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE MathSenderTesterBase ::
    getNum_from_LogText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_LogText);
  }
#endif

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
    connect_to_mathIn(
        const NATIVE_INT_TYPE portNum,
        Ref::InputMathResultPort *const mathIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_mathIn(),static_cast<AssertArg>(portNum));
    this->m_to_mathIn[portNum].addCallPort(mathIn);
  }

  void MathSenderTesterBase ::
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

  void MathSenderTesterBase ::
    invoke_to_mathIn(
        const NATIVE_INT_TYPE portNum,
        F32 result
    )
  {
    FW_ASSERT(portNum < this->getNum_to_mathIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_mathIn(),static_cast<AssertArg>(portNum));
    this->m_to_mathIn[portNum].invoke(
        result
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool MathSenderTesterBase ::
    isConnected_to_mathIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_mathIn(), static_cast<AssertArg>(portNum));
    return this->m_to_mathIn[portNum].isConnected();
  }

  bool MathSenderTesterBase ::
    isConnected_to_CmdDisp(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(), static_cast<AssertArg>(portNum));
    return this->m_to_CmdDisp[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Ref::InputMathOpPort *MathSenderTesterBase ::
    get_from_mathOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_mathOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_mathOut[portNum];
  }

  Fw::InputCmdResponsePort *MathSenderTesterBase ::
    get_from_CmdStatus(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdStatus(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdStatus[portNum];
  }

  Fw::InputCmdRegPort *MathSenderTesterBase ::
    get_from_CmdReg(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdReg(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdReg[portNum];
  }

  Fw::InputTlmPort *MathSenderTesterBase ::
    get_from_Tlm(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Tlm(),static_cast<AssertArg>(portNum));
    return &this->m_from_Tlm[portNum];
  }

  Fw::InputTimePort *MathSenderTesterBase ::
    get_from_Time(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Time(),static_cast<AssertArg>(portNum));
    return &this->m_from_Time[portNum];
  }

  Fw::InputLogPort *MathSenderTesterBase ::
    get_from_Log(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_Log(),static_cast<AssertArg>(portNum));
    return &this->m_from_Log[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *MathSenderTesterBase ::
    get_from_LogText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_LogText(),static_cast<AssertArg>(portNum));
    return &this->m_from_LogText[portNum];
  }
#endif

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
    from_mathOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        F32 val2,
        MathOperation operation
    )
  {
    FW_ASSERT(callComp);
    MathSenderTesterBase* _testerBase = 
      static_cast<MathSenderTesterBase*>(callComp);
    _testerBase->from_mathOut_handlerBase(
        portNum,
        val1, val2, operation
    );
  }

  void MathSenderTesterBase ::
    from_CmdStatus_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    MathSenderTesterBase* _testerBase =
      static_cast<MathSenderTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void MathSenderTesterBase ::
    from_CmdReg_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void MathSenderTesterBase ::
    from_Tlm_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    MathSenderTesterBase* _testerBase =
      static_cast<MathSenderTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void MathSenderTesterBase ::
    from_Log_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    MathSenderTesterBase* _testerBase =
      static_cast<MathSenderTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void MathSenderTesterBase ::
    from_LogText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    MathSenderTesterBase* _testerBase =
      static_cast<MathSenderTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void MathSenderTesterBase ::
    from_Time_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    MathSenderTesterBase* _testerBase =
      static_cast<MathSenderTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_mathOut->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: mathOut
  // ---------------------------------------------------------------------- 

  void MathSenderTesterBase ::
    pushFromPortEntry_mathOut(
        F32 val1,
        F32 val2,
        MathOperation operation
    )
  {
    FromPortEntry_mathOut _e = {
      val1, val2, operation
    };
    this->fromPortHistory_mathOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
    from_mathOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        F32 val1,
        F32 val2,
        MathOperation operation
    )
  {
    FW_ASSERT(portNum < this->getNum_from_mathOut(),static_cast<AssertArg>(portNum));
    this->from_mathOut_handler(
        portNum,
        val1, val2, operation
    );
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
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
  // Command: MS_DO_MATH
  // ---------------------------------------------------------------------- 

  void MathSenderTesterBase ::
    sendCmd_MS_DO_MATH(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        F32 val1,
        F32 val2,
        MathSenderComponentBase::MathOp operation
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(val1);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize(val2);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize((FwEnumStoreType) operation);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = MathSenderComponentBase::OPCODE_MS_DO_MATH + idBase;

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

  
  void MathSenderTesterBase ::
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

  void MathSenderTesterBase ::
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

  void MathSenderTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
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

      case MathSenderComponentBase::CHANNELID_MS_VAL1:
      {
        F32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing MS_VAL1: %d\n", _status);
          return;
        }
        this->tlmInput_MS_VAL1(timeTag, arg);
        break;
      }

      case MathSenderComponentBase::CHANNELID_MS_VAL2:
      {
        F32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing MS_VAL2: %d\n", _status);
          return;
        }
        this->tlmInput_MS_VAL2(timeTag, arg);
        break;
      }

      case MathSenderComponentBase::CHANNELID_MS_OP:
      {
        FwEnumStoreType MS_OParg;
        const Fw::SerializeStatus _status = val.deserialize(MS_OParg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing MS_OP: %d\n", _status);
          return;
        }
        MathSenderComponentBase::MathOpTlm arg = 
          static_cast<MathSenderComponentBase::MathOpTlm>(MS_OParg);
        this->tlmInput_MS_OP(timeTag, arg);
        break;
      }

      case MathSenderComponentBase::CHANNELID_MS_RES:
      {
        F32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing MS_RES: %d\n", _status);
          return;
        }
        this->tlmInput_MS_RES(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void MathSenderTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_MS_VAL1->clear();
    this->tlmHistory_MS_VAL2->clear();
    this->tlmHistory_MS_OP->clear();
    this->tlmHistory_MS_RES->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: MS_VAL1
  // ---------------------------------------------------------------------- 

  void MathSenderTesterBase ::
    tlmInput_MS_VAL1(
        const Fw::Time& timeTag,
        const F32& val
    )
  {
    TlmEntry_MS_VAL1 e = { timeTag, val };
    this->tlmHistory_MS_VAL1->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: MS_VAL2
  // ---------------------------------------------------------------------- 

  void MathSenderTesterBase ::
    tlmInput_MS_VAL2(
        const Fw::Time& timeTag,
        const F32& val
    )
  {
    TlmEntry_MS_VAL2 e = { timeTag, val };
    this->tlmHistory_MS_VAL2->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: MS_OP
  // ---------------------------------------------------------------------- 

  void MathSenderTesterBase ::
    tlmInput_MS_OP(
        const Fw::Time& timeTag,
        const MathSenderComponentBase::MathOpTlm& val
    )
  {
    TlmEntry_MS_OP e = { timeTag, val };
    this->tlmHistory_MS_OP->push_back(e);
    ++this->tlmSize;
  }

  // ---------------------------------------------------------------------- 
  // Channel: MS_RES
  // ---------------------------------------------------------------------- 

  void MathSenderTesterBase ::
    tlmInput_MS_RES(
        const Fw::Time& timeTag,
        const F32& val
    )
  {
    TlmEntry_MS_RES e = { timeTag, val };
    this->tlmHistory_MS_RES->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
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

      case MathSenderComponentBase::EVENTID_MS_COMMAND_RECV: 
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
        F32 val1;
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
        _status = args.deserialize(val1);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        F32 val2;
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
        _status = args.deserialize(val2);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

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
        FwEnumStoreType opInt;
        _status = args.deserialize(opInt);
        MathSenderComponentBase::MathOpEv op = static_cast<MathSenderComponentBase::MathOpEv>(opInt);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_LO_MS_COMMAND_RECV(val1, val2, op);

        break;

      }

      case MathSenderComponentBase::EVENTID_MS_RESULT: 
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
        F32 result;
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
        _status = args.deserialize(result);
        FW_ASSERT(
            _status == Fw::FW_SERIALIZE_OK,
            static_cast<AssertArg>(_status)
        );

        this->logIn_ACTIVITY_HI_MS_RESULT(result);

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void MathSenderTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventHistory_MS_COMMAND_RECV->clear();
    this->eventHistory_MS_RESULT->clear();
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
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

  void MathSenderTesterBase ::
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

  void MathSenderTesterBase ::
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
  // Event: MS_COMMAND_RECV 
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
    logIn_ACTIVITY_LO_MS_COMMAND_RECV(
        F32 val1,
        F32 val2,
        MathSenderComponentBase::MathOpEv op
    )
  {
    EventEntry_MS_COMMAND_RECV e = {
      val1, val2, op
    };
    eventHistory_MS_COMMAND_RECV->push_back(e);
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: MS_RESULT 
  // ----------------------------------------------------------------------

  void MathSenderTesterBase ::
    logIn_ACTIVITY_HI_MS_RESULT(
        F32 result
    )
  {
    EventEntry_MS_RESULT e = {
      result
    };
    eventHistory_MS_RESULT->push_back(e);
    ++this->eventsSize;
  }

} // end namespace Ref
