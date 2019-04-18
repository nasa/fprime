// ======================================================================
// \title  TelemTester/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for TelemTester component test harness base class
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

namespace Tlm {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TelemTesterTesterBase ::
    TelemTesterTesterBase(
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

  TelemTesterTesterBase ::
    ~TelemTesterTesterBase(void)
  {
  }

  void TelemTesterTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Initialize output port tlmRecvPort

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_tlmRecvPort();
        ++_port
    ) {
      this->m_to_tlmRecvPort[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_tlmRecvPort[%d]",
          this->m_objName,
          _port
      );
      this->m_to_tlmRecvPort[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TelemTesterTesterBase ::
    getNum_to_tlmRecvPort(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_tlmRecvPort);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void TelemTesterTesterBase ::
    connect_to_tlmRecvPort(
        const NATIVE_INT_TYPE portNum,
        Fw::InputTlmPort *const tlmRecvPort
    )
  {
    FW_ASSERT(portNum < this->getNum_to_tlmRecvPort(),static_cast<AssertArg>(portNum));
    this->m_to_tlmRecvPort[portNum].addCallPort(tlmRecvPort);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void TelemTesterTesterBase ::
    invoke_to_tlmRecvPort(
        const NATIVE_INT_TYPE portNum,
        FwChanIdType id,
        Fw::Time &timeTag,
        Fw::TlmBuffer &val
    )
  {
    FW_ASSERT(portNum < this->getNum_to_tlmRecvPort(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_tlmRecvPort(),static_cast<AssertArg>(portNum));
    this->m_to_tlmRecvPort[portNum].invoke(
        id, timeTag, val
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool TelemTesterTesterBase ::
    isConnected_to_tlmRecvPort(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_tlmRecvPort(), static_cast<AssertArg>(portNum));
    return this->m_to_tlmRecvPort[portNum].isConnected();
  }

} // end namespace Tlm
