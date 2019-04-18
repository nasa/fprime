// ======================================================================
// \title  BufferAccumulator/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for BufferAccumulator component test harness base class
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

  BufferAccumulatorTesterBase ::
    BufferAccumulatorTesterBase(
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
    this->tlmHistory_BufferAccumulator_NumQueuedBuffers = 
      new History<TlmEntry_BufferAccumulator_NumQueuedBuffers>(maxHistorySize);
    // Initialize event histories
#if FW_ENABLE_TEXT_LOGGING
    this->textLogHistory = new History<TextLogEntry>(maxHistorySize);
#endif
    // Initialize histories for typed user output ports
    this->fromPortHistory_bufferSendOutDrain =
      new History<FromPortEntry_bufferSendOutDrain>(maxHistorySize);
    this->fromPortHistory_bufferSendOutReturn =
      new History<FromPortEntry_bufferSendOutReturn>(maxHistorySize);
    this->fromPortHistory_pingOut =
      new History<FromPortEntry_pingOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  BufferAccumulatorTesterBase ::
    ~BufferAccumulatorTesterBase(void) 
  {
    // Destroy command history
    delete this->cmdResponseHistory;
    // Destroy telemetry histories
    delete this->tlmHistory_BufferAccumulator_NumQueuedBuffers;
    // Destroy event histories
#if FW_ENABLE_TEXT_LOGGING
    delete this->textLogHistory;
#endif
  }

  void BufferAccumulatorTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port bufferSendOutDrain

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_bufferSendOutDrain();
        ++_port
    ) {

      this->m_from_bufferSendOutDrain[_port].init();
      this->m_from_bufferSendOutDrain[_port].addCallComp(
          this,
          from_bufferSendOutDrain_static
      );
      this->m_from_bufferSendOutDrain[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_bufferSendOutDrain[%d]",
          this->m_objName,
          _port
      );
      this->m_from_bufferSendOutDrain[_port].setObjName(_portName);
#endif

    }

    // Attach input port bufferSendOutReturn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_bufferSendOutReturn();
        ++_port
    ) {

      this->m_from_bufferSendOutReturn[_port].init();
      this->m_from_bufferSendOutReturn[_port].addCallComp(
          this,
          from_bufferSendOutReturn_static
      );
      this->m_from_bufferSendOutReturn[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_bufferSendOutReturn[%d]",
          this->m_objName,
          _port
      );
      this->m_from_bufferSendOutReturn[_port].setObjName(_portName);
#endif

    }

    // Attach input port cmdRegOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_cmdRegOut();
        ++_port
    ) {

      this->m_from_cmdRegOut[_port].init();
      this->m_from_cmdRegOut[_port].addCallComp(
          this,
          from_cmdRegOut_static
      );
      this->m_from_cmdRegOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_cmdRegOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_cmdRegOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port cmdResponseOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_cmdResponseOut();
        ++_port
    ) {

      this->m_from_cmdResponseOut[_port].init();
      this->m_from_cmdResponseOut[_port].addCallComp(
          this,
          from_cmdResponseOut_static
      );
      this->m_from_cmdResponseOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_cmdResponseOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_cmdResponseOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port eventOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_eventOut();
        ++_port
    ) {

      this->m_from_eventOut[_port].init();
      this->m_from_eventOut[_port].addCallComp(
          this,
          from_eventOut_static
      );
      this->m_from_eventOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_eventOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_eventOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port eventOutText

#if FW_ENABLE_TEXT_LOGGING == 1
    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_eventOutText();
        ++_port
    ) {

      this->m_from_eventOutText[_port].init();
      this->m_from_eventOutText[_port].addCallComp(
          this,
          from_eventOutText_static
      );
      this->m_from_eventOutText[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_eventOutText[%d]",
          this->m_objName,
          _port
      );
      this->m_from_eventOutText[_port].setObjName(_portName);
#endif

    }
