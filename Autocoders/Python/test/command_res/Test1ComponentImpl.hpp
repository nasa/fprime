// ======================================================================
// \title  Test1Impl.hpp
// \author tcanham
// \brief  hpp file for Test1 component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Test1_HPP
#define Test1_HPP

#include "Autocoders/Python/test/command_res/Test1ComponentAc.hpp"

namespace Cmd {

  class Test1ComponentImpl :
    public Test1ComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Test1
      //!
      Test1ComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object Test1
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Test1
      //!
      ~Test1ComponentImpl();

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
          F32 arg2, /*!< The F32 command argument*/
          U8 arg3 /*!< The U8 command argument*/
      );

      //! Implementation for TEST_CMD_2 command handler
      //! A test command
      void TEST_CMD_2_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          U32 arg1, /*!< The I32 command argument*/
          U16 arg2, /*!< The F32 command argument*/
          U8 arg3 /*!< The U8 command argument*/
      );


    };

} // end namespace Cmd

#endif
