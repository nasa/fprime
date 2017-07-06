// ====================================================================== 
// \title  TestSerialImpl.hpp
// \author tim
// \brief  hpp file for TestSerial component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef TestSerial_HPP
#define TestSerial_HPP

#include "Autocoders/test/serial_passive/TestComponentAc.hpp"

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
#if FW_OBJECT_NAMES == 1
          const char *const compName //!< The component name
#else
          void
#endif
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
      ~TestSerialImpl(void);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined serial input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for SerialInSync
      //!
      void SerialInSync_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
      );

      //! Handler implementation for SerialInGuarded
      //!
      void SerialInGuarded_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
      );

      //! Handler implementation for SerialInAsync
      //!
      void SerialInAsync_handler(
        NATIVE_INT_TYPE portNum, //!< The port number
        Fw::SerializeBufferBase &Buffer //!< The serialization buffer
      );

    };

} // end namespace TestComponents

#endif
