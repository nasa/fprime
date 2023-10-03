#ifndef TESTER_HPP
#define TESTER_HPP

#include "testgenGTestBase.hpp"
#include "Autocoders/Python/test/testgen/MathSenderComponentImpl.hpp"

namespace Ref {

  class testgenTester :
    public MathSenderGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object testgenTester
      //!
      testgenTester();

      //! Destroy object testgenTester
      //!
      ~testgenTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!
		//! Test operation command
		//!
		void testAddCommand();
		void testSubCommand();
		void testMultCommand();
		void testDivCommand();
      void toDo();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_mathOut
      //!
      void from_mathOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          F32 val1,
          F32 val2,
          MathOperation operation /*!< operation argument*/
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

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      MathSenderComponentImpl component;

  };

} // end namespace Ref

#endif
