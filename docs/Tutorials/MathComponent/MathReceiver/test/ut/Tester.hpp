// ======================================================================
// \title  MathReceiver/test/ut/Tester.hpp
// \author tcanham
// \brief  hpp file for MathReceiver test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "GTestBase.hpp"
#include "Ref/MathReceiver/MathReceiverComponentImpl.hpp"

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
      void testAddCommand();
      void testSubCommand();
      void testMultCommand();
      void testDivCommand();
      void testThrottle();

    private:

      // ----------------------------------------------------------------------
      // Handlers for typed from ports
      // ----------------------------------------------------------------------

      //! Handler for from_mathOut
      //!
      void from_mathOut_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          F32 result /*!< the result of the operation*/
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
      MathReceiverComponentImpl component;

  };

} // end namespace Ref

#endif
