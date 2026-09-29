// ======================================================================
// \title  Algorithms.hpp
// \author sjsreehari
// \brief  hpp file for general-purpose mathematical algorithms
//
// \copyright
// Copyright (C) 2026 California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef UTILS_ALGORITHMS_HPP
#define UTILS_ALGORITHMS_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Utils {
namespace Algorithms {

//! \brief Calculate the greatest common divisor of two numbers using the Euclidean algorithm
//! \param a first number
//! \param b second number
//! \return greatest common divisor of a and b
FwSizeType gcd(FwSizeType a, FwSizeType b);

//! \brief Calculate the least common multiple of two numbers
//! Asserts that a != 0, b != 0, and that the calculated LCM does not overflow FwSizeType
//! \param a first number
//! \param b second number
//! \return least common multiple of a and b
FwSizeType lcm(FwSizeType a, FwSizeType b);

}  // namespace Algorithms

// Convenience aliases in namespace Utils
using Algorithms::gcd;
using Algorithms::lcm;

}  // namespace Utils

#endif
