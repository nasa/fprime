// ======================================================================
// \title  StringTest.hpp
// \author T. Chieu
// \brief  hpp file for StringTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_STRING_TEST_HPP
#define FPP_TEST_STRING_TEST_HPP

#include "FppTest/utils/Utils.hpp"

#include "Fw/Types/String.hpp"
#include "Fw/Types/StringUtils.hpp"

#include "gtest/gtest.h"

namespace FppTest {

    namespace String {

        // Get the size of a string type
        template <typename StringType>
        U32 getSize() {
            return 80;
        }

    } // namespace String

} // namespace FppTest

// Test a nested string class
template <class StringType>
class StringTest : public ::testing::Test {
protected:
    void SetUp() override {
        size = FppTest::String::getSize<StringType>();

        FppTest::Utils::setString(src, size);

        char fwStrBuf1[Fw::String::STRING_SIZE];
        FppTest::Utils::setString(fwStrBuf1, sizeof(fwStrBuf1));
        fwStr = fwStrBuf1;

        // Truncate fwStr for comparison
        char fwStrBuf2[size];
        Fw::StringUtils::string_copy(fwStrBuf2, fwStr.toChar(), size);
        fwSubstr = fwStrBuf2;
    }

    U32 size;
    char src[StringType::SERIALIZED_SIZE];

    Fw::String fwStr;
    Fw::String fwSubstr;
};

TYPED_TEST_SUITE_P(StringTest);

// Test string capacity and default constructor
TYPED_TEST_P(StringTest, Default) {
    TypeParam str;

    // Capacity
    ASSERT_EQ(str.getCapacity(), this->size);

    // Serialized size
    ASSERT_EQ(
        TypeParam::SERIALIZED_SIZE, 
        this->size + sizeof(FwBuffSizeType)
    );

    // Default constructors
    ASSERT_STREQ(str.toChar(), "");
}

// Test string constructors
TYPED_TEST_P(StringTest, Constructors) {
    // Char array constructor
    TypeParam str1(this->src);
    ASSERT_STREQ(str1.toChar(), this->src);

    // Copy constructor
    TypeParam str2(str1);
    ASSERT_STREQ(str2.toChar(), str1.toChar());

    // Fw::StringBase constructor
    TypeParam str3(this->fwStr);
    ASSERT_STREQ(str3.toChar(), this->fwSubstr.toChar());
}

// Test string assignment operator
TYPED_TEST_P(StringTest, AssignmentOp) {
    TypeParam str1;
    TypeParam str2;
    TypeParam str3;

    // Char array assignment
    str1 = this->src;
    ASSERT_STREQ(str1.toChar(), this->src);

    // Copy assignment
    TypeParam& strRef = str1;
    str1 = strRef;
    ASSERT_EQ(&str1, &strRef);
    
    str2 = str1;
    ASSERT_STREQ(str1.toChar(), str1.toChar());

    // Fw::StringBase assignment
    Fw::StringBase& sbRef = str1;
    str1 = sbRef;
    ASSERT_EQ(&str1, &sbRef);

    str3 = this->fwStr;
    ASSERT_STREQ(str3.toChar(), this->fwSubstr.toChar());
}

// Register all test patterns
REGISTER_TYPED_TEST_SUITE_P(StringTest,
    Default,
    Constructors,
    AssignmentOp
);

#endif
