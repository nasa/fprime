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
    Option<T>(T value) : state(State::VALUE), value(value) {}
    Option<T>() : state(State::NO_VALUE), value(noValue) {}

  public:
    static Option<T> some(T value) { return Option(value); }
    static constexpr Option<T> none() { return Option(); }

  public:
    bool hasValue() const { return this->state == State::VALUE; }
    void set(T value) {
        this->state = State::VALUE;
        this->value = value;
    }
    void clear() { this->state = State::NO_VALUE; }
    T get() const {
        FW_ASSERT(this->hasValue());
        return this->value;
    }
    T getOrElse(T value) const {
        T result = value;
        if (this->hasValue()) {
            result = this->value;
        }
        return result;
    }

  private:
    State state;
    T value;
};

}  // namespace TestUtils

#endif