#endif

    // Attach input port pingOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_pingOut();
        ++_port
    ) {

      this->m_from_pingOut[_port].init();
      this->m_from_pingOut[_port].addCallComp(
          this,
          from_pingOut_static
      );
      this->m_from_pingOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_pingOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_pingOut[_port].setObjName(_portName);
#endif

    }

    // Attach input port timeCaller

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_timeCaller();
        ++_port
    ) {

      this->m_from_timeCaller[_port].init();
      this->m_from_timeCaller[_port].addCallComp(
          this,
          from_timeCaller_static
      );
      this->m_from_timeCaller[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_timeCaller[%d]",
          this->m_objName,
          _port
      );
      this->m_from_timeCaller[_port].setObjName(_portName);
#endif

    }

    // Attach input port tlmOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_tlmOut();
        ++_port
    ) {

      this->m_from_tlmOut[_port].init();
      this->m_from_tlmOut[_port].addCallComp(
          this,
          from_tlmOut_static
      );
      this->m_from_tlmOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_tlmOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_tlmOut[_port].setObjName(_portName);
#endif

    }

    // Initialize output port bufferSendInFill

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_bufferSendInFill();
        ++_port
    ) {
      this->m_to_bufferSendInFill[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_bufferSendInFill[%d]",
          this->m_objName,
          _port
      );
      this->m_to_bufferSendInFill[_port].setObjName(_portName);
#endif

    }

    // Initialize output port bufferSendInReturn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_bufferSendInReturn();
        ++_port
    ) {
      this->m_to_bufferSendInReturn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_bufferSendInReturn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_bufferSendInReturn[_port].setObjName(_portName);
#endif

    }

    // Initialize output port pingIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_pingIn();
        ++_port
    ) {
      this->m_to_pingIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_pingIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_pingIn[_port].setObjName(_portName);
#endif

    }

    // Initialize output port schedIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_schedIn();
        ++_port
    ) {
      this->m_to_schedIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_schedIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_schedIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_to_bufferSendInFill(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_bufferSendInFill);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_to_bufferSendInReturn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_bufferSendInReturn);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_bufferSendOutDrain(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_bufferSendOutDrain);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_bufferSendOutReturn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_bufferSendOutReturn);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_to_cmdIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_cmdIn);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_cmdRegOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdRegOut);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_cmdResponseOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_cmdResponseOut);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_eventOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_eventOut);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_eventOutText(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_eventOutText);
  }
