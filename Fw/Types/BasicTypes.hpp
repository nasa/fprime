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
#include <limits>
// Use C linkage for the basic items
extern "C" {
#include "BasicTypes.h"
};

#ifndef FW_BASIC_TYPES_HPP
#define FW_BASIC_TYPES_HPP

// IEEE compliance checks must occur in C++ code
#if !defined(SKIP_FLOAT_IEEE_754_COMPLIANCE) || !SKIP_FLOAT_IEEE_754_COMPLIANCE
static_assert((std::numeric_limits<float>::is_iec559 == true) && (std::numeric_limits<float>::radix == 2) &&
                  (std::numeric_limits<float>::digits == 24) && (std::numeric_limits<float>::max_exponent == 128),
              "The 32-bit floating point type does not conform to the IEEE-754 standard.");
#if FW_HAS_F64
static_assert((std::numeric_limits<double>::is_iec559 == true) && (std::numeric_limits<double>::radix == 2) &&
                  (std::numeric_limits<double>::digits == 53) && (std::numeric_limits<double>::max_exponent == 1024),
              "The 64-bit floating point type does not conform to the IEEE-754 standard.");
#endif
#endif
#endif  // End FW_BASIC_TYPES_HPP
