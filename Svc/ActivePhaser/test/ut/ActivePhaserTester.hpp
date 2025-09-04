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

#include <deque>
#include "Svc/ActivePhaser/ActivePhaser.hpp"
#include "Svc/ActivePhaser/ActivePhaserGTestBase.hpp"

namespace Svc {

static const U32 MAX_CHILDREN = 1000;
static const U64 MAX_CYCLES = 100llu;

struct CallContext {
    FwIndexType port;
    U32 context;
    U32 tick;
};

struct TestChild {
    FwIndexType port;
    U32 length;         // Execution time bound for a child task
    U32 actual_length;  // Actual execution time configured by the test case
    U32 actual_start;
    U32 start = ActivePhaser::DONT_CARE;
    U32 context = ActivePhaser::DONT_CARE;
    U32 runtime;  // A counter tracking ongoing execution time
};

class FauxPhaser {
  public:
    enum State { RUNNING, IDLE, END_OF_CYCLE, STARTING };

    void register_child(TestChild& child);

    void reset();

    State run(U32 ticks_of_cycle, U32 cycle_time);

    TestChild* active();

    // Member variables
  private:
    std::deque<TestChild> children;
    std::deque<TestChild>::iterator current;
    State state;
};

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

    //! \brief wrapper to create child and integrate it into the test harness
    //!
    void create_child(FwIndexType port,
                      U32 length,
                      U32 start = ActivePhaser::DONT_CARE,
                      U32 context = ActivePhaser::DONT_CARE,
                      U32 lethargy = 0);

    //! \brief A test that assumes nominally behaving single child
    //!
    void test_nominal_child(void);

    //! \brief A test that assumes nominally behaving children
    //!
    void test_nominal_children(void);

    //! \brief A test for children that do things in a less-than sequential manner unruly
    //!
    void test_unruly_children(void);

    //! \brief A test that assumes one lethargic child followed by recovery
    //!
    void test_lethargic_child(void);

    //! \brief A test that tests many lethargic children
    //!
    void test_lethargic_children(bool adjust_initial = false, U64 cycles = MAX_CYCLES, U32 tick_start = 0xFFFFFFFF);

    //! \brief A test rollover engine
    //!
    void test_rollover();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_PhaserMemberOut
    //!
    void from_PhaserMemberOut_handler(const FwIndexType portNum, /*!< The port number*/
                                      U32 context                /*!< The call order*/
    );

  private:
    void check_cycle(U64 outer);
    bool new_cycle(U64 cycle_number);
    bool cycle();

    void start_active();
    bool stop_active();

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

    FauxPhaser mock;
    std::deque<TestChild> children;
    std::map<FwIndexType, std::deque<CallContext> > calls;
    TestChild* active;

    U32 m_start_counter;
    U32 m_ticks;
    U32 m_cycle;
    bool m_nominal;
    bool m_on_time;
};

}  // namespace Svc

#endif
