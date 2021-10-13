// ======================================================================
// \title  MathSender/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for MathSender test harness implementation class
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
#include "Ref/MathSender/MathSender.hpp"

namespace Ref {

  class Tester :
    public MathSenderGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

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

      //! Test operation command
      //!
      void testAddCommand();
      void testSubCommand();
      void testMultCommand();
      void testDivCommand();
      void testResult();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_mathOut
      //!
      void from_mathOpOut_handler(
          const NATIVE_INT_TYPE portNum, //!< The port number
          F32 val1, //!< First operand
          const MathOp& op, //!< operation
          F32 val2 //!< Second operand
      );

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

      //! Test an operation
      void testDoMath(
          F32 val1,
          MathOp op,
          F32 val2
      );

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      MathSender component;

  };

} // end namespace Ref

#endif
