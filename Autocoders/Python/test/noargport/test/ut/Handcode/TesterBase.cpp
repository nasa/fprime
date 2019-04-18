// ======================================================================
// \title  Example/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for Example component test harness base class
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

namespace ExampleComponents {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ExampleTesterBase ::
    ExampleTesterBase(
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

  ExampleTesterBase ::
    ~ExampleTesterBase(void)
  {
  }

  void ExampleTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Initialize output port exampleInput

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_exampleInput();
        ++_port
    ) {
      this->m_to_exampleInput[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_exampleInput[%d]",
          this->m_objName,
          _port
      );
      this->m_to_exampleInput[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE ExampleTesterBase ::
    getNum_to_exampleInput(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_exampleInput);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void ExampleTesterBase ::
    connect_to_exampleInput(
        const NATIVE_INT_TYPE portNum,
        APortCollection::InputSomePort *const exampleInput
    )
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    this->m_to_exampleInput[portNum].addCallPort(exampleInput);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  U32 ExampleTesterBase ::
    invoke_to_exampleInput(
        const NATIVE_INT_TYPE portNum
    )
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    return this->m_to_exampleInput[portNum].invoke(

    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool ExampleTesterBase ::
    isConnected_to_exampleInput(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(), static_cast<AssertArg>(portNum));
    return this->m_to_exampleInput[portNum].isConnected();
  }

} // end namespace ExampleComponents
