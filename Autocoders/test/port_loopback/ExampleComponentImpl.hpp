// ====================================================================== 
// \title  ExampleImpl.hpp
// \author tim
// \brief  hpp file for Example component implementation class
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

#ifndef Example_HPP
#define Example_HPP

#include "Autocoders/test/port_loopback/ExampleComponentAc.hpp"

namespace ExampleComponents {

  class ExampleComponentImpl :
    public ExampleComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Example
      //!
      ExampleComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName //!< The component name
#else
          void
#endif
      );

      //! Initialize object Example
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

      //! Destroy object Example
      //!
      ~ExampleComponentImpl(void);

      void makePortCall(NATIVE_INT_TYPE port);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for exampleInput
      //!
      void exampleInput_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::SomeEnum arg2, //!< The ENUM argument
          AnotherExample::arg6String arg6 
      );

    };

} // end namespace ExampleComponents

#endif
