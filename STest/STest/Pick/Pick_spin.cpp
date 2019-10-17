// ====================================================================== 
// \title  Pick_spin.cpp
// \author AUTO-GENERATED: DO NOT EDIT
// \brief  Pick_spin implementation
//
// \copyright
// Copyright (C) 2019 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "STest/Pick/Pick.hpp"
#include "STest/Pick/Pick_spin.hpp"
#include "STest/Random/Random.hpp"

namespace STest {

  namespace Pick {

    namespace Spin {

      namespace {

        //! The array of random seeds
        U32 seeds[NUM_RANDOM_SEEDS];

        //! The array of random values
        U32 values[NUM_RANDOM_VALUES];

        //! Whether this module is initialized
        bool initialized = false;

      }

      //! Pick a random U32
      U32 pickRandU32(void) {
        assert(initialized);
        const U32 randU32 = STest::Random::lowerUpper(0, 0xFFFFFFFFU);
        return values[randU32 % NUM_RANDOM_VALUES];
      }

      //! Pick a double value in the interval [0, 1]
      double inUnitInterval(void) {
        const U32 randU32 = pickRandU32();
        const F64 ratio = static_cast<F64>(randU32) / 0xFFFFFFFFU;
        return ratio;
      }

      void init(void) {
        for (U32 i = 0; i < NUM_RANDOM_SEEDS; ++i) {
          seeds[i] = STest::Random::lowerUpper(0, 0xFFFFFFFFU);
        }
        for (U32 i = 0; i < NUM_RANDOM_VALUES; ++i) {
          values[i] = STest::Random::lowerUpper(0, 0xFFFFFFFFU);
        }
        initialized = true;
      }

      void initRule(const U32 seedIndex) {
        assert(initialized);
        assert(seedIndex < NUM_RANDOM_SEEDS);
        const U32 seed = seeds[seedIndex];
        STest::Random::SeedValue::set(seed);
      }

    }

    U32 startLength(const U32 start, const U32 length) {
      assert(length > 0);
      const F64 randFloat = Spin::inUnitInterval() * length;
      const U32 offset = static_cast<U32>(randFloat);
      return start + offset;
    }

    U32 lowerUpper(const U32 lower, const U32 upper) {
      assert(lower <= upper);
      const F64 length = static_cast<F64>(upper) - lower + 1;
      const F64 randFloat = Spin::inUnitInterval() * length;
      const U32 offset = static_cast<U32>(randFloat);
      return lower + offset;
    }

  }

}
