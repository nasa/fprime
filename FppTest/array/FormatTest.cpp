// ======================================================================
// \title  FormatTest.cpp
// \author T. Chieu
// \brief  cpp file for FormatTest class
//
// \copyright
// Copyright (C) 2009-2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FppTest/array/FormatBoolArrayAc.hpp"
#include "FppTest/array/FormatU8ArrayAc.hpp"
#include "FppTest/array/FormatU16DecArrayAc.hpp"
#include "FppTest/array/FormatU32OctArrayAc.hpp"
#include "FppTest/array/FormatU64HexArrayAc.hpp"
#include "FppTest/array/FormatI8ArrayAc.hpp"
#include "FppTest/array/FormatI16DecArrayAc.hpp"
#include "FppTest/array/FormatI32OctArrayAc.hpp"
#include "FppTest/array/FormatI64HexArrayAc.hpp"
#include "FppTest/array/FormatF32eArrayAc.hpp"
#include "FppTest/array/FormatF32fArrayAc.hpp"
#include "FppTest/array/FormatF64gArrayAc.hpp"
#include "FppTest/array/FormatStringArrayAc.hpp"
#include "FppTest/array/FormatCharArrayAc.hpp"
#include "FppTest/utils/Utils.hpp"

#include "gtest/gtest.h"

#include <sstream>
#include <limits>

// Tests FPP format strings
class FormatTest : public ::testing::Test {
protected:
    void SetUp() override {
        buf2 << "[ ";
    }

    std::stringstream buf1, buf2;
};

TEST_F(FormatTest, Bool) {
    bool testVals[FormatBool::SIZE] = {true, true, false};
    FormatBool a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatBool::SIZE; i++) {
        buf2 << "a " << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, U8) {
    U8 testVals[FormatU8::SIZE] = {0, 100, std::numeric_limits<U8>::max()};
    FormatU8 a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatU8::SIZE; i++) {
        buf2 << "a " << (U16) testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, U16Dec) {
    U16 testVals[FormatU16Dec::SIZE] = {0, 100, std::numeric_limits<U16>::max()};
    FormatU16Dec a(testVals);
    
    buf1 << a;
    for (U32 i = 0; i < FormatU16Dec::SIZE; i++) {
        buf2 << "a " << std::dec << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, U32Oct) {
    U32 testVals[FormatU32Oct::SIZE] = {0, 100, std::numeric_limits<U32>::max()};
    FormatU32Oct a(testVals);
    
    buf1 << a;
    for (U32 i = 0; i < FormatU32Oct::SIZE; i++) {
        buf2 << "a " << std::oct << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, U64Hex) {
    U64 testVals[FormatU64Hex::SIZE] = 
        {0, 100, std::numeric_limits<U64>::max()};
    FormatU64Hex a(testVals);
    
    buf1 << a;
    for (U32 i = 0; i < FormatU64Hex::SIZE; i++) {
        buf2 << "a " << std::hex << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, I8) {
    I8 testVals[FormatI8::SIZE] = 
        {std::numeric_limits<I8>::min(), 0, std::numeric_limits<I8>::max()};
    FormatI8 a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatI8::SIZE; i++) {
        buf2 << "a " << (I16) testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, I16Dec) {
    I16 testVals[FormatI16Dec::SIZE] = 
        {std::numeric_limits<I16>::min(), 0, std::numeric_limits<I16>::max()};
    FormatI16Dec a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatI16Dec::SIZE; i++) {
        buf2 << "a " << std::dec << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, I32Oct) {
    I32 testVals[FormatI32Oct::SIZE] = 
        {std::numeric_limits<I32>::min(), 0, std::numeric_limits<I32>::max()};
    FormatI32Oct a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatI32Oct::SIZE; i++) {
        buf2 << "a " << std::oct << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
    
}

TEST_F(FormatTest, I64Hex) {
    I64 testVals[FormatI64Hex::SIZE] = 
        {std::numeric_limits<I64>::min(), 0, std::numeric_limits<I64>::max()};
    FormatI64Hex a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatI64Hex::SIZE; i++) {
        buf2 << "a " << std::hex << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, F32E) {
    F32 testVals[FormatF32e::SIZE] = 
        {std::numeric_limits<F32>::min(), 0.0, std::numeric_limits<F32>::max()};
    FormatF32e a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatF32e::SIZE; i++) {
        buf2 << "a " << std::setprecision(1) << std::scientific << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, F32F) {
    F32 testVals[FormatF32f::SIZE] = 
        {std::numeric_limits<F32>::min(), 0.0, std::numeric_limits<F32>::max()};
    FormatF32f a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatF32f::SIZE; i++) {
        buf2 << "a " << std::setprecision(2) << std::fixed << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, F64G) {
    F64 testVals[FormatF64g::SIZE] = 
        {std::numeric_limits<F64>::min(), 0.0, std::numeric_limits<F64>::max()};
    FormatF64g a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatF64g::SIZE; i++) {
        buf2 << "a " << std::setprecision(3) << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, String) {
    FormatString::StringSize80 testVals[FormatString::SIZE];
    char buf[80];
    for (U32 i = 0; i < FormatString::SIZE; i++) {
        FppTest::Utils::setString(buf, sizeof(buf));
        testVals[i] = buf;   
    }

    FormatString a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatString::SIZE; i++) {
        buf2 << "% " << testVals[i].toChar() << " ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}

TEST_F(FormatTest, Char) {
    U8 testVals[FormatChar::SIZE] = 
        {FppTest::Utils::getU8(), FppTest::Utils::getU8(), FppTest::Utils::getU8()};
    FormatChar a(testVals);

    buf1 << a;
    for (U32 i = 0; i < FormatChar::SIZE; i++) {
        buf2 << "a " << testVals[i] << " b ";
    }
    buf2 << "]";

    ASSERT_STREQ(
        buf1.str().c_str(),
        buf2.str().c_str()
    );
}
