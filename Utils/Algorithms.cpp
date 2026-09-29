// ======================================================================
// \title  Algorithms.cpp
// \author sjsreehari
// \brief  cpp file for general-purpose mathematical algorithms
//
// \copyright
// Copyright (C) 2026 California Institute of Technology.
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <Utils/Algorithms.hpp>
#include <Fw/Types/Assert.hpp>
#include <limits>

namespace Utils {
namespace Algorithms {

FwSizeType gcd(FwSizeType a, FwSizeType b) {
    const FwSizeType maxIterations = static_cast<FwSizeType>(std::numeric_limits<FwSizeType>::digits) * 2;
    for (FwSizeType i = 0; (i < maxIterations) && (b != 0); i++) {
        FwSizeType temp = b;
        b = a % b;
        a = temp;
    }
    FW_ASSERT(b == 0);
    return a;
}

FwSizeType lcm(FwSizeType a, FwSizeType b) {
    FW_ASSERT(a != 0);
    FW_ASSERT(b != 0);
    const FwSizeType g = gcd(a, b);
    const FwSizeType a_div_g = a / g;
    // Ensure that lcm will not overflow
    FW_ASSERT((std::numeric_limits<FwSizeType>::max() / b) >= a_div_g,
              static_cast<FwAssertArgType>(a),
              static_cast<FwAssertArgType>(b));
    return a_div_g * b;
}

}  // namespace Algorithms
}  // namespace Utils
