// ======================================================================
// \title  ParamTester/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for ParamTester component test harness base class
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

  ParamTesterTesterBase ::
    ParamTesterTesterBase(
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

  ParamTesterTesterBase ::
    ~ParamTesterTesterBase(void)
  {
  }

  void ParamTesterTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Initialize output port paramGetPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_paramGetPort();
        ++_port
    ) {
      this->m_to_paramGetPort[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_paramGetPort[%d]",
          this->m_objName,
          _port
      );
      this->m_to_paramGetPort[_port].setObjName(_portName);
#endif

    }

    // Initialize output port paramSetPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_paramSetPort();
        ++_port
    ) {
      this->m_to_paramSetPort[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_paramSetPort[%d]",
          this->m_objName,
          _port
      );
      this->m_to_paramSetPort[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE ParamTesterTesterBase ::
    getNum_to_paramGetPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_paramGetPort);
  }

  NATIVE_INT_TYPE ParamTesterTesterBase ::
    getNum_to_paramSetPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_paramSetPort);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void ParamTesterTesterBase ::
    connect_to_paramGetPort(
        const NATIVE_INT_TYPE portNum,
        Fw::InputPrmGetPort *const paramGetPort
    )
  {
    FW_ASSERT(portNum < this->getNum_to_paramGetPort(),static_cast<AssertArg>(portNum));
    this->m_to_paramGetPort[portNum].addCallPort(paramGetPort);
  }

  void ParamTesterTesterBase ::
    connect_to_paramSetPort(
        const NATIVE_INT_TYPE portNum,
        Fw::InputPrmSetPort *const paramSetPort
    )
  {
    FW_ASSERT(portNum < this->getNum_to_paramSetPort(),static_cast<AssertArg>(portNum));
    this->m_to_paramSetPort[portNum].addCallPort(paramSetPort);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  Fw::ParamValid ParamTesterTesterBase ::
    invoke_to_paramGetPort(
        const NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    FW_ASSERT(portNum < this->getNum_to_paramGetPort(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_paramGetPort(),static_cast<AssertArg>(portNum));
    return this->m_to_paramGetPort[portNum].invoke(
        id, val
    );
  }

  void ParamTesterTesterBase ::
    invoke_to_paramSetPort(
        const NATIVE_INT_TYPE portNum,
        FwPrmIdType id,
        Fw::ParamBuffer &val
    )
  {
    FW_ASSERT(portNum < this->getNum_to_paramSetPort(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_paramSetPort(),static_cast<AssertArg>(portNum));
    this->m_to_paramSetPort[portNum].invoke(
        id, val
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool ParamTesterTesterBase ::
    isConnected_to_paramGetPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_paramGetPort(), static_cast<AssertArg>(portNum));
    return this->m_to_paramGetPort[portNum].isConnected();
  }

  bool ParamTesterTesterBase ::
    isConnected_to_paramSetPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_paramSetPort(), static_cast<AssertArg>(portNum));
    return this->m_to_paramSetPort[portNum].isConnected();
  }

} // end namespace Prm
