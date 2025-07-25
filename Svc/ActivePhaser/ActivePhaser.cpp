// ======================================================================
// \title  ActivePhaser.cpp
// \author mstarch
// \brief  cpp file for ActivePhaser component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "Svc/ActivePhaser/ActivePhaser.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

ActivePhaser ::ActivePhaser(const char* const compName)
    : ActivePhaserComponentBase(compName),
      m_cycle(0),
      m_ticks(0xFFFFFFFF),
      m_last_start_ticks(0),
      m_last_cycle_ticks(0),
      m_cycle_count(0) {
    ::memset(&m_state, 0, sizeof(m_state));  // Zero-out the whole configuration table
}

void ActivePhaser ::init(const FwSizeType queueDepth, const FwIndexType instance) {
    FW_ASSERT(queueDepth == 1, static_cast<FwAssertArgType>(
                                   queueDepth));  // Dependent on queue-depth of one to prevent a rush to catch up
    ActivePhaserComponentBase::init(queueDepth, instance);
}

void ActivePhaser ::configure(U32 cycle_ticks) {
    FW_ASSERT(cycle_ticks != 0);
    m_cycle = cycle_ticks;
}

void ActivePhaser ::register_phased(FwIndexType port, U32 length, U32 start, U32 context) {
    FW_ASSERT(m_cycle != 0);
    FW_ASSERT(m_state.used < 0xFFFF, static_cast<FwAssertArgType>(m_state.used));
    // Additional checks when there are previous entries
    if (m_state.used > 0) {
        const PhaserStateEntry& previous = m_state.entries[m_state.used - 1];
        FW_ASSERT((previous.start + previous.length - 1) < start, static_cast<FwAssertArgType>(m_state.used),
                  static_cast<FwAssertArgType>(previous.start),
                  static_cast<FwAssertArgType>(start));  // Must start after previous entry
        FW_ASSERT(previous.start < start, static_cast<FwAssertArgType>(m_state.used),
                  static_cast<FwAssertArgType>(previous.start),
                  static_cast<FwAssertArgType>(start));  // Must start after previous entry
        start = (start == DONT_CARE) ? previous.start + previous.length : start;
    }
    // If start is DONT_CARE and does not inherit from the end of the previous task,
    // which happens when registering the first task, set start to 0.
    start = (start == DONT_CARE) ? 0 : start;
    PhaserStateEntry& entry = m_state.entries[m_state.used];

    // Check assertions on the ports
    FW_ASSERT(port < getNum_RateGroupMemberOut_OutputPorts(), static_cast<FwAssertArgType>(port));
    FW_ASSERT(isConnected_RateGroupMemberOut_OutputPort(port), static_cast<FwAssertArgType>(port));
    FW_ASSERT((start + length) <= m_cycle, static_cast<FwAssertArgType>(start), static_cast<FwAssertArgType>(length),
              static_cast<FwAssertArgType>(m_cycle));
    FW_ASSERT(context > m_cycle, static_cast<FwAssertArgType>(context), static_cast<FwAssertArgType>(m_cycle));

    entry.port = port;
    entry.start = start;
    entry.length = length;
    // By default, context is DONT_CARE, which means the context type is SEQUENTIAL
    // and a port's context value by default increments every time it is registered.
    // If a value is given to context, the context type becomes COUNT, and
    // entry.context represents the ratio between the user-configured context and the phaser cycle.
    // The user-configured context must be greater than the phaser cycle.
    // Example: If context == 2000 and m_cycle == 100, then entry.context == 20 while contextType == COUNT.
    // Shaokai: This is a point of confusion because entry.context and context are
    // very different things, yet they have the same name.
    entry.context = (context != DONT_CARE) ? context / m_cycle : getNextContext(port);
    entry.contextType = (context != DONT_CARE) ? PhaserContextType::COUNT : PhaserContextType::SEQUENTIAL;
    entry.started = false;
    m_state.used += 1;
}

