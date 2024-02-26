// ======================================================================
// \title  OnChangeChannel.hpp
// \author Rob Bocchino
// \brief  A model of an on-change channel for testing
//
// \copyright
// Copyright (C) 2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// ======================================================================

#ifndef TestUtils_OnChangeChannel_HPP
#define TestUtils_OnChangeChannel_HPP

#include <cstring>

#include "TestUtils/Option.hpp"
#include "config/FpConfig.hpp"

namespace TestUtils {

//! The status of an on-change telemetry channel
enum class OnChangeStatus { CHANGED, NOT_CHANGED };

//! A model of an on-change telemetry channel
template <typename T>
class OnChangeChannel {
  public:
    //! Constructor
    OnChangeChannel(T value) : value(value) {}
    //! Update the previous value
    OnChangeStatus updatePrev() {
        const auto status = ((!this->prev.hasValue()) || (this->value != this->prev.get()))
                                ? OnChangeStatus::CHANGED
                                : OnChangeStatus::NOT_CHANGED;
        this->prev.set(this->value);
        return status;
    }

  public:
    //! The current value
    T value;

  private:
    //! The previous value
    Option<T> prev;
};

}  // namespace TestUtils

#endif
