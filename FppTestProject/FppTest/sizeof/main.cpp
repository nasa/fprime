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
#include "gtest/gtest.h"

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
    ASSERT_EQ(SizeofStringAlias, 102);
    ASSERT_EQ(SizeofStringDefaultAlias, 258);
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
    ASSERT_EQ(SizeofStruct, 94);
    ASSERT_EQ(SizeofStruct2, TestStruct2::SERIALIZED_SIZE);
    ASSERT_EQ(SizeofStruct2, 338);
}