ActivePhaser ::~ActivePhaser(void) {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void ActivePhaser ::CycleIn_handler(FwIndexType portNum, Os::RawTime& cycleStart) {
    m_lock.lock();
    m_ticks += 1;
    m_lock.unLock();
    this->Tick_internalInterfaceInvoke();
}

// ----------------------------------------------------------------------
// Handler implementations for user-defined internal interfaces
// ----------------------------------------------------------------------

void ActivePhaser ::Tick_internalInterfaceHandler() {
    m_lock.lock();
    U32 full_ticks = m_ticks;
    m_lock.unLock();

    // If the cycle is over, wait for the cycle to end before restarting
    if ((this->timeInCycle(full_ticks) >= m_cycle) && (m_state.current == m_state.used)) {
        m_last_cycle_ticks = full_ticks;
        // FIXME: Risk of overflow? If a tick occurs every microsecond, an overflow happens every 71.6 minutes.
        m_cycle_count++;
        m_state.current = 0;  // Back to processing the first task.
    }
    // Finish active children and run the next child if it is not a short cycle
    // Shaokai: Finish active "children" or "child"?
    // FIXME: This is really strange. Usually it is if a child is finished, then start a new child.
    if (!finishChild(full_ticks)) {
        startChild(full_ticks);
    }
}

bool ActivePhaser ::finishChild(U32 full_ticks) {
    // Guard against finishing improperly
    // FIXME: This is confusing. false actually means we will start a child.
    // But what does it mean to start a child when current >= used??
    if ((m_state.current >= m_state.used) || (not m_state.entries[m_state.current].started)) {
        return false;
    }
    // Only reachable here when current has not reached used
    // and the current task was previously marked started.
    // Now the task can be marked as done and the next task
    // can be launched.
    PhaserStateEntry& entry = m_state.entries[(m_state.current % m_state.used)];
    const U32 execution_time = full_ticks - m_last_start_ticks;
    const U32 expected_time = entry.length;

    // Mark entry as done
    entry.started = false;
    // Increment the current task index if it has not reached used, i.e., the max index registered.
    m_state.current = (m_state.current == m_state.used) ? m_state.used : (m_state.current + 1);
    // Check for overrun in timing. If a deadline violation is detected,
    // return true to prevent the next child task from launching.
    // Shaokai: It is unclear what the intended behavior of deadline handling here.
    // Returning true here only stalls scheduling the child for one tick only,
    // because in the next tick, not entries[current].started = true and gets launched.
    if (execution_time > expected_time) {
        this->log_WARNING_HI_MissedDeadline(entry.port, entry.start, entry.length, (execution_time - expected_time));
        return true;
    }
    // If no overrun, proceed with the next child task.
    return false;
}

void ActivePhaser ::startChild(U32 full_ticks) {
    // Guard against starting improperly
    if ((m_state.current >= m_state.used)  // Invalid. Current index surpasses the indices of registered tasks.
        || (m_state.entries[m_state.current].start >
            timeInCycle(full_ticks))                  // Current time has not reached the intended start time.
        || m_state.entries[m_state.current].started)  // The current child task has already started.
    {
        return;
    }
    PhaserStateEntry& entry = m_state.entries[(m_state.current % m_state.used)];
    // If context type is SEQUENTIAL, entry.context stores the number of times a port is called from the beginning of
    // execution. If context type is COUNT, entry.context stores the number of phaser cycles elapsed within a
    // user-specified time window.
    U32 context = (entry.contextType == SEQUENTIAL) ? entry.context : m_cycle_count % entry.context;
    entry.started = true;
    m_last_start_ticks = full_ticks;
    this->RateGroupMemberOut_out(entry.port, context);
}

U32 ActivePhaser ::getNextContext(FwIndexType port) {
    U32 context = 0;
    // Linear search to see if the entry's port matches the target port,
    // if so, increment the context.
    // Unlikely to overflow because this happens during registration.
    for (U32 i = 0; i < m_state.used; i++) {
        if (m_state.entries[i].port == port) {
            context = m_state.entries[i].context + 1;
        }
    }
    return context;
}

U32 ActivePhaser ::timeInCycle(U32 full_ticks) {
    return (full_ticks - m_last_cycle_ticks);
}

}  // namespace Svc
