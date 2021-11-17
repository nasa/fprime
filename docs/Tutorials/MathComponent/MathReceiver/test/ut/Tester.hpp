// ======================================================================
// \title  MathReceiver/test/ut/Tester.hpp
// \author tcanham, bocchino
// \brief  hpp file for MathReceiver test harness implementation class
//
// \copyright
// Copyright 2009-2021, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Ref/MathReceiver/MathReceiver.hpp"

namespace Ref {

  class Tester :
    public MathReceiverGTestBase
  {

    private:

      // ----------------------------------------------------------------------
      // Types
      // ----------------------------------------------------------------------

      enum class ThrottleState {
        THROTTLED,
        NOT_THROTTLED
      };

    public:

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

      //! Construct object Tester
      //!
      Tester();

      //! Destroy object Tester
      //!
      ~Tester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!
      void testAdd();
      void testSub();
      void testMul();
      void testDiv();
      void testThrottle();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_mathResultOut
      //!
      void from_mathResultOut_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          F32 result //!< the result of the operation
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Pick an F32 value
      static F32 pickF32Value();

      //! Set the factor parameter
      void setFactor(
          F32 factor, //!< The parameter value
          ThrottleState throttleState //!< The throttle state
      );

      //! Compute a result
      F32 computeResult(
          F32 val1,
          MathOp op,
          F32 val2,
          F32 factor
      );

      //! Do a math operation
      //! Factor parameter must be set
      void doMathOp(
          MathOp op,
          F32 factor
      );

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      MathReceiver component;

  };

} // end namespace Ref

#endif
