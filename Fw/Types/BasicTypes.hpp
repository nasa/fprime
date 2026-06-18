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

// FW_NUM_ARRAY_ELEMENTS for C++. This has to live outside the extern "C" block
// above (C linkage cannot be applied to templates). The plain sizeof macro is
// wrong for FPP array types and other non-array types (see #5155), so the C++
// form only accepts a C-style array.
//
// fwArrayElementCountHelper is declared (never defined) for array types only,
// and returns a reference to char[N]. The call sits inside sizeof, which is an
// unevaluated context, so the argument itself is never evaluated. That keeps the
// result a constant expression and, crucially, safe on member arrays such as
// this->member (a by-reference helper would force evaluation of this and fail in
// constant expressions). Passing anything that is not a C-style array has no
// matching overload, so it is a compile error.
namespace Fw {
namespace BasicTypes {

template <typename T, std::size_t N>
char (&fwArrayElementCountHelper(const T (&)[N]))[N];

}  // namespace BasicTypes
}  // namespace Fw

#define FW_NUM_ARRAY_ELEMENTS(a) \
    (sizeof(::Fw::BasicTypes::fwArrayElementCountHelper(a)))  //!< number of elements in a C-style array

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
