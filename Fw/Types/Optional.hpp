// ======================================================================
// \title  Optional.hpp
// \author celskeggs
// \brief  hpp file for Optional type
//
// \description
//   A lightweight optional type for C++14 environments where
//   std::optional (C++17) is not available.
//
// \copyright
// Copyright (C) 2025-2026 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Fw_Optional_HPP
#define Fw_Optional_HPP

#include <Fw/Types/Assert.hpp>
#include <type_traits>

namespace Fw {

//! \brief Sentinel type representing the absence of a value in an Optional
struct None_t {
    constexpr explicit None_t() {}
};

//! \brief Global constant representing an absent/empty optional value
constexpr None_t NONE = None_t();

//! \class Optional
//! \brief A type-safe container for an optional value
//!
//! Provides a C++14-compatible alternative to std::optional.
//! Only supports trivially copyable types (no non-trivial constructors,
//! destructors, or copy/move operators).
//!
//! \tparam T The type of the contained value. Must be trivially copyable.
//!
template <typename T>
class Optional {
    static_assert(std::is_trivially_copyable<T>::value, "Fw::Optional only supports trivially copyable types");

    T m_val;
    bool m_engaged;

  public:
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Construct an empty Optional
    constexpr Optional() : m_val(), m_engaged(false) {}

    //! Construct an empty Optional from NONE sentinel
    constexpr Optional(const None_t& a) : Optional() { (void)a; }

    //! Construct an Optional containing a value
    constexpr Optional(const T& t) : m_val(t), m_engaged(true) {}

    //! Copy constructor
    Optional(const Optional& other) : Optional() {
        if (other.has_value()) {
            *this = other.value();
        }
    }

    //! Destructor (trivial — T must be trivially copyable)
    ~Optional() = default;

    // ----------------------------------------------------------------------
    // Observers
    // ----------------------------------------------------------------------

    //! Check whether the Optional contains a value
    //! \return true if a value is present
    bool has_value() const { return m_engaged; }

    //! Access the contained value
    //! \pre has_value() must be true
    //! \return Mutable reference to the contained value
    T& value() {
        FW_ASSERT(m_engaged);
        return m_val;
    }

    //! Access the contained value (const)
    //! \pre has_value() must be true
    //! \return Const reference to the contained value
    const T& value() const {
        FW_ASSERT(m_engaged);
        return m_val;
    }

    //! Access the value or return a default
    //! \return The contained value if present, otherwise the provided default
    T value_or(const T& default_value) const { return m_engaged ? m_val : default_value; }

    // ----------------------------------------------------------------------
    // Modifiers
    // ----------------------------------------------------------------------

    //! Reset the Optional to an empty state
    void reset() { m_engaged = false; }

    // ----------------------------------------------------------------------
    // Assignment operators
    // ----------------------------------------------------------------------

    //! Assign a value into the Optional
    Optional& operator=(const T& t) {
        m_engaged = true;
        m_val = t;
        return *this;
    }

    //! Reset the Optional via NONE assignment
    Optional& operator=(const None_t& a) {
        (void)a;
        reset();
        return *this;
    }

    //! Copy assignment operator
    Optional& operator=(const Optional& other) {
        if (other.has_value()) {
            *this = other.value();
        } else {
            reset();
        }
        return *this;
    }

    // ----------------------------------------------------------------------
    // Comparison operators
    // ----------------------------------------------------------------------

    //! Compare with NONE sentinel
    bool operator==(const None_t& a) const {
        (void)a;
        return !m_engaged;
    }

    //! Compare with NONE sentinel (inequality)
    bool operator!=(const None_t& a) const {
        (void)a;
        return m_engaged;
    }

    //! Equality comparison with another Optional
    bool operator==(const Optional& other) const {
        if (m_engaged != other.m_engaged) {
            return false;
        }
        if (!m_engaged) {
            return true;
        }
        return m_val == other.m_val;
    }

    //! Inequality comparison with another Optional
    bool operator!=(const Optional& other) const { return !(*this == other); }
};

}  // namespace Fw

#endif
