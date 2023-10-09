// ======================================================================
// \title  Example/test/ut/Tester.hpp
// \author joshuaa
// \brief  hpp file for Example test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "noargportGTestBase.hpp"
#include "Autocoders/Python/test/noargport/ExampleComponentImpl.hpp"

namespace ExampleComponents {

  class noargportTester :
    public ExampleGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object noargportTester
      //!
      noargportTester();

      //! Destroy object noargportTester
      //!
      ~noargportTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!
      void testNoArgs();

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
      ExampleComponentImpl component;

  };

} // end namespace ExampleComponents

#endif
