// ======================================================================
// \title  TestPrm/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for TestPrm component test harness base class
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

namespace Prm {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestPrmTesterBase ::
    TestPrmTesterBase(
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
    // Clear history
    this->clearHistory();
  }

  TestPrmTesterBase ::
    ~TestPrmTesterBase(void)
  {
    // Destroy command history
    delete this->cmdResponseHistory;
  }

  void TestPrmTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    this->m_param_someparam_valid = Fw::PARAM_UNINIT;

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

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

  NATIVE_INT_TYPE TestPrmTesterBase ::
    getNum_to_CmdDisp(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CmdDisp);
  }

  NATIVE_INT_TYPE TestPrmTesterBase ::
    getNum_from_CmdReg(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdReg);
  }

  NATIVE_INT_TYPE TestPrmTesterBase ::
    getNum_from_CmdStatus(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdStatus);
  }

  NATIVE_INT_TYPE TestPrmTesterBase ::
    getNum_from_ParamGet(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_ParamGet);
  }

  NATIVE_INT_TYPE TestPrmTesterBase ::
    getNum_from_ParamSet(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_ParamSet);
  }

  NATIVE_INT_TYPE TestPrmTesterBase ::
    getNum_to_aport(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_aport);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void TestPrmTesterBase ::
    connect_to_CmdDisp(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdPort *const CmdDisp
    )
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(),static_cast<AssertArg>(portNum));
    this->m_to_CmdDisp[portNum].addCallPort(CmdDisp);
  }

  void TestPrmTesterBase ::
    connect_to_aport(
        const NATIVE_INT_TYPE portNum,
        Another::InputTestPort *const aport
    )
  {
    FW_ASSERT(portNum < this->getNum_to_aport(),static_cast<AssertArg>(portNum));
    this->m_to_aport[portNum].addCallPort(aport);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void TestPrmTesterBase ::
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

  bool TestPrmTesterBase ::
    isConnected_to_CmdDisp(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(), static_cast<AssertArg>(portNum));
    return this->m_to_CmdDisp[portNum].isConnected();
  }

  bool TestPrmTesterBase ::
    isConnected_to_aport(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_aport(), static_cast<AssertArg>(portNum));
    return this->m_to_aport[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  Fw::InputCmdRegPort *TestPrmTesterBase ::
    get_from_CmdReg(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdReg(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdReg[portNum];
  }

  Fw::InputCmdResponsePort *TestPrmTesterBase ::
    get_from_CmdStatus(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdStatus(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdStatus[portNum];
  }

  Fw::InputPrmGetPort *TestPrmTesterBase ::
    get_from_ParamGet(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_ParamGet(),static_cast<AssertArg>(portNum));
    return &this->m_from_ParamGet[portNum];
  }

  Fw::InputPrmSetPort *TestPrmTesterBase ::
    get_from_ParamSet(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_ParamSet(),static_cast<AssertArg>(portNum));
    return &this->m_from_ParamSet[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void TestPrmTesterBase ::
    from_CmdStatus_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    TestPrmTesterBase* _testerBase =
      static_cast<TestPrmTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void TestPrmTesterBase ::
    from_CmdReg_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }


  Fw::ParamValid TestPrmTesterBase ::
    from_ParamGet_static(
        Fw::PassiveComponentBase* component,
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    TestPrmTesterBase* _testerBase =
      static_cast<TestPrmTesterBase*>(component);

    Fw::SerializeStatus _status;
    Fw::ParamValid _ret = Fw::PARAM_VALID;
    val.resetSer();

    const U32 idBase = _testerBase->getIdBase();
    FW_ASSERT(id >= idBase, id, idBase);

    switch (id - idBase) {
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
      default:
        FW_ASSERT(id);
        break;
    }

    return _ret;
  }

  void TestPrmTesterBase ::
    from_ParamSet_static(
        Fw::PassiveComponentBase* component,
        NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    TestPrmTesterBase* _testerBase =
      static_cast<TestPrmTesterBase*>(component);

    Fw::SerializeStatus _status;
    val.resetDeser();

    // This is exercised completely in autocode,
    // so just verify that it works. If it doesn't
    // it probably is an autocoder error.

    const U32 idBase = _testerBase->getIdBase();
    FW_ASSERT(id >= idBase, id, idBase);

    switch (id - idBase) {
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

      default: {
        FW_ASSERT(id);
        break;
      }

    }
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void TestPrmTesterBase ::
    cmdResponseIn(
        const FwOpcodeType opCode,
        const U32 seq,
        const Fw::CommandResponse response
    )
  {
    CmdResponse e = { opCode, seq, response };
    this->cmdResponseHistory->push_back(e);
  }


  void TestPrmTesterBase ::
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

  void TestPrmTesterBase ::
    clearHistory()
  {
    this->cmdResponseHistory->clear();
  }

  // ----------------------------------------------------------------------
  // Parameter someparam
  // ----------------------------------------------------------------------

  void TestPrmTesterBase ::
    paramSet_someparam(
        const U32& val,
        Fw::ParamValid valid
    )
  {
    this->m_param_someparam = val;
    this->m_param_someparam_valid = valid;
  }

  void TestPrmTesterBase ::
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
    const U32 idBase = this->getIdBase();
    FwOpcodeType _prmOpcode;
    _prmOpcode =  TestPrmComponentBase::OPCODE_SOMEPARAM_SET + idBase;
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

  void TestPrmTesterBase ::
    paramSave_someparam (
        NATIVE_INT_TYPE instance,
        U32 cmdSeq
    )

  {
    Fw::CmdArgBuffer args;
    FwOpcodeType _prmOpcode;
    const U32 idBase = this->getIdBase();
    _prmOpcode = TestPrmComponentBase::OPCODE_SOMEPARAM_SAVE + idBase;
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

} // end namespace Prm
