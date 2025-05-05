// ======================================================================
// \title  ExampleImpl.hpp
// \author tim
// \brief  hpp file for Example component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Example_HPP
#define Example_HPP

#include "Autocoders/Python/test/port_loopback/ExampleComponentAc.hpp"

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
          const char *const compName //!< The component name
      );

      //! Initialize object Example
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

      //! Destroy object Example
      //!
      ~ExampleComponentImpl();

      void makePortCall(NATIVE_INT_TYPE port);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for exampleInput
      //!
      void exampleInput_handler(
          const FwIndexType portNum, //!< The port number
          I32 arg1, //!< A built-in type argument
          AnotherExample::SomeEnum arg2, //!< The ENUM argument
          const AnotherExample::arg6String& arg6
      );

    };

} // end namespace ExampleComponents

#endif
