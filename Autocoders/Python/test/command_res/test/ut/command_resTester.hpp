// ======================================================================
// \title  Test1/test/ut/Tester.hpp
// \author tcanham
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

#include "command_resGTestBase.hpp"
#include "Autocoders/Python/test/command_res/Test1ComponentImpl.hpp"

namespace Cmd {

  class command_resTester :
    public Test1GTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object command_resTester
      //!
      command_resTester();

      //! Destroy object command_resTester
      //!
      ~command_resTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Test residual
      //!
      void residualTest();

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
