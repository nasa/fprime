/**
 * \file: BasicTypes.hpp
 * \author mstarch
 * \brief C++ header for working with basic fprime types
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 */

#ifndef FW_BASIC_TYPES_HPP
#define FW_BASIC_TYPES_HPP

#include <cstddef>
#include <limits>
// Use C linkage for the basic items
extern "C" {
#include "Fw/Types/BasicTypes.h"
}

// FW_NUM_ARRAY_ELEMENTS for C++. These templates have to live outside the
// extern "C" block above (C linkage cannot be applied to templates). The plain
// sizeof macro is wrong for FPP array types and other non-arrays (see #5155),
// so restrict the C++ version to C-style arrays and reject everything else at
// compile time.
namespace Fw {
namespace BasicTypes {

// Element count of a C-style array.
template <typename T, std::size_t N>
constexpr std::size_t fwNumArrayElements(const T (&)[N]) {
    return N;
}

// Catch-all for anything that is not a C-style array. sizeof(T) is used in the
// assert so it only fires when this overload is actually selected, instead of a
// template-error explosion.
template <typename T>
constexpr std::size_t fwNumArrayElements(const T&) {
    static_assert(sizeof(T) == 0, "FW_NUM_ARRAY_ELEMENTS may only be used on a primitive (C-style) array");
    return 0;
}

}  // namespace BasicTypes
}  // namespace Fw

#define FW_NUM_ARRAY_ELEMENTS(a) (::Fw::BasicTypes::fwNumArrayElements(a))  //!< number of elements in a C-style array

// IEEE compliance checks must occur in C++ code
#if !defined(SKIP_FLOAT_IEEE_754_COMPLIANCE) || !SKIP_FLOAT_IEEE_754_COMPLIANCE
static_assert((std::numeric_limits<float>::is_iec559 == true) && (std::numeric_limits<float>::radix == 2) &&
                  (std::numeric_limits<float>::digits == 24) && (std::numeric_limits<float>::max_exponent == 128),
              "The 32-bit floating point type does not conform to the IEEE-754 standard.");
static_assert((std::numeric_limits<double>::is_iec559 == true) && (std::numeric_limits<double>::radix == 2) &&
                  (std::numeric_limits<double>::digits == 53) && (std::numeric_limits<double>::max_exponent == 1024),
              "The 64-bit floating point type does not conform to the IEEE-754 standard.");
#endif
#endif  // End FW_BASIC_TYPES_HPP
