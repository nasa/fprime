// ======================================================================
// \title  ActivePhaser.hpp
// \author mstarch
// \brief  cpp file for ActivePhaser test harness implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Tester.hpp"
#include "Common/ActivePhaser/ActivePhaserComponentAc.hpp"

#define INSTANCE 0
#define MAX_HISTORY_SIZE 1000
#define QUEUE_DEPTH 1


namespace Common {

void FauxPhaser:: register_child(TestChild& child) {
    child.runtime = 0;
    children.push_back(child);
    current = children.begin();
    state = FauxPhaser::END_OF_CYCLE;
}

FauxPhaser::State FauxPhaser::run(U32 tick_of_cycle, U32 cycle_length) {
    // End of cycle, reset and the above machine will check
    if (current == children.end() && tick_of_cycle >= cycle_length) {
        current = children.begin();
        current->runtime = 0;
        state = FauxPhaser::END_OF_CYCLE;
    }
    // Current child or end of cycle hasn't been reached yet
    else if (current == children.end() || tick_of_cycle < current->actual_start) {
        state = FauxPhaser::IDLE;
    }
    // Runtime has expired pull the next child out of the list
    else if (current->runtime == current->actual_length && (state == FauxPhaser::RUNNING || state == FauxPhaser::STARTING)) {
        bool finished_on_time = (current->length == current->actual_length);
        current++;
        current->runtime = 0;
        // A child has run long, the machine is forced to idle before starting the next child to avoid a short cycle
        state = finished_on_time ? FauxPhaser::STARTING : FauxPhaser::IDLE;
    }
    // Running the current child
    else if (state == FauxPhaser::RUNNING || state == FauxPhaser::STARTING) {
        state = FauxPhaser::RUNNING;
    }
    // End-of-cycle reset
    else if (state == FauxPhaser::END_OF_CYCLE || tick_of_cycle >= current->actual_start) {
        current->runtime = 0;
        state = FauxPhaser::STARTING;
    }
    else
    {
        EXPECT_FALSE(1);
    }
    current->runtime++;
    return state;
}

TestChild* FauxPhaser ::active() {
    return &*current;
}

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

Tester ::Tester(void) : ActivePhaserGTestBase("Tester", MAX_HISTORY_SIZE), component("ActivePhaser"),
                        active(nullptr), m_start_counter(0), m_on_time(true), m_nominal(true)
{
    this->initComponents();
    this->connectPorts();
}

Tester ::~Tester(void) {}

// ----------------------------------------------------------------------
// Test helpers
// ----------------------------------------------------------------------

void Tester ::create_child(U32 port, U32 length, U32 start, U32 context, U32 lethargy) {
    TestChild child;
    child.port = port;
    child.length = length;
    child.start = start;
    child.actual_start = (start != ActivePhaserComponentImpl::DONT_CARE) ? start : m_start_counter;
    child.context = context;
    child.actual_length = length + lethargy;

    m_start_counter = child.actual_start + child.length;
    children.push_back(child);
    mock.register_child(child);
    component.register_phased(child.port, child.length, child.start, child.context);
}

bool Tester ::new_cycle(U32 cycle_number) {
    Svc::TimerVal nope;
    FauxPhaser::State response = mock.run(m_ticks, m_cycle);
    component.CycleIn_handler(0, nope);

    switch (response) {
        // End of cycle checks and resets ticks
        case FauxPhaser::END_OF_CYCLE:
            check_cycle(cycle_number);
            m_ticks = 0;
            return true; // Cycle done
        // Do dispatch of component
        case FauxPhaser::IDLE:
        case FauxPhaser::STARTING:
            component.doDispatch();
        // Cascade intended, increment ticks as long as cycle is not finished
        case FauxPhaser::RUNNING:
        default:
            m_ticks++;
            break;
    }
    // Shaokai: Why don't we return false here. It response == END_OF_CYCLE,
    // we will never reach here.
    return response == FauxPhaser::END_OF_CYCLE; // This cycle not done
}

void Tester ::check_cycle(U32 outer) {
    std::map<U32, U32> counts;
    // Iterate through the deque and much calls
    for (std::deque<TestChild>::iterator it = children.begin() ; it != children.end(); ++it) {
        TestChild* child = &*it;

        ASSERT_GT(calls[child->port].size(), 0) << "Child attached to port " << child->port <<
            " and actual start " << child->actual_start << " not called in cycle number " << outer;
        CallContext call = calls[child->port].front();
        calls[child->port].pop_front();
        ASSERT_EQ(call.port, it->port) << "Unexpected port on call context";
        if (child->context != ActivePhaserComponentImpl::DONT_CARE) {
            ASSERT_EQ(call.context, outer % (child->context/m_cycle)) <<
                "Subdividing context bad for port " << child->port << " and outer loop " << outer;
        } else {
            ASSERT_EQ(counts[child->port], call.context) << "Sequential context bad for port " << child->port
                << " and child start " << child->actual_start;
            counts[child->port] = counts[child->port] + 1;
        }
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void Tester ::test_nominal_child() {
    // Test parameters
    m_cycle = 100, m_ticks = 0, m_nominal = true;
    // Child configuration
    component.configure(m_cycle);
    create_child(0, 5);

    for (U64 i = 0; i < MAX_CYCLES; i++) {
        while (!this->new_cycle(i));
        ASSERT_EVENTS_MissedDeadline_SIZE(0);
    }
}

void Tester ::test_nominal_children() {
    // Test parameters
    m_cycle = 100, m_ticks = 0, m_nominal = true;
    // Child configuration
    component.configure(m_cycle);
    create_child(0, 5);
    create_child(0, 5);
    create_child(1, 5);

    for (U64 i = 0; i < MAX_CYCLES; i++) {
        while (!this->new_cycle(i));
        ASSERT_EVENTS_MissedDeadline_SIZE(0);
    }
}

void Tester ::test_unruly_children() {
    // Test parameters
    m_cycle = 100, m_ticks = 0, m_nominal = true;
    // Child configuration
    component.configure(m_cycle);
    create_child(0, 5);
    create_child(0, 5, 8);
    create_child(1, 5, ActivePhaserComponentImpl::DONT_CARE, 1000);
    create_child(0, 1, 23);
    create_child(2, 1);
    create_child(5, 30, 25, 2000);

    for (U64 i = 0; i < MAX_CYCLES; i++) {
        while (!this->new_cycle(i));
        ASSERT_EVENTS_MissedDeadline_SIZE(0);
    }
}

void Tester ::test_lethargic_child() {
    // Test parameters
    m_cycle = 100, m_ticks = 0, m_nominal = false;
    U32 throttle = 5;
    // Child configuration
    component.configure(m_cycle);
    // Shaokai: It looks like lethargy is amount of overrun time above the specified length.
    create_child(0, 5, ActivePhaserComponentImpl::DONT_CARE, ActivePhaserComponentImpl::DONT_CARE, 5);

    for (U64 i = 0; i < MAX_CYCLES; i++) {
        while (!this->new_cycle(i));
        // Check as long as throttle
        if (throttle > 0) {
            ASSERT_EVENTS_MissedDeadline_SIZE(1);
            ASSERT_EVENTS_MissedDeadline(0, 0, 0, 5, 5);
            clearEvents();
            throttle--;
        }
        m_ticks = 0;
    }
}

void Tester ::test_lethargic_children(bool adjust_initial, U64 cycles, U32 tick_start) {
    // Test parameters
    m_cycle = 100, m_ticks = 0, m_nominal = false;
    U32 throttle = 5;
    // Child configuration
    component.configure(m_cycle);

    create_child(0, 5, (adjust_initial) ? 2 : ActivePhaserComponentImpl::DONT_CARE,
                 ActivePhaserComponentImpl::DONT_CARE, 5);
    create_child(0, 5, 8, ActivePhaserComponentImpl::DONT_CARE, 1);
    create_child(1, 5, ActivePhaserComponentImpl::DONT_CARE, 1000, 5);
    create_child(0, 1, 23);
    create_child(2, 1, ActivePhaserComponentImpl::DONT_CARE, ActivePhaserComponentImpl::DONT_CARE, 1);
    create_child(5, 30, 25, 2000, 80);

    // Fast-forward time
    if (tick_start != 0xFFFFFFFF) {
        tick_start = tick_start / m_cycle * m_cycle;  // Force to the nearest multiple of m_cycle
        component.m_ticks = tick_start - 1;           // Right before the last start
        component.m_last_cycle_ticks = tick_start - m_cycle;
        component.m_state.current = component.m_state.used;
        component.m_cycle_count -= 1;
    }
    for (U64 i = 0; i < MAX_CYCLES; i++) {
        while (!this->new_cycle(i));
        //this->new_cycle(); // Kick start next cycle for the evrs
        // Check as long as throttle
        if (throttle > 0) {
            ASSERT_EVENTS_MissedDeadline_SIZE(5);
            ASSERT_EVENTS_MissedDeadline(0, 0, (adjust_initial) ? 2 : 0, 5, 5);
            ASSERT_EVENTS_MissedDeadline(1, 0, 8, 5, 1);
            ASSERT_EVENTS_MissedDeadline(2, 1, 13, 5, 5);
            ASSERT_EVENTS_MissedDeadline(3, 2, 24, 1, 1);
            ASSERT_EVENTS_MissedDeadline(4, 5, 25, 30, 80);
            clearEvents();
            throttle -= 5;
        }
    }
}

void Tester ::test_rollover() {
    U32 cycle_start = 4294960000lu;
    this->test_lethargic_children(true, 1000llu, cycle_start);
    ASSERT_LT(component.m_ticks, cycle_start) << "Component rollover failed";
}
// ----------------------------------------------------------------------
// Handlers for typed from ports
// ----------------------------------------------------------------------

void Tester ::from_RateGroupMemberOut_handler(const NATIVE_INT_TYPE portNum, NATIVE_UINT_TYPE context) {
    TestChild* active = mock.active();
    EXPECT_NE(active, nullptr) << "Component run when none should be active";
    if (!active) {
        return;
    }

    TestChild& current = *active;
    // Build a call context and add it to the list of calls
    CallContext call;
    call.context = context;
    call.port = portNum;
    call.tick = m_ticks % m_cycle;
    calls[portNum].push_back(call);

    EXPECT_EQ(current.port, portNum) << "Child attached to invalid port";
    if (m_nominal) {
        EXPECT_EQ(current.actual_start, m_ticks % m_cycle);
    } else {
        EXPECT_LE(current.actual_start, m_ticks % m_cycle);
    }
}

// ----------------------------------------------------------------------
// Helper methods
// ----------------------------------------------------------------------

void Tester ::connectPorts(void) {
    // CycleIn
    this->connect_to_CycleIn(0, this->component.get_CycleIn_InputPort(0));

    // RateGroupMemberOut
    for (NATIVE_INT_TYPE i = 0; i < Common::ActivePhaserComponentBase::NUM_RATEGROUPMEMBEROUT_OUTPUT_PORTS; ++i) {
        this->component.set_RateGroupMemberOut_OutputPort(i, this->get_from_RateGroupMemberOut(i));
    }

    // Time
    this->component.set_Time_OutputPort(0, this->get_from_Time(0));

    // Log
    this->component.set_Log_OutputPort(0, this->get_from_Log(0));

    // LogText
    this->component.set_LogText_OutputPort(0, this->get_from_LogText(0));
}

void Tester ::initComponents(void) {
    this->init();
    this->component.init(QUEUE_DEPTH, INSTANCE);
}

}  // end namespace Common
