#include "FppTest/enum/ImplicitEnumAc.hpp"
#include "FppTest/enum/ExplicitEnumAc.hpp"
#include "FppTest/enum/DefaultEnumAc.hpp"
#include "FppTest/enum/IntervalEnumAc.hpp"
#include "FppTest/enum/SerializeTypeU8EnumAc.hpp"
#include "FppTest/enum/SerializeTypeU64EnumAc.hpp"

#include "FppTest/typed_tests/EnumTest.hpp"

#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

// Instantiate enum tests
using EnumTestImplementations = ::testing::Types<
    Implicit, 
    Explicit, 
    Default,
    Interval, 
    SerializeTypeU8,
    SerializeTypeU64
>;
INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, 
                               EnumTest, 
                               EnumTestImplementations);

// Define default value specializations
template<>
Explicit::T getDefaultValue<Explicit>() {
    return Explicit::A;
}

template<>
Default::T getDefaultValue<Default>() {
    return Default::C;
}

// Define valid value specializations
template<>
Explicit::T getValidValue<Explicit>() {
    U32 val = STest::Pick::startLength(0, Explicit::NUM_CONSTANTS - 1);

    switch (val) {
        case 0: return Explicit::A;
        case 1: return Explicit::B;
        default: return Explicit::C;
    }
}

template<>
Interval::T getValidValue<Interval>() {
    U32 val = STest::Pick::startLength(0, Interval::NUM_CONSTANTS - 1);

    switch (val) {
        case 0: return Interval::A;
        case 1: return Interval::B;
        case 2: return Interval::C;
        case 3: return Interval::D;
        case 4: return Interval::E;
        case 5: return Interval::F;
        default: return Interval::G;
    }
}

// Define invalid value specializations
template <>
Explicit::T getInvalidValue<Explicit>() {
    U32 sign = STest::Pick::lowerUpper(0, 1);

    switch (sign) {
        case 0:
            return static_cast<Explicit::T>(STest::Pick::lowerUpper(
                Explicit::C + 1,
                std::numeric_limits<Explicit::SerialType>::max()
            ));
        default:
            return static_cast<Explicit::T>(STest::Pick::lowerUpper(
                (Explicit::A - 1) * (-1),
                std::numeric_limits<Explicit::SerialType>::max()
            ) * (-1));
    }
}

template<>
Interval::T getInvalidValue<Interval>() {
    return static_cast<Interval::T>(STest::Pick::lowerUpper(
        Interval::G + 1,
        std::numeric_limits<Interval::SerialType>::max()
    ));
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
