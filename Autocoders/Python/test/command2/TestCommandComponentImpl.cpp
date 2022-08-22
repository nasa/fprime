// ======================================================================
// \title  TestCommandImpl.cpp
// \author tim
// \brief  cpp file for TestCommand component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Autocoders/Python/test/command2/TestCommandComponentImpl.hpp>
#include <FpConfig.hpp>

namespace AcTest {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  TestCommandComponentImpl ::
    TestCommandComponentImpl(
        const char *const compName
    ) :
      TestCommandComponentBase(compName)
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
    ~TestCommandComponentImpl()
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
