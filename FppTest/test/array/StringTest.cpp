#include "FppTest/test/array/StringArrayAc.hpp"
#include "FppTest/test/array/String100ArrayAc.hpp"
#include "FppTest/test/utils/utils.hpp"

#include "Fw/Types/String.hpp"
#include "Fw/Types/StringUtils.hpp"

#include "gtest/gtest.h"

#include <cstring>

class StringTest : public ::testing::Test {
protected:
    void SetUp() override {
        setString(src80, sizeof(src80));
        setString(src100, sizeof(src100));

        char fwStrBuf[Fw::String::STRING_SIZE];
        setString(fwStrBuf, sizeof(fwStrBuf));
        fwStr = fwStrBuf;

        // Truncate fwStr for comparison
        char fwStr80Buf[80];
        char fwStr100Buf[100];
        Fw::StringUtils::string_copy(fwStr80Buf, fwStr.toChar(), 80);
        Fw::StringUtils::string_copy(fwStr100Buf, fwStr.toChar(), 100);
        fwStr80 = fwStr80Buf;
        fwStr100 = fwStr100Buf;
    }

    char src80[80];
    char src100[100];

    Fw::String fwStr;
    Fw::String fwStr80;
    Fw::String fwStr100;
};

TEST_F(StringTest, Default) {
    String::StringSize80 str80;
    String100::StringSize100 str100;

    // Capacity
    ASSERT_EQ(str80.getCapacity(), 80);
    ASSERT_EQ(str100.getCapacity(), 100);

    // Serialized size
    ASSERT_EQ(
        String::StringSize80::SERIALIZED_SIZE, 
        80 + sizeof(FwBuffSizeType)
    );
    ASSERT_EQ(
        String100::StringSize100::SERIALIZED_SIZE, 
        100 + sizeof(FwBuffSizeType)
    );

    // Default constructors
    ASSERT_STREQ(str80.toChar(), "");
    ASSERT_STREQ(str100.toChar(), "");
}

TEST_F(StringTest, Constructors) {
    // Char array constructor
    String::StringSize80 str80_1(src80);
    String100::StringSize100 str100_1(src100);
    ASSERT_STREQ(str80_1.toChar(), src80);
    ASSERT_STREQ(str100_1.toChar(), src100);

    // Copy constructor
    String::StringSize80 str80_2(str80_1);
    String100::StringSize100 str100_2(str100_1);
    ASSERT_STREQ(str80_2.toChar(), str80_1.toChar());
    ASSERT_STREQ(str100_2.toChar(), str100_1.toChar());

    // Fw::StringBase constructor
    String::StringSize80 str80_3(fwStr);
    String100::StringSize100 str100_3(fwStr);
    ASSERT_STREQ(str80_3.toChar(), fwStr80.toChar());
    ASSERT_STREQ(str100_3.toChar(), fwStr100.toChar());
}

TEST_F(StringTest, AssignmentOp) {
    String::StringSize80 str80_1;
    String::StringSize80 str80_2;
    String::StringSize80 str80_3;
    String100::StringSize100 str100_1;
    String100::StringSize100 str100_2;
    String100::StringSize100 str100_3;

    // Char array assignment
    str80_1 = src80;
    str100_1 = src100;
    ASSERT_STREQ(str80_1.toChar(), src80);
    ASSERT_STREQ(str100_1.toChar(), src100);

    // Copy assignment
    str80_2 = str80_1;
    str100_2 = str100_1;
    ASSERT_STREQ(str80_1.toChar(), str80_1.toChar());
    ASSERT_STREQ(str100_1.toChar(), str100_1.toChar());

    // Fw::StringBase assignment
    str80_3 = fwStr;
    str100_3 = fwStr;
    ASSERT_STREQ(str80_3.toChar(), fwStr80.toChar());
    ASSERT_STREQ(str100_3.toChar(), fwStr100.toChar());
}
