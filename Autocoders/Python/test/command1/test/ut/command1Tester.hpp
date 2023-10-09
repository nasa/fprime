// ======================================================================
// \title  Test1/test/ut/Tester.hpp
// \author tim
// \brief  hpp file for Test1 test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "command1GTestBase.hpp"
#include "Autocoders/Python/test/command1/Test1ComponentImpl.hpp"

namespace Cmd {

  class command1Tester :
    public Test1GTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object command1Tester
      //!
      command1Tester();

      //! Destroy object command1Tester
      //!
      ~command1Tester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! To do
      //!
      void toDo();

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
      Test1ComponentImpl component;

  };

} // end namespace Cmd

#endif
