// ======================================================================
// \title  ArrayToStringTest.cpp
// \author T. Chieu
// \brief  cpp file for ArrayToStringTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/array/EnumArrayAc.hpp"
#include "FppTest/array/StringArrayAc.hpp"
#include "FppTest/array/StructArrayAc.hpp"
#include "FppTest/array/Uint32ArrayArrayAc.hpp"

#include "FppTest/typed_tests/ArrayTest.hpp"

#include "gtest/gtest.h"

#include <sstream>

// Test array string functions
template <typename ArrayType>
class ArrayToStringTest : public ::testing::Test {
protected:
    void SetUp() override {
        FppTest::Array::setTestVals<ArrayType>(testVals);
    }

    typename ArrayType::ElementType testVals[ArrayType::SIZE];
};

using ArrayTypes = ::testing::Types<
    Enum,
    String,
    Struct,
    Uint32Array
>;
TYPED_TEST_SUITE(ArrayToStringTest, ArrayTypes);

// Test array toString() and ostream operator functions
TYPED_TEST(ArrayToStringTest, ToString) {
    TypeParam a(this->testVals);
    std::stringstream buf1, buf2;

    buf1 << a;

    buf2 << "[ ";
    for (U32 i = 0; i < TypeParam::SIZE; i++) {
        buf2 << this->testVals[i] << " ";    
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}
