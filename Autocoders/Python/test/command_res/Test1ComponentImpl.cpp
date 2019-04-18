// ======================================================================
// \title  Test1Impl.cpp
// \author tcanham
// \brief  cpp file for Test1 component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <Autocoders/Python/test/command_res/Test1ComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Cmd {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  Test1ComponentImpl ::
#if FW_OBJECT_NAMES == 1
    Test1ComponentImpl(
        const char *const compName
    ) :
      Test1ComponentBase(compName)
#else
    Test1Impl(void)
#endif
  {

  }

  void Test1ComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    Test1ComponentBase::init(queueDepth, instance);
  }

  Test1ComponentImpl ::
    ~Test1ComponentImpl(void)
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void Test1ComponentImpl ::
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

  void Test1ComponentImpl ::
    TEST_CMD_1_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        I32 arg1,
        F32 arg2,
        U8 arg3
    )
  {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

  void Test1ComponentImpl ::
    TEST_CMD_2_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq,
        U32 arg1,
        U16 arg2,
        U8 arg3
    )
  {
      this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace Cmd
