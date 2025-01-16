// ======================================================================
//
// \title  Guard.hpp
// \author R. Bocchino
// \brief  Template for test implementation of a state machine guard
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_SmHarness_Guard_HPP
#define FppTest_SmHarness_Guard_HPP

#include <FpConfig.hpp>
#include "FppTest/state_machine/internal/harness/SignalValueHistory.hpp"

namespace FppTest {

namespace SmHarness {

//! Test implementation of a state machine guard with a value argument
template <typename Signal, typename T, FwSizeType size>
class Guard {
  public:
    //! The call history type
    using CallHistory = SignalValueHistory<Signal, T, size>;

    //! Constructor
    Guard() {}

    //! Reset the guard to the initial state
    void reset() {
        this->m_callHistory.clear();
        this->m_returnValue = false;
    }

    //! Clear the call history but keep the return value
    void clearCallHistory() { this->m_callHistory.clear(); }

    //! Call the guard
    bool call(Signal signal,  //!< The signal
              const T& arg    //!< The argument
    ) const {
        // Use const cast to update the history
        const_cast<Guard<Signal, T, size>*>(this)->m_callHistory.push(signal, arg);
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
    CallHistory m_callHistory = {};
};

}  // namespace SmHarness

}  // end namespace FppTest

#endif
