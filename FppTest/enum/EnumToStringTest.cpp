// ======================================================================
// \title  EnumToStringTest.cpp
// \author T. Chieu
// \brief  cpp file for EnumToStringTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/enum/ImplicitEnumAc.hpp"
#include "FppTest/enum/ExplicitEnumAc.hpp"
#include "FppTest/enum/DefaultEnumAc.hpp"
#include "FppTest/enum/IntervalEnumAc.hpp"
#include "FppTest/enum/SerializeTypeU8EnumAc.hpp"
#include "FppTest/enum/SerializeTypeU64EnumAc.hpp"

#include "gtest/gtest.h"

#include <cstring>
#include <sstream>

namespace FppTest {
    
    // Populate an array with enum values
    template <typename EnumType>
    void setEnumValArray(typename EnumType::T (&a)[EnumType::NUM_CONSTANTS+1]) {
        for (U32 i = 0; i < EnumType::NUM_CONSTANTS + 1; i++) {
            a[i] = static_cast<typename EnumType::T>(i);
        }
    }

    template<>
    void setEnumValArray<Explicit>(Explicit::T (&a)[Explicit::NUM_CONSTANTS+1]) {
        a[0] = Explicit::A;
        a[1] = Explicit::B;
        a[2] = Explicit::C;
        a[3] = static_cast<Explicit::T>(11);
    }

    template<>
    void setEnumValArray<Interval>(Interval::T (&a)[Interval::NUM_CONSTANTS+1]) {
        a[0] = Interval::A;
        a[1] = Interval::B;
        a[2] = Interval::C;
        a[3] = Interval::D;
        a[4] = Interval::E;
        a[5] = Interval::F;
        a[6] = Interval::G;
        a[7] = static_cast<Interval::T>(11);
    }

    // Populate an array with strings representing enum values
    template <typename EnumType>
    void setEnumStrArray(std::string (&a)[EnumType::NUM_CONSTANTS+1]) {
        a[0] = "A (0)";
        a[1] = "B (1)";
        a[2] = "C (2)";
        a[3] = "D (3)";
        a[4] = "E (4)";
        a[5] = "[invalid] (5)";
    }

    template<>
    void setEnumStrArray<Explicit>(std::string (&a)[Explicit::NUM_CONSTANTS+1]) {
        a[0] = "A (-1952875139)";
        a[1] = "B (2)";
        a[2] = "C (2000999333)";
        a[3] = "[invalid] (11)";
    }

    template <>
    void setEnumStrArray<Interval>(std::string (&a)[Interval::NUM_CONSTANTS+1]) {
        a[0] = "A (0)";
        a[1] = "B (3)";
        a[2] = "C (4)";
        a[3] = "D (5)";
        a[4] = "E (10)";
        a[5] = "F (100)";
        a[6] = "G (101)";
        a[7] = "[invalid] (11)";
    }

} // namespace FppTest

// Test enum string functions
template <typename EnumType>
class EnumToStringTest : public ::testing::Test {
protected:
    void SetUp() override {
        FppTest::setEnumValArray<EnumType>(vals);
        FppTest::setEnumStrArray<EnumType>(strs);
    };

    EnumType e;
    std::stringstream buf;

    typename EnumType::T vals[EnumType::NUM_CONSTANTS+1];
    std::string strs[EnumType::NUM_CONSTANTS+1];
};

// Specify type parameters for this test suite
using EnumTypes = ::testing::Types<
    Implicit, 
    Explicit, 
    Default,
    Interval, 
    SerializeTypeU8,
    SerializeTypeU64
>;
TYPED_TEST_SUITE(EnumToStringTest, EnumTypes);

// Test enum toString() and ostream operator functions
TYPED_TEST(EnumToStringTest, ToString) {
    for (U32 i = 0; i < TypeParam::NUM_CONSTANTS + 1; i++) {
        this->e = this->vals[i];
        this->buf << this->e;

        ASSERT_STREQ(this->buf.str().c_str(), this->strs[i].c_str());

        this->buf.str("");
    }
}
