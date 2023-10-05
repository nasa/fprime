// ======================================================================
// \title  StaticMemory/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for StaticMemory test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP
#include <STest/Pick/Pick.hpp>
#include "StaticMemoryGTestBase.hpp"
#include "Svc/StaticMemory/StaticMemoryComponentImpl.hpp"

namespace Svc {

  class StaticMemoryTester :
    public StaticMemoryGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object StaticMemoryTester
      //!
      StaticMemoryTester();

      //! Destroy object StaticMemoryTester
      //!
      ~StaticMemoryTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      //! Basic allocation and deallocation
      //!
      void test_allocate();

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
      StaticMemoryComponentImpl component;

  };

} // end namespace Svc

#endif
