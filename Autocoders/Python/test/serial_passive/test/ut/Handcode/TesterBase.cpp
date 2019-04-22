// ======================================================================
// \title  TestSerial/test/ut/TesterBase.cpp
// \author Auto-generated
// \brief  cpp file for TestSerial component test harness base class
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

namespace TestComponents {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestSerialTesterBase ::
    TestSerialTesterBase(
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
    // Clear history
    this->clearHistory();
  }

  TestSerialTesterBase ::
    ~TestSerialTesterBase(void)
  {
  }

  void TestSerialTesterBase ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {

    // Initialize base class

		Fw::PassiveComponentBase::init(instance);

    // Attach input port SerialOut

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_from_SerialOut();
        ++_port
    ) {

      this->m_from_SerialOut[_port].init();
      this->m_from_SerialOut[_port].addCallComp(
          this,
          from_SerialOut_static
      );
      this->m_from_SerialOut[_port].setPortNum(_port);

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      (void) snprintf(
          _portName,
          sizeof(_portName),
          "%s_from_SerialOut[%d]",
          this->m_objName,
          _port
      );
      this->m_from_SerialOut[_port].setObjName(_portName);
#endif

    }

    // Initialize output port SerialInSync

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_SerialInSync();
        ++_port
    ) {
      this->m_to_SerialInSync[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_SerialInSync[%d]",
          this->m_objName,
          _port
      );
      this->m_to_SerialInSync[_port].setObjName(_portName);
#endif

    }

    // Initialize output port SerialInGuarded

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_SerialInGuarded();
        ++_port
    ) {
      this->m_to_SerialInGuarded[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_SerialInGuarded[%d]",
          this->m_objName,
          _port
      );
      this->m_to_SerialInGuarded[_port].setObjName(_portName);
#endif

    }

    // Initialize output port SerialInAsync

    for (
        NATIVE_INT_TYPE _port = 0;
        _port < this->getNum_to_SerialInAsync();
        ++_port
    ) {
      this->m_to_SerialInAsync[_port].init();

#if FW_OBJECT_NAMES == 1
      char _portName[80];
      snprintf(
          _portName,
          sizeof(_portName),
          "%s_to_SerialInAsync[%d]",
          this->m_objName,
          _port
      );
      this->m_to_SerialInAsync[_port].setObjName(_portName);
#endif

    }

  }

  // ----------------------------------------------------------------------
  // Getters for port counts
  // ----------------------------------------------------------------------

  NATIVE_INT_TYPE TestSerialTesterBase ::
    getNum_to_SerialInSync(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_SerialInSync);
  }

  NATIVE_INT_TYPE TestSerialTesterBase ::
    getNum_to_SerialInGuarded(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_SerialInGuarded);
  }

  NATIVE_INT_TYPE TestSerialTesterBase ::
    getNum_to_SerialInAsync(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_to_SerialInAsync);
  }

  NATIVE_INT_TYPE TestSerialTesterBase ::
    getNum_from_SerialOut(void) const
  {
    return (NATIVE_INT_TYPE) FW_NUM_ARRAY_ELEMENTS(this->m_from_SerialOut);
  }

  // ----------------------------------------------------------------------
  // Connectors for to ports
  // ----------------------------------------------------------------------

  void TestSerialTesterBase ::
    connect_to_SerialInSync(
        const NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *const SerialInSync
    )
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInSync(),static_cast<AssertArg>(portNum));
    this->m_to_SerialInSync[portNum].registerSerialPort(SerialInSync);
  }

  void TestSerialTesterBase ::
    connect_to_SerialInGuarded(
        const NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *const SerialInGuarded
    )
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInGuarded(),static_cast<AssertArg>(portNum));
    this->m_to_SerialInGuarded[portNum].registerSerialPort(SerialInGuarded);
  }

  void TestSerialTesterBase ::
    connect_to_SerialInAsync(
        const NATIVE_INT_TYPE portNum,
        Fw::InputSerializePort *const SerialInAsync
    )
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInAsync(),static_cast<AssertArg>(portNum));
    this->m_to_SerialInAsync[portNum].registerSerialPort(SerialInAsync);
  }


  // ----------------------------------------------------------------------
  // Invocation functions for to ports
  // ----------------------------------------------------------------------

  void TestSerialTesterBase ::
    invoke_to_SerialInSync(
      NATIVE_INT_TYPE portNum, //!< The port number
      Fw::SerializeBufferBase& Buffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInSync(),static_cast<AssertArg>(portNum));
    this->m_to_SerialInSync[portNum].invokeSerial(Buffer);
  }

  void TestSerialTesterBase ::
    invoke_to_SerialInGuarded(
      NATIVE_INT_TYPE portNum, //!< The port number
      Fw::SerializeBufferBase& Buffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInGuarded(),static_cast<AssertArg>(portNum));
    this->m_to_SerialInGuarded[portNum].invokeSerial(Buffer);
  }

  void TestSerialTesterBase ::
    invoke_to_SerialInAsync(
      NATIVE_INT_TYPE portNum, //!< The port number
      Fw::SerializeBufferBase& Buffer
    )
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInAsync(),static_cast<AssertArg>(portNum));
    this->m_to_SerialInAsync[portNum].invokeSerial(Buffer);
  }

  // ----------------------------------------------------------------------
  // Connection status for to ports
  // ----------------------------------------------------------------------

  bool TestSerialTesterBase ::
    isConnected_to_SerialInSync(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInSync(), static_cast<AssertArg>(portNum));
    return this->m_to_SerialInSync[portNum].isConnected();
  }

  bool TestSerialTesterBase ::
    isConnected_to_SerialInGuarded(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInGuarded(), static_cast<AssertArg>(portNum));
    return this->m_to_SerialInGuarded[portNum].isConnected();
  }

  bool TestSerialTesterBase ::
    isConnected_to_SerialInAsync(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_to_SerialInAsync(), static_cast<AssertArg>(portNum));
    return this->m_to_SerialInAsync[portNum].isConnected();
  }

  // ----------------------------------------------------------------------
  // Getters for from ports
  // ----------------------------------------------------------------------

  Fw::InputSerializePort *TestSerialTesterBase ::
    get_from_SerialOut(const NATIVE_INT_TYPE portNum)
  {
    FW_ASSERT(portNum < this->getNum_from_SerialOut(),static_cast<AssertArg>(portNum));
    return &this->m_from_SerialOut[portNum];
  }

  // ----------------------------------------------------------------------
  // History
  // ----------------------------------------------------------------------

  void TestSerialTesterBase ::
    clearHistory()
  {
  }

} // end namespace TestComponents
