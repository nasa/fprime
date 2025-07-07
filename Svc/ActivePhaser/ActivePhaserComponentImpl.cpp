// ======================================================================
// \title  ActiveRateGroupComponentImpl.cpp
// \author mstarch
// \brief  cpp file for ActiveRateGroup component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <Common/ActivePhaser/ActivePhaserComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace Common {

// ----------------------------------------------------------------------
// Construction, initialization, and destruction
// ----------------------------------------------------------------------

ActivePhaserComponentImpl ::ActivePhaserComponentImpl(const char* const compName)
    : ActivePhaserComponentBase(compName),
      m_cycle(0),
      m_ticks(0xFFFFFFFF),
      m_last_start_ticks(0),
      m_last_cycle_ticks(0),
      m_cycle_count(0)
{
    ::memset(&m_state, 0, sizeof(m_state)); // Zero-out the whole configuration table
}

void ActivePhaserComponentImpl ::init(const NATIVE_INT_TYPE queueDepth, const NATIVE_INT_TYPE instance) {
    FW_ASSERT(queueDepth == 1); // Dependent on queue-depth of one to prevent a rush to catch up
    ActivePhaserComponentBase::init(queueDepth, instance);
}

ActivePhaserComponentImpl ::~ActivePhaserComponentImpl(void) {}

void ActivePhaserComponentImpl ::configure(U32 cycle_ticks) {
    FW_ASSERT(cycle_ticks != 0);
    m_cycle = cycle_ticks;
}

void ActivePhaserComponentImpl ::register_phased(U32 port, U32 length, U32 start, U32 context) {
    FW_ASSERT(m_cycle != 0);
    FW_ASSERT(m_state.used < 0xFFFF, m_state.used);
    // Additional checks when there are previous entries
    if (m_state.used > 0) {
        const PhaserStateEntry& previous = m_state.entries[m_state.used - 1];
        FW_ASSERT((previous.start + previous.length - 1) < start, m_state.used, previous.start, start); // Must start after previous entry
        FW_ASSERT(previous.start < start, m_state.used, previous.start, start); // Must start after previous entry
        start = (start == DONT_CARE) ? previous.start + previous.length : start;
    }
    start = (start == DONT_CARE) ? 0 : start; // Shaokai: If start == DONT_CARE, doesn't 0 overwrite the start value set above?
    PhaserStateEntry& entry = m_state.entries[m_state.used];

    // Check assertions on the ports
    FW_ASSERT(port < getNum_RateGroupMemberOut_OutputPorts(), port);
    FW_ASSERT(isConnected_RateGroupMemberOut_OutputPort(port), port);
    FW_ASSERT((start + length) <= m_cycle, start, length, m_cycle);
    FW_ASSERT(context > m_cycle, context, m_cycle);

    entry.port = port;
    entry.start = start;
    entry.length = length;
    entry.context = (context != DONT_CARE) ? context/m_cycle : getNextContext(port);
    entry.contextType = (context != DONT_CARE) ? PhaserContextType::COUNT :  PhaserContextType::SEQUENTIAL;
    entry.started = false;
    m_state.used += 1;
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined typed input ports
// ----------------------------------------------------------------------

void ActivePhaserComponentImpl ::CycleIn_handler(const NATIVE_INT_TYPE portNum, Svc::TimerVal& cycleStart) {
    m_lock.lock();
    m_ticks += 1;
    m_lock.unLock();
    this->Tick_internalInterfaceInvoke();
}

void ActivePhaserComponentImpl ::Tick_internalInterfaceHandler() {
    m_lock.lock();
    U32 full_ticks = m_ticks;
    m_lock.unLock();

    // If the cycle is over, wait for the cycle to end before restarting
    if ((this->timeInCycle(full_ticks) >= m_cycle) && (m_state.current == m_state.used)) {
        m_last_cycle_ticks = full_ticks;
        m_cycle_count++;
        m_state.current = 0;
    }
    // Finish active children and run the next child if it is not a short cycle
    if (!finishChild(full_ticks)) {
        startChild(full_ticks);
    }
}

bool ActivePhaserComponentImpl ::finishChild(U32 full_ticks) {
    // Guard against finishing improperly
    if ((m_state.current >= m_state.used) || (not m_state.entries[m_state.current].started)) {
        return false;
    }
    // Shaokai: Only reachable here when m_state.current < m_state.used && m_state.entries[m_state.current].started
    PhaserStateEntry& entry = m_state.entries[(m_state.current % m_state.used)];
    const U32 execution_time = full_ticks - m_last_start_ticks;
    const U32 expected_time = entry.length;

    // Mark entry as done
    entry.started = false;
    // Shaokai: If they are the same, then there is no need to set it to used.
    // Shaokai: I would check whether they are different, if so, increment current.
    m_state.current = (m_state.current == m_state.used) ? m_state.used : (m_state.current + 1);
    // Check for overrun in timing
    if (execution_time > expected_time) {
        this->log_WARNING_HI_MissedDeadline(entry.port, entry.start, entry.length, (execution_time - expected_time));
        return true;
    }
    return false;
}

void ActivePhaserComponentImpl ::startChild(U32 full_ticks) {
    // Guard against starting improperly
    if ((m_state.current >= m_state.used) || (m_state.entries[m_state.current].start > timeInCycle(full_ticks)) ||
        m_state.entries[m_state.current].started) {
        return;
    }
    PhaserStateEntry& entry = m_state.entries[(m_state.current % m_state.used)];
    U32 context = (entry.contextType == SEQUENTIAL) ? entry.context : m_cycle_count % entry.context;
    entry.started = true;
    m_last_start_ticks = full_ticks;
    this->RateGroupMemberOut_out(entry.port, context);
}

U32 ActivePhaserComponentImpl ::getNextContext(U32 port) {
    U32 context = 0;
    // Shaokai: Do a linear search to see if the entry's port matches the target port,
    // if so, bump the context.
    for (U32 i = 0; i < m_state.used; i++) {
        if (m_state.entries[i].port == port) {
            context = m_state.entries[i].context + 1;
        }
    }
    return context;
}

U32 ActivePhaserComponentImpl ::timeInCycle(U32 full_ticks) {
    return (full_ticks - m_last_cycle_ticks);
}

}  // end namespace Svc
