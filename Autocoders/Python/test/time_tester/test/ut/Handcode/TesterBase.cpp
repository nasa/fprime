// ======================================================================
// \title  TimeTester/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for TimeTester component test harness base class
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

namespace Time {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TimeTesterTesterBase ::
    TimeTesterTesterBase(
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
  }

  TimeTesterTesterBase ::
    ~TimeTesterTesterBase(void)
  {
  }

  void TimeTesterTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Initialize output port timeGetPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_timeGetPort();
        ++_port
    ) {
      this->m_to_timeGetPort[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_timeGetPort[%d]",
          this->m_objName,
          _port
      );
      this->m_to_timeGetPort[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TimeTesterTesterBase ::
    getNum_to_timeGetPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_timeGetPort);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void TimeTesterTesterBase ::
    connect_to_timeGetPort(
        const NATIVE_INT_TYPE portNum,
        Fw::InputTimePort *const timeGetPort
    )
  {
    FW_ASSERT(portNum < this->getNum_to_timeGetPort(),static_cast<AssertArg>(portNum));
    this->m_to_timeGetPort[portNum].addCallPort(timeGetPort);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void TimeTesterTesterBase ::
    invoke_to_timeGetPort(
        const NATIVE_INT_TYPE portNum,
        Fw::Time &time
    )
  {
    FW_ASSERT(portNum < this->getNum_to_timeGetPort(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_timeGetPort(),static_cast<AssertArg>(portNum));
    this->m_to_timeGetPort[portNum].invoke(
        time
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool TimeTesterTesterBase ::
    isConnected_to_timeGetPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_timeGetPort(), static_cast<AssertArg>(portNum));
    return this->m_to_timeGetPort[portNum].isConnected();
  }

} // end namespace Time
