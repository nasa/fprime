// ======================================================================
// \title  TestCommand/test/ut/Tester.hpp
// \author tim
// \brief  hpp file for TestCommand test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "command2GTestBase.hpp"
#include "Autocoders/Python/test/command2/TestCommandComponentImpl.hpp"

namespace AcTest {

  class command2Tester :
    public TestCommandGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object command2Tester
      //!
      command2Tester();

      //! Destroy object command2Tester
      //!
      ~command2Tester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!
      void msgTest();

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
      TestCommandComponentImpl component;

  };

} // end namespace AcTest

#endif
