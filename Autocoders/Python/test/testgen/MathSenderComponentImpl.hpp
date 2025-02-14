// ======================================================================
// \title  MathSenderComponentImpl.hpp
// \author jishii
// \brief  hpp file for MathSender component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef MathSender_HPP
#define MathSender_HPP

#include "Autocoders/Python/test/testgen/MathSenderComponentAc.hpp"

namespace Ref {

  class MathSenderComponentImpl :
    public MathSenderComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object MathSender
      //!
      MathSenderComponentImpl(
          const char *const compName /*!< The component name*/
      );

      //! Initialize object MathSender
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object MathSender
      //!
      ~MathSenderComponentImpl();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for mathIn
      //!
      void mathIn_handler(
          const FwIndexType portNum, /*!< The port number*/
          F32 result /*!< the result of the operation*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for MS_DO_MATH command handler
      //! Do a math operation
      void MS_DO_MATH_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq, /*!< The command sequence number*/
          F32 val1, /*!< The first value*/
          F32 val2, /*!< The second value*/
          MathOp operation /*!< The operation to perform*/
      );


    };

} // end namespace Ref

#endif