#endif

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_to_pingIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_pingIn);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_pingOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_pingOut);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_timeCaller(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_timeCaller);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_to_schedIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_schedIn);
  }

  NATIVE_INT_TYPE BufferAccumulatorTesterBase ::
    getNum_from_tlmOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_tlmOut);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
    connect_to_bufferSendInFill(
        const NATIVE_INT_TYPE portNum,
        Fw::InputBufferSendPort *const bufferSendInFill
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendInFill(),static_cast<AssertArg>(portNum));
    this->m_to_bufferSendInFill[portNum].addCallPort(bufferSendInFill);
  }

  void BufferAccumulatorTesterBase ::
    connect_to_bufferSendInReturn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputBufferSendPort *const bufferSendInReturn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendInReturn(),static_cast<AssertArg>(portNum));
    this->m_to_bufferSendInReturn[portNum].addCallPort(bufferSendInReturn);
  }

  void BufferAccumulatorTesterBase ::
    connect_to_cmdIn(
        const NATIVE_INT_TYPE portNum,
        Fw::InputCmdPort *const cmdIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_cmdIn(),static_cast<AssertArg>(portNum));
    this->m_to_cmdIn[portNum].addCallPort(cmdIn);
  }

  void BufferAccumulatorTesterBase ::
    connect_to_pingIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputPingPort *const pingIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    this->m_to_pingIn[portNum].addCallPort(pingIn);
  }

  void BufferAccumulatorTesterBase ::
    connect_to_schedIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputSchedPort *const schedIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_schedIn(),static_cast<AssertArg>(portNum));
    this->m_to_schedIn[portNum].addCallPort(schedIn);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
    invoke_to_bufferSendInFill(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendInFill(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_bufferSendInFill(),static_cast<AssertArg>(portNum));
    this->m_to_bufferSendInFill[portNum].invoke(
        fwBuffer
    );
  }

  void BufferAccumulatorTesterBase ::
    invoke_to_bufferSendInReturn(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendInReturn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_bufferSendInReturn(),static_cast<AssertArg>(portNum));
    this->m_to_bufferSendInReturn[portNum].invoke(
        fwBuffer
    );
  }

  void BufferAccumulatorTesterBase ::
    invoke_to_pingIn(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_pingIn(),static_cast<AssertArg>(portNum));
    this->m_to_pingIn[portNum].invoke(
        key
    );
  }

  void BufferAccumulatorTesterBase ::
    invoke_to_schedIn(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    FW_ASSERT(portNum < this->getNum_to_schedIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_schedIn(),static_cast<AssertArg>(portNum));
    this->m_to_schedIn[portNum].invoke(
        context
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool BufferAccumulatorTesterBase ::
    isConnected_to_bufferSendInFill(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendInFill(), static_cast<AssertArg>(portNum));
    return this->m_to_bufferSendInFill[portNum].isConnected();
  }

  bool BufferAccumulatorTesterBase ::
    isConnected_to_bufferSendInReturn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_bufferSendInReturn(), static_cast<AssertArg>(portNum));
    return this->m_to_bufferSendInReturn[portNum].isConnected();
  }

  bool BufferAccumulatorTesterBase ::
    isConnected_to_cmdIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_cmdIn(), static_cast<AssertArg>(portNum));
    return this->m_to_cmdIn[portNum].isConnected();
  }

  bool BufferAccumulatorTesterBase ::
    isConnected_to_pingIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_pingIn(), static_cast<AssertArg>(portNum));
    return this->m_to_pingIn[portNum].isConnected();
  }

  bool BufferAccumulatorTesterBase ::
    isConnected_to_schedIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_schedIn(), static_cast<AssertArg>(portNum));
    return this->m_to_schedIn[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Fw::InputBufferSendPort *BufferAccumulatorTesterBase ::
    get_from_bufferSendOutDrain(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_bufferSendOutDrain(),static_cast<AssertArg>(portNum));
    return &this->m_from_bufferSendOutDrain[portNum];
  }

  Fw::InputBufferSendPort *BufferAccumulatorTesterBase ::
    get_from_bufferSendOutReturn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_bufferSendOutReturn(),static_cast<AssertArg>(portNum));
    return &this->m_from_bufferSendOutReturn[portNum];
  }

  Fw::InputCmdRegPort *BufferAccumulatorTesterBase ::
    get_from_cmdRegOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdRegOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdRegOut[portNum];
  }

  Fw::InputCmdResponsePort *BufferAccumulatorTesterBase ::
    get_from_cmdResponseOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_cmdResponseOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_cmdResponseOut[portNum];
  }

  Fw::InputLogPort *BufferAccumulatorTesterBase ::
    get_from_eventOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_eventOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_eventOut[portNum];
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  Fw::InputLogTextPort *BufferAccumulatorTesterBase ::
    get_from_eventOutText(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_eventOutText(),static_cast<AssertArg>(portNum));
    return &this->m_from_eventOutText[portNum];
  }
#endif

  Svc::InputPingPort *BufferAccumulatorTesterBase ::
    get_from_pingOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_pingOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_pingOut[portNum];
  }

  Fw::InputTimePort *BufferAccumulatorTesterBase ::
    get_from_timeCaller(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_timeCaller(),static_cast<AssertArg>(portNum));
    return &this->m_from_timeCaller[portNum];
  }

  Fw::InputTlmPort *BufferAccumulatorTesterBase ::
    get_from_tlmOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_tlmOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_tlmOut[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
    from_bufferSendOutDrain_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(callComp);
    BufferAccumulatorTesterBase* _testerBase = 
      static_cast<BufferAccumulatorTesterBase*>(callComp);
    _testerBase->from_bufferSendOutDrain_handlerBase(
        portNum,
        fwBuffer
    );
  }

  void BufferAccumulatorTesterBase ::
    from_bufferSendOutReturn_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(callComp);
    BufferAccumulatorTesterBase* _testerBase = 
      static_cast<BufferAccumulatorTesterBase*>(callComp);
    _testerBase->from_bufferSendOutReturn_handlerBase(
        portNum,
        fwBuffer
    );
  }

  void BufferAccumulatorTesterBase ::
    from_pingOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(callComp);
    BufferAccumulatorTesterBase* _testerBase = 
      static_cast<BufferAccumulatorTesterBase*>(callComp);
    _testerBase->from_pingOut_handlerBase(
        portNum,
        key
    );
  }

  void BufferAccumulatorTesterBase ::
    from_cmdResponseOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        const Fw::CommandResponse response
    )
  {
    BufferAccumulatorTesterBase* _testerBase =
      static_cast<BufferAccumulatorTesterBase*>(component);
    _testerBase->cmdResponseIn(opCode, cmdSeq, response);
  }

  void BufferAccumulatorTesterBase ::
    from_cmdRegOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        const FwOpcodeType opCode
    )
  {

  }

  void BufferAccumulatorTesterBase ::
    from_tlmOut_static(
        Fw::PassiveComponentBase *const component,
        NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    BufferAccumulatorTesterBase* _testerBase =
      static_cast<BufferAccumulatorTesterBase*>(component);
    _testerBase->dispatchTlm(id, timeTag, val);
  }

  void BufferAccumulatorTesterBase ::
    from_eventOut_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::LogSeverity severity,
        Fw::LogBuffer &args
    )
  {
    BufferAccumulatorTesterBase* _testerBase =
      static_cast<BufferAccumulatorTesterBase*>(component);
    _testerBase->dispatchEvents(id, timeTag, severity, args);
  }

