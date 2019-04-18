// ======================================================================
// \title  TestPort/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for TestPort component test harness base class
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

namespace StressTest {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestPortTesterBase ::
    TestPortTesterBase(
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
    this->fromPortHistory_aport =
      new History<FromPortEntry_aport>(maxHistorySize);
    this->fromPortHistory_aport2 =
      new History<FromPortEntry_aport2>(maxHistorySize);
    // Clear history
    this->clearHistory();
  }

  TestPortTesterBase ::
    ~TestPortTesterBase(void)
  {
  }

  void TestPortTesterBase ::
    init(NATIVE_INT_TYPE instance)
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port aport

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_aport();
        ++_port
    ) {

      this->m_from_aport[_port].init();
      this->m_from_aport[_port].addCallComp(
          this,
          from_aport_static
      );
      this->m_from_aport[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_aport[%d]",
          this->m_objName,
          _port
      );
      this->m_from_aport[_port].setObjName(_portName);
#endif

    }

    // Attach input port aport2

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_aport2();
        ++_port
    ) {

      this->m_from_aport2[_port].init();
      this->m_from_aport2[_port].addCallComp(
          this,
          from_aport2_static
      );
      this->m_from_aport2[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_aport2[%d]",
          this->m_objName,
          _port
      );
      this->m_from_aport2[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TestPortTesterBase ::
    getNum_from_aport(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_aport);
  }

  NATIVE_INT_TYPE TestPortTesterBase ::
    getNum_from_aport2(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_aport2);
  }


  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  Another::InputTestPort *TestPortTesterBase ::
    get_from_aport(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_aport(),static_cast<AssertArg>(portNum));
    return &this->m_from_aport[portNum];
  }

  Somewhere::InputTest2Port *TestPortTesterBase ::
    get_from_aport2(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_aport2(),static_cast<AssertArg>(portNum));
    return &this->m_from_aport2[portNum];
  }

  // ----------------------------------------------------------------------
  // Static functions for from ports
  // ----------------------------------------------------------------------

  void TestPortTesterBase ::
    from_aport_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        I32 arg4,
        F32 arg5,
        U8 arg6
    )
  {
    FW_ASSERT(callComp);
    TestPortTesterBase* _testerBase =
      static_cast<TestPortTesterBase*>(callComp);
    _testerBase->from_aport_handlerBase(
        portNum,
        arg4, arg5, arg6
    );
  }

  void TestPortTesterBase ::
    from_aport2_static(
        Fw::PassiveComponentBase *const callComp,
        const NATIVE_INT_TYPE portNum,
        I32 arg4,
        F32 arg5,
        Ref::Gnc::Quaternion arg6
    )
  {
    FW_ASSERT(callComp);
    TestPortTesterBase* _testerBase =
      static_cast<TestPortTesterBase*>(callComp);
    _testerBase->from_aport2_handlerBase(
        portNum,
        arg4, arg5, arg6
    );
  }

  // ----------------------------------------------------------------------
  // Histories for typed from ports
  // ----------------------------------------------------------------------

  void TestPortTesterBase ::
    clearFromPortHistory(void)
  {
    this->fromPortHistorySize = 0;
    this->fromPortHistory_aport->clear();
    this->fromPortHistory_aport2->clear();
  }

  // ----------------------------------------------------------------------
  // From port: aport
  // ----------------------------------------------------------------------

  void TestPortTesterBase ::
    pushFromPortEntry_aport(
        I32 arg4,
        F32 arg5,
        U8 arg6
    )
  {
    FromPortEntry_aport _e = {
      arg4, arg5, arg6
    };
    this->fromPortHistory_aport->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // From port: aport2
  // ----------------------------------------------------------------------

  void TestPortTesterBase ::
    pushFromPortEntry_aport2(
        I32 arg4,
        F32 arg5,
        Ref::Gnc::Quaternion arg6
    )
  {
    FromPortEntry_aport2 _e = {
      arg4, arg5, arg6
    };
    this->fromPortHistory_aport2->push_back(_e);
    ++this->fromPortHistorySize;
  }

  // ----------------------------------------------------------------------
  // Handler base functions for from ports
  // ----------------------------------------------------------------------

  void TestPortTesterBase ::
    from_aport_handlerBase(
        const NATIVE_INT_TYPE portNum,
        I32 arg4,
        F32 arg5,
        U8 arg6
    )
  {
    FW_ASSERT(portNum < this->getNum_from_aport(),static_cast<AssertArg>(portNum));
    this->from_aport_handler(
        portNum,
        arg4, arg5, arg6
    );
  }

  void TestPortTesterBase ::
    from_aport2_handlerBase(
        const NATIVE_INT_TYPE portNum,
        I32 arg4,
        F32 arg5,
        Ref::Gnc::Quaternion arg6
    )
  {
    FW_ASSERT(portNum < this->getNum_from_aport2(),static_cast<AssertArg>(portNum));
    this->from_aport2_handler(
        portNum,
        arg4, arg5, arg6
    );
  }

  // ----------------------------------------------------------------------
  // History
  // ----------------------------------------------------------------------

  void TestPortTesterBase ::
    clearHistory()
  {
    this->clearFromPortHistory();
  }

} // end namespace StressTest
