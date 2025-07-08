// ======================================================================
// \title  ActivePhaserTester.hpp
// \author mstarch
// \brief  hpp file for ActivePhaser component test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_ActivePhaserTester_HPP
#define Svc_ActivePhaserTester_HPP

#include "Svc/ActivePhaser/ActivePhaser.hpp"
#include "Svc/ActivePhaser/ActivePhaserGTestBase.hpp"

namespace Svc {

class ActivePhaserTester final : public ActivePhaserGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 1;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object ActivePhaserTester
    ActivePhaserTester();

    //! Destroy object ActivePhaserTester
    ~ActivePhaserTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! To do
    void toDo();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    ActivePhaser component;
};

}  // namespace Svc

#endif
