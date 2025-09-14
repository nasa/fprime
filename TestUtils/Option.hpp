// ======================================================================
// \title  Option.hpp
// \author Rob Bocchino
// \brief  An option type for unit testing
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef TestUtils_Option_HPP
#define TestUtils_Option_HPP

namespace TestUtils {

//! An optional value
template <typename T, T noValue = T()>
class Option {
  private:
    enum class State { VALUE, NO_VALUE };

  public:
    explicit Option(T value) : m_state(State::VALUE), m_value(value) {}
    Option() : m_state(State::NO_VALUE), m_value(noValue) {}

  public:
    static Option<T> some(T value) { return Option(value); }
    static constexpr Option<T> none() { return Option(); }

  public:
    bool hasValue() const { return this->m_state == State::VALUE; }
    void set(T value) {
        this->m_state = State::VALUE;
        this->m_value = value;
    }
    void clear() { this->m_state = State::NO_VALUE; }
    T get() const {
        FW_ASSERT(this->hasValue());
        return this->m_value;
    }
    T getOrElse(T value) const {
        T result = value;
        if (this->hasValue()) {
            result = this->m_value;
        }
        return result;
    }

  private:
    State m_state;
    T m_value;
};

}  // namespace TestUtils

#endif
