// ======================================================================
// \title  Test1/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for Test1 component test harness base class
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

namespace Cmd {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Test1TesterBase ::
    Test1TesterBase(
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

  Test1TesterBase ::
    ~Test1TesterBase(void)
  {
    // Destroy command history
    delete this->cmdResponseHistory;
  }

  void Test1TesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

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

  NATIVE_INT_TYPE Test1TesterBase ::
    getNum_to_CmdDisp(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CmdDisp);
  }

  NATIVE_INT_TYPE Test1TesterBase ::
    getNum_from_CmdReg(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdReg);
  }

  NATIVE_INT_TYPE Test1TesterBase ::
    getNum_from_CmdStatus(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CmdStatus);
  }

  NATIVE_INT_TYPE Test1TesterBase ::
    getNum_to_aport(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_aport);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void Test1TesterBase ::
    connect_to_CmdDisp(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdPort *const CmdDisp
    )
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(),static_cast<AssertArg>(portNum));
    this->m_to_CmdDisp[portNum].addCallPort(CmdDisp);
  }

  void Test1TesterBase ::
    connect_to_aport(
        const NATIVE_INT_TYPE portNum,
        Another::InputTest2Port *const aport
    )
  {
    FW_ASSERT(portNum < this->getNum_to_aport(),static_cast<AssertArg>(portNum));
    this->m_to_aport[portNum].addCallPort(aport);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void Test1TesterBase ::
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

  bool Test1TesterBase ::
    isConnected_to_CmdDisp(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CmdDisp(), static_cast<AssertArg>(portNum));
    return this->m_to_CmdDisp[portNum].isConnected();
  }

  bool Test1TesterBase ::
    isConnected_to_aport(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_aport(), static_cast<AssertArg>(portNum));
    return this->m_to_aport[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  Fw::InputCmdRegPort *Test1TesterBase ::
    get_from_CmdReg(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdReg(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdReg[portNum];
  }

  Fw::InputCmdResponsePort *Test1TesterBase ::
    get_from_CmdStatus(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CmdStatus(),static_cast<AssertArg>(portNum));
    return &this->m_from_CmdStatus[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void Test1TesterBase ::
    from_CmdStatus_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    Test1TesterBase* _testerBase =
      static_cast<Test1TesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void Test1TesterBase ::
    from_CmdReg_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void Test1TesterBase ::
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

  void Test1TesterBase ::
    sendCmd_TEST_CMD_1(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        I32 arg1,
        F32 arg2,
        U8 arg3
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(arg1);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize(arg2);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize(arg3);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port

    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = Test1ComponentBase::OPCODE_TEST_CMD_1 + idBase;

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

  void Test1TesterBase ::
    sendCmd_TEST_CMD_2(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        U32 arg1,
        U16 arg2,
        U8 arg3
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize(arg1);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize(arg2);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));
    _status = buff.serialize(arg3);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port

    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = Test1ComponentBase::OPCODE_TEST_CMD_2 + idBase;

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


  void Test1TesterBase ::
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

  void Test1TesterBase ::
    clearHistory()
  {
    this->cmdResponseHistory->clear();
  }

} // end namespace Cmd
