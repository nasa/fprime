// ======================================================================
// \title  ExampleImpl.cpp
// \author tim
// \brief  cpp file for Example component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Autocoders/Python/test/port_loopback/ExampleComponentImpl.hpp>
#include <FpConfig.hpp>
#include <cstdio>

namespace ExampleComponents {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  ExampleComponentImpl ::
    ExampleComponentImpl(
        const char *const compName
    ) :
      ExampleComponentBase(compName)
  {

  }

  void ExampleComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    ExampleComponentBase::init(instance);
  }

  ExampleComponentImpl ::
    ~ExampleComponentImpl()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void ExampleComponentImpl ::
    exampleInput_handler(
        const NATIVE_INT_TYPE portNum,
        I32 arg1,
        AnotherExample::SomeEnum arg2,
        const AnotherExample::arg6String& arg6
    )
  {
    printf("%d %d %s\n",arg1,arg2,arg6.toChar());
  }

  void ExampleComponentImpl::makePortCall(NATIVE_INT_TYPE port) {
      AnotherExample::arg6String arg6 = "foo";
      this->exampleOutput_out(port,10,AnotherExample::MEMBER2,arg6);
  }

} // end namespace ExampleComponents
