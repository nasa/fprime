// ======================================================================
// \title  CommandTester/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for CommandTester component test harness base class
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

  CommandTesterTesterBase ::
    CommandTesterTesterBase(
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
    // Initialize histories for typed user output ports
    this->fromPortHistory_cmdSendPort =
      new History<FromPortEntry_cmdSendPort>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  CommandTesterTesterBase ::
    ~CommandTesterTesterBase(void)
  {
  }

  void CommandTesterTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port cmdSendPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_cmdSendPort();
        ++_port
    ) {

      this->m_from_cmdSendPort[_port].init();
      this->m_from_cmdSendPort[_port].addCallComp(
          this,
          from_cmdSendPort_static
      );
      this->m_from_cmdSendPort[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_cmdSendPort[%d]",
          this->m_objName,
          _port
      );
      this->m_from_cmdSendPort[_port].setObjName(_portName);
#endif

    }

    // Initialize output port cmdStatusPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_cmdStatusPort();
        ++_port
    ) {
      this->m_to_cmdStatusPort[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_cmdStatusPort[%d]",
          this->m_objName,
          _port
      );
      this->m_to_cmdStatusPort[_port].setObjName(_portName);
#endif

    }

    // Initialize output port cmdRegPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_cmdRegPort();
        ++_port
    ) {
      this->m_to_cmdRegPort[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_cmdRegPort[%d]",
          this->m_objName,
          _port
      );
      this->m_to_cmdRegPort[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE CommandTesterTesterBase ::
    getNum_from_cmdSendPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdSendPort);
  }

  NATIVE_INT_TYPE CommandTesterTesterBase ::
    getNum_to_cmdStatusPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_cmdStatusPort);
  }

  NATIVE_INT_TYPE CommandTesterTesterBase ::
    getNum_to_cmdRegPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_cmdRegPort);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void CommandTesterTesterBase ::
    connect_to_cmdStatusPort(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdResponsePort *const cmdStatusPort
    )
  {
    FW_ASSERT(portNum < this->getNum_to_cmdStatusPort(),static_cast<AssertArg>(portNum));
    this->m_to_cmdStatusPort[portNum].addCallPort(cmdStatusPort);
  }

  void CommandTesterTesterBase ::
    connect_to_cmdRegPort(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdRegPort *const cmdRegPort
    )
  {
    FW_ASSERT(portNum < this->getNum_to_cmdRegPort(),static_cast<AssertArg>(portNum));
    this->m_to_cmdRegPort[portNum].addCallPort(cmdRegPort);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void CommandTesterTesterBase ::
    invoke_to_cmdStatusPort(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CommandResponse response
    )
  {
    FW_ASSERT(portNum < this->getNum_to_cmdStatusPort(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_cmdStatusPort(),static_cast<AssertArg>(portNum));
    this->m_to_cmdStatusPort[portNum].invoke(
        opCode, cmdSeq, response
    );
  }

  void CommandTesterTesterBase ::
    invoke_to_cmdRegPort(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode
    )
  {
    FW_ASSERT(portNum < this->getNum_to_cmdRegPort(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_cmdRegPort(),static_cast<AssertArg>(portNum));
    this->m_to_cmdRegPort[portNum].invoke(
        opCode
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool CommandTesterTesterBase ::
    isConnected_to_cmdStatusPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_cmdStatusPort(), static_cast<AssertArg>(portNum));
    return this->m_to_cmdStatusPort[portNum].isConnected();
  }

  bool CommandTesterTesterBase ::
    isConnected_to_cmdRegPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_cmdRegPort(), static_cast<AssertArg>(portNum));
    return this->m_to_cmdRegPort[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  Fw::InputCmdPort *CommandTesterTesterBase ::
    get_from_cmdSendPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdSendPort(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdSendPort[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void CommandTesterTesterBase ::
    from_cmdSendPort_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {
    FW_ASSERT(callComp);
    CommandTesterTesterBase* _testerBase =
      static_cast<CommandTesterTesterBase*>(callComp);
    _testerBase->from_cmdSendPort_handlerBase(
        portNum,
        opCode, cmdSeq, args
    );
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void CommandTesterTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_cmdSendPort->clear();
  }

  // ----------------------------------------------------------------------
  // From port: cmdSendPort
  // ----------------------------------------------------------------------

  void CommandTesterTesterBase ::
    pushFromPortEntry_cmdSendPort(
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {
    FromPortEntry_cmdSendPort _e = {
      opCode, cmdSeq, args
    };
    this->fromPortHistory_cmdSendPort->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void CommandTesterTesterBase ::
    from_cmdSendPort_handlerBase(
        const NATIVE_INT_TYPE portNum,
        FwOpcodeType opCode,
        U32 cmdSeq,
        Fw::CmdArgBuffer &args
    )
  {
    FW_ASSERT(portNum < this->getNum_from_cmdSendPort(),static_cast<AssertArg>(portNum));
    this->from_cmdSendPort_handler(
        portNum,
        opCode, cmdSeq, args
    );
  }

  // ----------------------------------------------------------------------
  // History
  // ----------------------------------------------------------------------

  void CommandTesterTesterBase ::
    clearHistory()
  {
    this->clearFromPortHistory();
  }

} // end namespace Cmd
