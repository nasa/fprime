// ======================================================================
// \title  TestSerialImpl.cpp
// \author tim
// \brief  cpp file for TestSerial component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Autocoders/Python/test/serial_passive/TestSerialImpl.hpp>
#include <FpConfig.hpp>

namespace TestComponents {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestSerialImpl ::
    TestSerialImpl(
        const char *const compName
    ) :
      TestSerialComponentBase(compName)
  {

  }

  void TestSerialImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE msgSize,
        const NATIVE_INT_TYPE instance
    )
  {
    TestSerialComponentBase::init(queueDepth, msgSize, instance);
  }

  TestSerialImpl ::
    ~TestSerialImpl()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined serial input ports
  // ----------------------------------------------------------------------

  void TestSerialImpl ::
    SerialInSync_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
    )
  {
    this->SerialOut_out(0,Buffer);
  }

  void TestSerialImpl ::
    SerialInGuarded_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
    )
  {
      this->SerialOut_out(0,Buffer);
  }

  void TestSerialImpl ::
    SerialInAsync_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
    )
  {
      this->SerialOut_out(0,Buffer);
  }

} // end namespace TestComponents
