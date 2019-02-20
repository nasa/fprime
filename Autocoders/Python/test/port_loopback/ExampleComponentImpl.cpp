// ====================================================================== 
// \title  ExampleImpl.cpp
// \author tim
// \brief  cpp file for Example component implementation class
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


#include <Autocoders/test/port_loopback/ExampleComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"
#include <stdio.h>

namespace ExampleComponents {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction 
  // ----------------------------------------------------------------------

  ExampleComponentImpl ::
#if FW_OBJECT_NAMES == 1
    ExampleComponentImpl(
        const char *const compName
    ) :
      ExampleComponentBase(compName)
#else
    ExampleImpl(void)
#endif
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
    ~ExampleComponentImpl(void)
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
        AnotherExample::arg6String arg6
    )
  {
    printf("%d %d %s\n",arg1,arg2,arg6.toChar());
  }

  void ExampleComponentImpl::makePortCall(NATIVE_INT_TYPE port) {
      AnotherExample::arg6String arg6 = "foo";
      this->exampleOutput_out(port,10,AnotherExample::MEMBER2,arg6);
  }

} // end namespace ExampleComponents
