// ======================================================================
// \title  RateGroupDriver/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for RateGroupDriver component test harness base class
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

namespace Svc {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  RateGroupDriverTesterBase ::
    RateGroupDriverTesterBase(
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
    this->fromPortHistory_CycleOut =
      new History<FromPortEntry_CycleOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  RateGroupDriverTesterBase ::
    ~RateGroupDriverTesterBase(void) 
  {
  }

  void RateGroupDriverTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port CycleOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_CycleOut();
        ++_port
    ) {

      this->m_from_CycleOut[_port].init();
      this->m_from_CycleOut[_port].addCallComp(
          this,
          from_CycleOut_static
      );
      this->m_from_CycleOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_CycleOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_CycleOut[_port].setObjName(_portName);
#endif

    }

    // Initialize output port CycleIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_CycleIn();
        ++_port
    ) {
      this->m_to_CycleIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_CycleIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_CycleIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE RateGroupDriverTesterBase ::
    getNum_to_CycleIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_CycleIn);
  }

  NATIVE_INT_TYPE RateGroupDriverTesterBase ::
    getNum_from_CycleOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_CycleOut);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports 
  // ----------------------------------------------------------------------

  void RateGroupDriverTesterBase ::
    connect_to_CycleIn(
        const NATIVE_INT_TYPE portNum,
        Svc::InputCyclePort *const CycleIn
    ) 
  {
    FW_ASSERT(portNum < this->getNum_to_CycleIn(),static_cast<AssertArg>(portNum));
    this->m_to_CycleIn[portNum].addCallPort(CycleIn);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void RateGroupDriverTesterBase ::
    invoke_to_CycleIn(
        const NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    FW_ASSERT(portNum < this->getNum_to_CycleIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_CycleIn(),static_cast<AssertArg>(portNum));
    this->m_to_CycleIn[portNum].invoke(
        cycleStart
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool RateGroupDriverTesterBase ::
    isConnected_to_CycleIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_CycleIn(), static_cast<AssertArg>(portNum));
    return this->m_to_CycleIn[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------
 
  Svc::InputCyclePort *RateGroupDriverTesterBase ::
    get_from_CycleOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_CycleOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_CycleOut[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void RateGroupDriverTesterBase ::
    from_CycleOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    FW_ASSERT(callComp);
    RateGroupDriverTesterBase* _testerBase = 
      static_cast<RateGroupDriverTesterBase*>(callComp);
    _testerBase->from_CycleOut_handlerBase(
        portNum,
        cycleStart
    );
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void RateGroupDriverTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_CycleOut->clear();
  }

  // ---------------------------------------------------------------------- 
  // From port: CycleOut
  // ---------------------------------------------------------------------- 

  void RateGroupDriverTesterBase ::
    pushFromPortEntry_CycleOut(
        Svc::TimerVal &cycleStart
    )
  {
    FromPortEntry_CycleOut _e = {
      cycleStart
    };
    this->fromPortHistory_CycleOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void RateGroupDriverTesterBase ::
    from_CycleOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        Svc::TimerVal &cycleStart
    )
  {
    FW_ASSERT(portNum < this->getNum_from_CycleOut(),static_cast<AssertArg>(portNum));
    this->from_CycleOut_handler(
        portNum,
        cycleStart
    );
  }

  // ----------------------------------------------------------------------
  // History 
  // ----------------------------------------------------------------------

  void RateGroupDriverTesterBase ::
    clearHistory()
  {
    this->clearFromPortHistory();
  }

} // end namespace Svc
