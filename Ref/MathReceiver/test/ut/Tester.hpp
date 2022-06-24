// ======================================================================
// \title  MathReceiver/test/ut/Tester.hpp
// \author nayanatiwari
// \brief  hpp file for MathReceiver test harness implementation class
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Ref/MathReceiver/MathReceiver.hpp"

namespace Ref {

  class Tester :
    public MathReceiverGTestBase
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

      //! Test adding capability
      //!
      void testAdd();

       //! Test subtracting capability
      //!
      void testSub();

       //! Test multiply capability
      //!
      void testMul();

       //! Test dividing capability
      //!
      void testDiv();

      //! Test that throttling occurs when expected and can be cleared
      void testThrottle();
      

	private:
	  // ---------------------------------------------------------------------
	  // Types
	  // ---------------------------------------------------------------------

	  enum class ThrottleState {
		THROTTLED,
		NOT_THROTTLED
	  };

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_mathResultOut
      //!
      void from_mathResultOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          F32 result /*!< 
      The result of the operation
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

      //! Generate random F32
      //!
      F32 pickF32Value();

      //! set the factor
      void setFactor(F32 factor, ThrottleState throttleState);

      //! check result against math component
      F32 computeResult(F32 val1, MathOp op, F32 val2, F32 factor);

      //! test that math operation occurs as expected
      void doMathOp( MathOp op, F32 factor);
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
