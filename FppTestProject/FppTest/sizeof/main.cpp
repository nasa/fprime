// ----------------------------------------------------------------------
// main.cpp
// ----------------------------------------------------------------------

#include "FppTest/sizeof/EnumAliasAliasAc.hpp"
#include "FppTest/sizeof/FppConstantsAc.hpp"
#include "FppTest/sizeof/StringAliasAliasAc.hpp"
#include "FppTest/sizeof/StringDefaultAliasAliasAc.hpp"
#include "FppTest/sizeof/TestArray1ArrayAc.hpp"
#include "FppTest/sizeof/TestArray2ArrayAc.hpp"
#include "FppTest/sizeof/TestEnum1EnumAc.hpp"
#include "FppTest/sizeof/TestStruct1SerializableAc.hpp"
#include "FppTest/sizeof/TestStruct2SerializableAc.hpp"
#include "FppTest/sizeof/U64AliasAliasAc.hpp"
#include "Fw/FPrimeBasicTypes.hpp"
#include "gtest/gtest.h"

// Serialized size of the length prefix on every string
static constexpr FwSizeType STRING_LENGTH_SIZE = sizeof(FwSizeStoreType);

TEST(SizeofTest, Primitives) {
    ASSERT_EQ(SizeofU8, sizeof(U8));
    ASSERT_EQ(SizeofU16, sizeof(U16));
    ASSERT_EQ(SizeofU32, sizeof(U32));
    ASSERT_EQ(SizeofU64, sizeof(U64));
    ASSERT_EQ(SizeofI8, sizeof(I8));
    ASSERT_EQ(SizeofI16, sizeof(I16));
    ASSERT_EQ(SizeofI32, sizeof(I32));
    ASSERT_EQ(SizeofI64, sizeof(I64));
    ASSERT_EQ(SizeofF32, sizeof(F32));
    ASSERT_EQ(SizeofF64, sizeof(F64));
    ASSERT_EQ(SizeofBool, sizeof(bool));
}

TEST(SizeofTest, AliasType) {
    ASSERT_EQ(SizeofStringAlias, 100 + STRING_LENGTH_SIZE);
    ASSERT_EQ(SizeofStringDefaultAlias, 256 + STRING_LENGTH_SIZE);
    ASSERT_EQ(SizeofU64Alias, sizeof(U64Alias));
    ASSERT_EQ(SizeofEnumAlias, EnumAlias::SERIALIZED_SIZE);
    ASSERT_EQ(SizeofEnumAlias, 2);
}

TEST(SizeofTest, Array) {
    ASSERT_EQ(SizeofArray, TestArray1::SERIALIZED_SIZE);
    ASSERT_EQ(SizeofArray, 24);
    ASSERT_EQ(SizeofArray2, TestArray2::SERIALIZED_SIZE);
    ASSERT_EQ(SizeofArray2, 48);
}

TEST(SizeofTest, Enum) {
    ASSERT_EQ(SizeofEnum, TestEnum1::SERIALIZED_SIZE);
    ASSERT_EQ(SizeofEnum, 2);
}

TEST(SizeofTest, Struct) {
    ASSERT_EQ(SizeofStruct, TestStruct1::SERIALIZED_SIZE);
    // m1: 2 * 24, m2: 8, m3: 2, m4: 3 * (10 + prefix), m5: 3 * prefix
    ASSERT_EQ(SizeofStruct, 88 + 6 * STRING_LENGTH_SIZE);
    ASSERT_EQ(SizeofStruct2, TestStruct2::SERIALIZED_SIZE);
    // m1: 48, m2: 100 + prefix, m3: 2 * SizeofStruct
    ASSERT_EQ(SizeofStruct2, 324 + 13 * STRING_LENGTH_SIZE);
}
