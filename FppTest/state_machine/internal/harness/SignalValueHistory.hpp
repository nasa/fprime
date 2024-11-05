// ======================================================================
//
// \title  SignalValueHistory.hpp
// \author R. Bocchino
// \brief  Template for a history of calls with signals and values
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FppTest_SmHarness_SignalValueHistory_HPP
#define FppTest_SmHarness_SignalValueHistory_HPP

#include <FpConfig.hpp>
#include <array>

#include "FppTest/state_machine/internal/harness/History.hpp"
#include "Fw/Types/Assert.hpp"

namespace FppTest {

namespace SmHarness {

//! A history with signals and values
template <typename Signal, typename T, FwSizeType size>
class SignalValueHistory {
  public:
    //! The signal history type
    using SignalHistory = History<Signal, size>;

    //! The value history type
    using ValueHistory = History<T, size>;

    //! Constructor
    SignalValueHistory() : m_signals(), m_values() {}

    //! Clear the history
    void clear() {
        this->m_size = 0;
        this->m_signals.clear();
        this->m_values.clear();
    }

    //! Check two histories for equality
    bool operator==(SignalValueHistory& history  //!< The other history
    ) const {
        return (this->m_size == history.m_size) && (this->m_signals = history.m_signals) &&
               (this->m_values = history.m_values);
    }

    //! Push an item on the history
    void push(Signal signal,  //!< The signal
              const T& value  //!< The value
    ) {
        FW_ASSERT(this->m_size < size, static_cast<FwAssertArgType>(this->m_size));
        this->m_signals.push(signal);
        this->m_values.push(value);
        this->m_size++;
    }

    //! Get the history size
    FwSizeType getSize() const { return this->m_size; }

    //! Get the signal history
    const SignalHistory& getSignals() const { return this->m_signals; }

    //! Get the value history
    const ValueHistory& getValues() const { return this->m_values; }

  private:
    //! The size of the history
    FwSizeType m_size = 0;

    //! The signal history
    SignalHistory m_signals = {};

    //! The values in the history
    ValueHistory m_values = {};
};

}  // namespace SmHarness

}  // end namespace FppTest

#endif
