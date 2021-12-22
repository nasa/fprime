// ======================================================================
// \title  MathSenderImpl.hpp
// \author tcanham, bocchino
// \brief  hpp file for MathSender component implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Ref_MathSender_HPP
#define Ref_MathSender_HPP

#include "Ref/MathSender/MathSenderComponentAc.hpp"

namespace Ref {

  class MathSender :
    public MathSenderComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object MathSender
      //!
      MathSender(
          const char *const compName //!< The component name
      );

      //! Initialize object MathSender
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, //!< The queue depth
          const NATIVE_INT_TYPE instance = 0 //!< The instance number
      );

      //! Destroy object MathSender
      //!
      ~MathSender();

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for mathResultIn
      //!
      void mathResultIn_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          F32 result //!< the result of the operation
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for DO_MATH command handler
      //! Do a math operation
      void DO_MATH_cmdHandler(
          const FwOpcodeType opCode, //!< The opcode
          const U32 cmdSeq, //!< The command sequence number
          F32 val1, //!< The first value
          MathOp operation, //!< The operation to perform
          F32 val2 //!< The second value
      );

    };

} // end namespace Ref

#endif
