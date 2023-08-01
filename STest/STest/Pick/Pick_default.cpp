// ======================================================================
// \title  Pick_default.cpp
// \author bocchino
// \brief  Pick_default implementation
//
// \copyright
// Copyright (C) 2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "STest/Random/Random.hpp"

namespace STest {

  namespace Pick {

    double inUnitInterval() {
      return STest::Random::inUnitInterval();
    }

    U32 startLength(
        const U32 start,
        const U32 length
    ) {
      return STest::Random::startLength(start, length);
    }

    U32 lowerUpper(
        const U32 lower,
        const U32 upper
    ) {
      return STest::Random::lowerUpper(lower, upper);
    }

  }

}
