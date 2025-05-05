// ======================================================================
// \title  TestSerialImpl.hpp
// \author tim
// \brief  hpp file for TestSerial component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestSerial_HPP
#define TestSerial_HPP

#include "Autocoders/Python/test/serial_passive/TestComponentAc.hpp"

namespace TestComponents {

  class TestSerialImpl :
    public TestSerialComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestSerial
      //!
      TestSerialImpl(
          const char *const compName //!< The component name
      );

      //! Initialize object TestSerial
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE msgSize, //!< The message size
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

      //! Destroy object TestSerial
      //!
      ~TestSerialImpl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined serial input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for SerialInSync
      //!
      void SerialInSync_handler(
        FwIndexType portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
      );

      //! Handler implementation for SerialInGuarded
      //!
      void SerialInGuarded_handler(
        FwIndexType portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
      );

      //! Handler implementation for SerialInAsync
      //!
      void SerialInAsync_handler(
        FwIndexType portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
      );

    };

} // end namespace TestComponents

#endif
