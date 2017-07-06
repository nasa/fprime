// ======================================================================
// \title  TestCommandImpl.cpp
// \author tim
// \brief  cpp file for TestCommand component implementation class
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


#include <Autocoders/test/command2/TestCommandComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace AcTest {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestCommandComponentImpl ::
#if FW_OBJECT_NAMES == 1
    TestCommandComponentImpl(
        const char *const compName
    ) :
      TestCommandComponentBase(compName)
#else
    TestCommandImpl(void)
#endif
  {

  }

  void TestCommandComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    TestCommandComponentBase::init(queueDepth, instance);
  }

  TestCommandComponentImpl ::
    ~TestCommandComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void TestCommandComponentImpl ::
    aport_handler(
        const NATIVE_INT_TYPE portNum,
        I32 arg4,
        F32 arg5,
        U8 arg6
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void TestCommandComponentImpl ::
    TEST_CMD_1_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        I32 arg1,
        SomeEnum arg2
    )
  {
    // TODO
  }

} // end namespace AcTest
