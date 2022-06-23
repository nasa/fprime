// ======================================================================
// \title  MathSender/test/ut/Tester.hpp
// \author nayanatiwari
// \brief  hpp file for MathSender test harness implementation class
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

      //! To do
      //!

      void toDo();

      //Add random choice
      static F32 pickF32Value();

      //! Test add commmand
      void testAddCommand();

      //! Test subtract commmand
      void testSubCommand();

      //! Test divide commmand
      void testDivCommand();

      //! Test multiply commmand
      void testMulCommand();

      //! Test receipt of a result
      void testResult();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_mathOpOut
      //!
      void from_mathOpOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          F32 val1, /*!< 
      The first operand
      */
          const Ref::MathOp &op, /*!< 
      The operation
      */
          F32 val2 /*!< 
      The second operand
      */
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

      //! Test a DO_MATH command
      void testDoMath(MathOp op);

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
