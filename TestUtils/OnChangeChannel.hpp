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

#include <Fw/FPrimeBasicTypes.hpp>
#include <cstring>

#include <Fw/Types/Optional.hpp>

namespace TestUtils {

//! The status of an on-change telemetry channel
enum class OnChangeStatus { CHANGED, NOT_CHANGED };

//! A model of an on-change telemetry channel
template <typename T>
class OnChangeChannel {
  public:
    //! Constructor
    explicit OnChangeChannel(T a_value) : value(a_value) {}
    //! Update the previous value
    OnChangeStatus updatePrev() {
        const auto status = ((!this->prev.has_value()) || (this->value != this->prev.value()))
                                ? OnChangeStatus::CHANGED
                                : OnChangeStatus::NOT_CHANGED;
        this->prev = this->value;
        return status;
    }

  public:
    //! The current value
    T value;

  private:
    //! The previous value
    Fw::Optional<T> prev;
};

}  // namespace TestUtils

#endif