#if FW_ENABLE_TEXT_LOGGING == 1
  void BufferAccumulatorTesterBase ::
    from_eventOutText_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        FwEventIdType id,
        Fw::Time &timeTag,
        Fw::TextLogSeverity severity,
        Fw::TextLogString &text
    )
  {
    BufferAccumulatorTesterBase* _testerBase =
      static_cast<BufferAccumulatorTesterBase*>(component);
    _testerBase->textLogIn(id,timeTag,severity,text);
  }
#endif

  void BufferAccumulatorTesterBase ::
    from_timeCaller_static(
        Fw::PassiveComponentBase *const component,
        const NATIVE_INT_TYPE portNum,
        Fw::Time& time
    )
  {
    BufferAccumulatorTesterBase* _testerBase =
      static_cast<BufferAccumulatorTesterBase*>(component);
    time = _testerBase->m_testTime;
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_bufferSendOutDrain->clear();
    this->fromPortHistory_bufferSendOutReturn->clear();
    this->fromPortHistory_pingOut->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: bufferSendOutDrain
  // ---------------------------------------------------------------------- 

  void BufferAccumulatorTesterBase ::
    pushFromPortEntry_bufferSendOutDrain(
        Fw::Buffer &fwBuffer
    )
  {
    FromPortEntry_bufferSendOutDrain _e = {
      fwBuffer
    };
    this->fromPortHistory_bufferSendOutDrain->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ---------------------------------------------------------------------- 
  // From port: bufferSendOutReturn
  // ---------------------------------------------------------------------- 

  void BufferAccumulatorTesterBase ::
    pushFromPortEntry_bufferSendOutReturn(
        Fw::Buffer &fwBuffer
    )
  {
    FromPortEntry_bufferSendOutReturn _e = {
      fwBuffer
    };
    this->fromPortHistory_bufferSendOutReturn->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ---------------------------------------------------------------------- 
  // From port: pingOut
  // ---------------------------------------------------------------------- 

  void BufferAccumulatorTesterBase ::
    pushFromPortEntry_pingOut(
        U32 key
    )
  {
    FromPortEntry_pingOut _e = {
      key
    };
    this->fromPortHistory_pingOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
    from_bufferSendOutDrain_handlerBase(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_from_bufferSendOutDrain(),static_cast<AssertArg>(portNum));
    this->from_bufferSendOutDrain_handler(
        portNum,
        fwBuffer
    );
  }

  void BufferAccumulatorTesterBase ::
    from_bufferSendOutReturn_handlerBase(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    FW_ASSERT(portNum < this->getNum_from_bufferSendOutReturn(),static_cast<AssertArg>(portNum));
    this->from_bufferSendOutReturn_handler(
        portNum,
        fwBuffer
    );
  }

  void BufferAccumulatorTesterBase ::
    from_pingOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        U32 key
    )
  {
    FW_ASSERT(portNum < this->getNum_from_pingOut(),static_cast<AssertArg>(portNum));
    this->from_pingOut_handler(
        portNum,
        key
    );
  }

  // ----------------------------------------------------------------------
  // Command response handling
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
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
  // Command: BA_SetMode
  // ---------------------------------------------------------------------- 

  void BufferAccumulatorTesterBase ::
    sendCmd_BA_SetMode(
        const NATIVE_INT_TYPE instance,
        const U32 cmdSeq,
        BufferAccumulatorComponentBase::OpState mode
    )
  {

    // Serialize arguments

    Fw::CmdArgBuffer buff;
    Fw::SerializeStatus _status;
    _status = buff.serialize((FwEnumStoreType) mode);
    FW_ASSERT(_status == Fw::FW_SERIALIZE_OK,static_cast<AssertArg>(_status));

    // Call output command port
    
    FwOpcodeType _opcode;
    const U32 idBase = this->getIdBase();
    _opcode = BufferAccumulatorComponentBase::OPCODE_BA_SETMODE + idBase;

    if (this->m_to_cmdIn[0].isConnected()) {
      this->m_to_cmdIn[0].invoke(
          _opcode,
          cmdSeq,
          buff
      );
    }
    else {
      printf("Test Command Output port not connected!\n");
    }

  }

  
  void BufferAccumulatorTesterBase ::
    sendRawCmd(FwOpcodeType opcode, U32 cmdSeq, Fw::CmdArgBuffer& args) {
       
    const U32 idBase = this->getIdBase();   
    FwOpcodeType _opcode = opcode + idBase;
    if (this->m_to_cmdIn[0].isConnected()) {
      this->m_to_cmdIn[0].invoke(
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

  void BufferAccumulatorTesterBase ::
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

  void BufferAccumulatorTesterBase ::
    setTestTime(const Fw::Time& time)
  {
    this->m_testTime = time;
  }

  // ----------------------------------------------------------------------
  // Telemetry dispatch
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
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

      case BufferAccumulatorComponentBase::CHANNELID_BUFFERACCUMULATOR_NUMQUEUEDBUFFERS:
      {
        U32 arg;
        const Fw::SerializeStatus _status = val.deserialize(arg);
        if (_status != Fw::FW_SERIALIZE_OK) {
          printf("Error deserializing BufferAccumulator_NumQueuedBuffers: %d\n", _status);
          return;
        }
        this->tlmInput_BufferAccumulator_NumQueuedBuffers(timeTag, arg);
        break;
      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void BufferAccumulatorTesterBase ::
    clearTlm(void)
  {
    this->tlmSize = 0;
    this->tlmHistory_BufferAccumulator_NumQueuedBuffers->clear();
  }

  // ---------------------------------------------------------------------- 
  // Channel: BufferAccumulator_NumQueuedBuffers
  // ---------------------------------------------------------------------- 

  void BufferAccumulatorTesterBase ::
    tlmInput_BufferAccumulator_NumQueuedBuffers(
        const Fw::Time& timeTag,
        const U32& val
    )
  {
    TlmEntry_BufferAccumulator_NumQueuedBuffers e = { timeTag, val };
    this->tlmHistory_BufferAccumulator_NumQueuedBuffers->push_back(e);
    ++this->tlmSize;
  }

  // ----------------------------------------------------------------------
  // Event dispatch
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
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

      case BufferAccumulatorComponentBase::EVENTID_BA_BUFFERACCEPTED: 
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
        this->logIn_ACTIVITY_HI_BA_BufferAccepted();

        break;

      }

      case BufferAccumulatorComponentBase::EVENTID_BA_QUEUEFULL: 
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
        this->logIn_WARNING_HI_BA_QueueFull();

        break;

      }

      default: {
        FW_ASSERT(0, id);
        break;
      }

    }

  }

  void BufferAccumulatorTesterBase ::
    clearEvents(void)
  {
    this->eventsSize = 0;
    this->eventsSize_BA_BufferAccepted = 0;
    this->eventsSize_BA_QueueFull = 0;
  }

#if FW_ENABLE_TEXT_LOGGING

  // ----------------------------------------------------------------------
  // Text events 
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
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

  void BufferAccumulatorTesterBase ::
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

  void BufferAccumulatorTesterBase ::
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
  // Event: BA_BufferAccepted 
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
    logIn_ACTIVITY_HI_BA_BufferAccepted(
        void
    )
  {
    ++this->eventsSize_BA_BufferAccepted;
    ++this->eventsSize;
  }

  // ----------------------------------------------------------------------
  // Event: BA_QueueFull 
  // ----------------------------------------------------------------------

  void BufferAccumulatorTesterBase ::
    logIn_WARNING_HI_BA_QueueFull(
        void
    )
  {
    ++this->eventsSize_BA_QueueFull;
    ++this->eventsSize;
  }

} // end namespace Svc
