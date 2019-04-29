// ======================================================================
// \title  ExampleComponent/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for ExampleComponent component test harness base class
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

  ExampleComponentTesterBase ::
    ExampleComponentTesterBase(
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
    this->fromPortHistory_exampleOutput =
      new History<FromPortEntry_exampleOutput>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  ExampleComponentTesterBase ::
    ~ExampleComponentTesterBase(void)
  {
  }

  void ExampleComponentTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port exampleOutput

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_exampleOutput();
        ++_port
    ) {

      this->m_from_exampleOutput[_port].init();
      this->m_from_exampleOutput[_port].addCallComp(
          this,
          from_exampleOutput_static
      );
      this->m_from_exampleOutput[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_exampleOutput[%d]",
          this->m_objName,
          _port
      );
      this->m_from_exampleOutput[_port].setObjName(_portName);
#endif

    }

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

  NATIVE_INT_TYPE ExampleComponentTesterBase ::
    getNum_to_exampleInput(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_exampleInput);
  }

  NATIVE_INT_TYPE ExampleComponentTesterBase ::
    getNum_from_exampleOutput(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_exampleOutput);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void ExampleComponentTesterBase ::
    connect_to_exampleInput(
        const NATIVE_INT_TYPE portNum,
        AnotherExample::InputExamplePort *const exampleInput
    )
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    this->m_to_exampleInput[portNum].addCallPort(exampleInput);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void ExampleComponentTesterBase ::
    invoke_to_exampleInput(
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        ANameSpace::UserSerializer arg2
    )
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_exampleInput(),static_cast<AssertArg>(portNum));
    this->m_to_exampleInput[portNum].invoke(
        arg1, arg2
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool ExampleComponentTesterBase ::
    isConnected_to_exampleInput(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_exampleInput(), static_cast<AssertArg>(portNum));
    return this->m_to_exampleInput[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  AnotherExample::InputExamplePort *ExampleComponentTesterBase ::
    get_from_exampleOutput(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_exampleOutput(),static_cast<AssertArg>(portNum));
    return &this->m_from_exampleOutput[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void ExampleComponentTesterBase ::
    from_exampleOutput_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        ANameSpace::UserSerializer arg2
    )
  {
    FW_ASSERT(callComp);
    ExampleComponentTesterBase* _testerBase =
      static_cast<ExampleComponentTesterBase*>(callComp);
    _testerBase->from_exampleOutput_handlerBase(
        portNum,
        arg1, arg2
    );
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void ExampleComponentTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_exampleOutput->clear();
  }

  // ----------------------------------------------------------------------
  // From port: exampleOutput
  // ----------------------------------------------------------------------

  void ExampleComponentTesterBase ::
    pushFromPortEntry_exampleOutput(
        I32 arg1,
        ANameSpace::UserSerializer arg2
    )
  {
    FromPortEntry_exampleOutput _e = {
      arg1, arg2
    };
    this->fromPortHistory_exampleOutput->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void ExampleComponentTesterBase ::
    from_exampleOutput_handlerBase(
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        ANameSpace::UserSerializer arg2
    )
  {
    FW_ASSERT(portNum < this->getNum_from_exampleOutput(),static_cast<AssertArg>(portNum));
    this->from_exampleOutput_handler(
        portNum,
        arg1, arg2
    );
  }

  // ----------------------------------------------------------------------
  // History
  // ----------------------------------------------------------------------

  void ExampleComponentTesterBase ::
    clearHistory()
  {
    this->clearFromPortHistory();
  }

} // end namespace ExampleComponents
