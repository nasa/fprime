// ======================================================================
// \title  PassBy/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for PassBy component test harness base class
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

namespace App {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  PassByTesterBase ::
    PassByTesterBase(
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

  PassByTesterBase ::
    ~PassByTesterBase(void)
  {
  }

  void PassByTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Initialize output port ExtIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_ExtIn();
        ++_port
    ) {
      this->m_to_ExtIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_ExtIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_ExtIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE PassByTesterBase ::
    getNum_to_ExtIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_ExtIn);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void PassByTesterBase ::
    connect_to_ExtIn(
        const NATIVE_INT_TYPE portNum,
        Ports::InputMsg1Port *const ExtIn
    )
  {
    FW_ASSERT(portNum < this->getNum_to_ExtIn(),static_cast<AssertArg>(portNum));
    this->m_to_ExtIn[portNum].addCallPort(ExtIn);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void PassByTesterBase ::
    invoke_to_ExtIn(
        const NATIVE_INT_TYPE portNum,
        U32 arg1,
        U32 *arg2,
        U32 &arg3
    )
  {
    FW_ASSERT(portNum < this->getNum_to_ExtIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_ExtIn(),static_cast<AssertArg>(portNum));
    this->m_to_ExtIn[portNum].invoke(
        arg1, arg2, arg3
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool PassByTesterBase ::
    isConnected_to_ExtIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_ExtIn(), static_cast<AssertArg>(portNum));
    return this->m_to_ExtIn[portNum].isConnected();
  }

} // end namespace App
