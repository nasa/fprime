// ======================================================================
//
// \title  NoArgGuard.hpp
// \author R. Bocchino
// \brief  Class for test implementation of a state machine guard
//         with no argument value
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_SmHarness_NoArgGuard_HPP
#define FppTest_SmHarness_NoArgGuard_HPP

#include "FppTest/state_machine/internal/harness/History.hpp"

namespace FppTest {

namespace SmHarness {

//! Test implementation of a state machine guard with no argument value
template <typename Signal, FwSizeType size>
class NoArgGuard {
  public:
    //! Call history type
    using CallHistory = History<Signal, size>;

    //! Constructor
    NoArgGuard() : m_callHistory() {}

    //! Reset the guard to the initial state
    void reset() {
        this->m_callHistory.clear();
        this->m_returnValue = false;
    }

    //! Clear the call history but keep the return value
    void clearCallHistory() { this->m_callHistory.clear(); }

    //! Call the guard
    bool call(Signal signal  //!< The signal
    ) const {
        // Use const cast to update the history
        const_cast<NoArgGuard<Signal, size>*>(this)->m_callHistory.push(signal);
        return this->m_returnValue;
    }

    //! Set the return value
    void setReturnValue(bool returnValue  //!< The return value
    ) {
        this->m_returnValue = returnValue;
    }

    //! Get the call history
    const CallHistory& getCallHistory() const { return this->m_callHistory; }

  private:
    //! The return value
    bool m_returnValue = false;

    //! The call history
    CallHistory m_callHistory;
};

}  // namespace SmHarness

}  // end namespace FppTest

#endif
