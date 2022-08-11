#include "FppTest/string/StringArrayArrayAc.hpp"
#include "FppTest/string/String100ArrayArrayAc.hpp"
#include "FppTest/string/StringStructSerializableAc.hpp"
#include "FppTest/utils/Utils.hpp"

#include "Fw/Types/String.hpp"
#include "Fw/Types/StringUtils.hpp"

#include "gtest/gtest.h"

// Get the size of a string type
template <typename StringType>
U32 getSize() {
    return 80;
}

template<>
U32 getSize<String100Array::StringSize100>() {
    return 100;
}

template<>
U32 getSize<StringStruct::StringSize50>() {
    return 50;
}

template<>
U32 getSize<StringStruct::StringSize60>() {
    return 60;
}

// Test a nested string class
template <typename StringType>
class StringTest : public ::testing::Test {
protected:
    void SetUp() override {
        size = getSize<StringType>();

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

// Specify type parameters for this test suite
using StringTypes = ::testing::Types<
    StringArray::StringSize80,
    String100Array::StringSize100,
    StringStruct::StringSize80,
    StringStruct::StringSize50,
    StringStruct::StringSize60
>;
TYPED_TEST_SUITE(StringTest, StringTypes);

// Test string capacity and default constructor
TYPED_TEST(StringTest, Default) {
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
TYPED_TEST(StringTest, Constructors) {
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
TYPED_TEST(StringTest, AssignmentOp) {
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
