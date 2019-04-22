// ======================================================================
// \title  TestComponent/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for TestComponent component test harness base class
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

  TestComponentTesterBase ::
    TestComponentTesterBase(
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
    this->fromPortHistory_testOut =
      new History<FromPortEntry_testOut>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  TestComponentTesterBase ::
    ~TestComponentTesterBase(void)
  {
  }

  void TestComponentTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port testOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_testOut();
        ++_port
    ) {

      this->m_from_testOut[_port].init();
      this->m_from_testOut[_port].addCallComp(
          this,
          from_testOut_static
      );
      this->m_from_testOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_testOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_testOut[_port].setObjName(_portName);
#endif

    }

    // Initialize output port testIn

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_testIn();
        ++_port
    ) {
      this->m_to_testIn[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_testIn[%d]",
          this->m_objName,
          _port
      );
      this->m_to_testIn[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TestComponentTesterBase ::
    getNum_to_testIn(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_testIn);
  }

  NATIVE_INT_TYPE TestComponentTesterBase ::
    getNum_from_testOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_testOut);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void TestComponentTesterBase ::
    connect_to_testIn(
        const NATIVE_INT_TYPE portNum,
        AnotherExample::InputTestPort *const testIn
    )
  {
    FW_ASSERT(portNum < this->getNum_to_testIn(),static_cast<AssertArg>(portNum));
    this->m_to_testIn[portNum].addCallPort(testIn);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void TestComponentTesterBase ::
    invoke_to_testIn(
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        AnotherExample::arg2String &arg2,
        AnotherExample::arg3Buffer arg3
    )
  {
    FW_ASSERT(portNum < this->getNum_to_testIn(),static_cast<AssertArg>(portNum));
    FW_ASSERT(portNum < this->getNum_to_testIn(),static_cast<AssertArg>(portNum));
    this->m_to_testIn[portNum].invoke(
        arg1, arg2, arg3
    );
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool TestComponentTesterBase ::
    isConnected_to_testIn(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_testIn(), static_cast<AssertArg>(portNum));
    return this->m_to_testIn[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  AnotherExample::InputTestPort *TestComponentTesterBase ::
    get_from_testOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_testOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_testOut[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void TestComponentTesterBase ::
    from_testOut_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        AnotherExample::arg2String &arg2,
        AnotherExample::arg3Buffer arg3
    )
  {
    FW_ASSERT(callComp);
    TestComponentTesterBase* _testerBase =
      static_cast<TestComponentTesterBase*>(callComp);
    _testerBase->from_testOut_handlerBase(
        portNum,
        arg1, arg2, arg3
    );
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void TestComponentTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_testOut->clear();
  }

  // ----------------------------------------------------------------------
  // From port: testOut
  // ----------------------------------------------------------------------

  void TestComponentTesterBase ::
    pushFromPortEntry_testOut(
        I32 arg1,
        AnotherExample::arg2String &arg2,
        AnotherExample::arg3Buffer arg3
    )
  {
    FromPortEntry_testOut _e = {
      arg1, arg2, arg3
    };
    this->fromPortHistory_testOut->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void TestComponentTesterBase ::
    from_testOut_handlerBase(
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        AnotherExample::arg2String &arg2,
        AnotherExample::arg3Buffer arg3
    )
  {
    FW_ASSERT(portNum < this->getNum_from_testOut(),static_cast<AssertArg>(portNum));
    this->from_testOut_handler(
        portNum,
        arg1, arg2, arg3
    );
  }

  // ----------------------------------------------------------------------
  // History
  // ----------------------------------------------------------------------

  void TestComponentTesterBase ::
    clearHistory()
  {
    this->clearFromPortHistory();
  }

} // end namespace ExampleComponents
