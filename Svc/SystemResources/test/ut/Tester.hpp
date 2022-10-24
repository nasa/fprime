// ======================================================================
// \title  SystemResources/test/ut/Tester.hpp
// \author sfregoso
// \brief  hpp file for SystemResources test harness implementation class
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
#include "Svc/SystemResources/SystemResources.hpp"

namespace Svc {

class Tester : public SystemResourcesGTestBase {
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

    //! Test the telemetry output
    //!
    void test_tlm(bool enabled = true);

    //! Test the telemetry enable/disable
    //!
    void test_disable_enable();

    //! Test version EVR
    //!
    void test_version_evr();

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
    SystemResources component;
};

}  // end namespace Svc

#endif
