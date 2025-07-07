// ======================================================================
// \title  ActivePhaser/test/ut/Tester.hpp
// \author mstarch
// \brief  hpp file for ActivePhaser test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef TESTER_HPP
#define TESTER_HPP

#include "Common/ActivePhaser/ActivePhaserComponentImpl.hpp"
#include "GTestBase.hpp"

#include <map>
#include <deque>

namespace Common {

static const U32 MAX_CHILDREN = 1000;
static const U64 MAX_CYCLES = 100llu;

struct CallContext {
    U32 port;
    U32 context;
    U32 tick;
};


struct TestChild {
    U32 port;
    U32 length;
    U32 actual_length;
    U32 actual_start;
    U32 start=ActivePhaserComponentImpl::DONT_CARE;
    U32 context=ActivePhaserComponentImpl::DONT_CARE;
    U32 runtime;
};

class FauxPhaser {
  public:
    enum State {
        RUNNING,
        IDLE,
        END_OF_CYCLE,
        STARTING
    };

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



class Tester : public ActivePhaserGTestBase {


    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    //! Construct object Tester
    //!
    Tester(void);

    //! Destroy object Tester
    //!
    ~Tester(void);

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! \breif wrapper to create child and integrate it into the test harness
    //!
    void create_child(U32 port, U32 length, U32 start=ActivePhaserComponentImpl::DONT_CARE,
                      U32 context=ActivePhaserComponentImpl::DONT_CARE,
                      U32 lethargy=0);


    //! \brief A test that assumes nominally behaving single child
    //!
    void test_nominal_child(void);

    //! \brief A test that assumes nominally behaving children
    //!
    void test_nominal_children(void);

    //! \brief A test for children that do things in a less-than sequential manner unrulely
    //!
    void test_unruly_children(void);

    //! \brief A test that assumes one lethargic child followed by recovery
    //!
    void test_lethargic_child(void);

    //! \brief A test that tests many lathargic children
    //!
    void test_lethargic_children(bool adjust_initial=false, U64 cycles=MAX_CYCLES, U32 tick_start=0xFFFFFFFF);

    //! \brief A test rollover engine
    //!
    void test_rollover();

  private:
    // ----------------------------------------------------------------------
    // Handlers for typed from ports
    // ----------------------------------------------------------------------

    //! Handler for from_RateGroupMemberOut
    //!
    void from_RateGroupMemberOut_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                                         NATIVE_UINT_TYPE context       /*!< The call order*/
    );

  private:
    void check_cycle(U32 outer);
    bool new_cycle(U32 cycle_number);
    bool cycle();

    void start_active();
    bool stop_active();

    // ----------------------------------------------------------------------
    // Helper methods
    // ----------------------------------------------------------------------

    //! Connect ports
    //!
    void connectPorts(void);

    //! Initialize components
    //!
    void initComponents(void);

  private:
    // ----------------------------------------------------------------------
    // Variables
    // ----------------------------------------------------------------------

    //! The component under test
    //!
    ActivePhaserComponentImpl component;
    FauxPhaser mock;
    std::deque<TestChild> children;
    std::map<U32, std::deque<CallContext> > calls;
    TestChild* active;

    U32 m_start_counter;
    U32 m_ticks;
    U32 m_cycle;
    bool m_nominal;
    bool m_on_time;
};

}  // end namespace Common

#endif
