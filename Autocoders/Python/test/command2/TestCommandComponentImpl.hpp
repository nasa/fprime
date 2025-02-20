// ======================================================================
// \title  TestCommandImpl.hpp
// \author tim
// \brief  hpp file for TestCommand component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TestCommand_HPP
#define TestCommand_HPP

#include "Autocoders/Python/test/command2/TestComponentAc.hpp"

namespace AcTest {

  class TestCommandComponentImpl :
    public TestCommandComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object TestCommand
      //!
      TestCommandComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object TestCommand
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object TestCommand
      //!
      ~TestCommandComponentImpl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for aport
      //!
      void aport_handler(
          const FwIndexType portNum, /*!< The port number*/
          I32 arg4, /*!< The first argument*/
          F32 arg5, /*!< The second argument*/
          U8 arg6 /*!< The third argument*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for TEST_CMD_1 command handler
      //! A test command
      void TEST_CMD_1_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          I32 arg1, /*!< The I32 command argument*/
          SomeEnum arg2 /*!< The ENUM argument*/
      );


    };

} // end namespace AcTest

#endif
