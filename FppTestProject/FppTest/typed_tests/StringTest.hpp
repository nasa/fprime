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

// Test a string type
template <class StringType>
class StringTest : public ::testing::Test {
protected:
    void SetUp() override {

        FppTest::Utils::setString(this->src, sizeof this->src);

        char fwStrBuf1[Fw::StringBase::BUFFER_SIZE(Fw::String::STRING_SIZE)];
        FppTest::Utils::setString(fwStrBuf1, sizeof(fwStrBuf1));
        fwStr = fwStrBuf1;

        // Truncate fwStr for comparison
        char fwStrBuf2[bufferSize];
        Fw::StringUtils::string_copy(fwStrBuf2, fwStr.toChar(), static_cast<FwSizeType>(sizeof fwStrBuf2));
        fwSubstr = fwStrBuf2;
    }

    static constexpr FwSizeType size = StringType::STRING_SIZE;
    static constexpr FwSizeType bufferSize = Fw::StringBase::BUFFER_SIZE(size);
    char src[bufferSize];

    Fw::String fwStr;
    Fw::String fwSubstr;
};

TYPED_TEST_SUITE_P(StringTest);

// Test string capacity and default constructor
TYPED_TEST_P(StringTest, Default) {
    TypeParam str;

    // Capacity
    const FwSizeType bufferSizeObject = this->bufferSize;
    ASSERT_EQ(str.getCapacity(), bufferSizeObject);

    // Serialized size
    ASSERT_EQ(
        TypeParam::SERIALIZED_SIZE, 
        Fw::StringBase::STATIC_SERIALIZED_SIZE(this->size)
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
