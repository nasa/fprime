// ======================================================================
// \title  main.cpp
// \author T. Chieu
// \brief  main cpp file for FPP array tests
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
#include "FppTest/array/String100ArrayAc.hpp"

#include "FppTest/typed_tests/ArrayTest.hpp"
#include "FppTest/typed_tests/StringTest.hpp"
#include "FppTest/utils/Utils.hpp"

#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

// Instantiate array tests
using ArrayTestImplementations = ::testing::Types<
    Enum,
    String,
    Struct,
    Uint32Array
>;
INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, ArrayTest, ArrayTestImplementations);

// Specializations for default values
template <>
void FppTest::Array::setDefaultVals<Enum>(E (&a)[Enum::SIZE]) {
    a[0] = E::A;
    a[1] = E::B;
    a[2] = E::C;
}

// Specialization for test values
template<>
void FppTest::Array::setTestVals<Enum>(E (&a)[Enum::SIZE]) {
    a[0] = static_cast<E::T>(STest::Pick::startLength(
        E::B,
        E::NUM_CONSTANTS - 1
    ));

    for (U32 i = 1; i < Enum::SIZE; i++) {
        a[i] = static_cast<E::T>(STest::Pick::startLength(
            E::A,
            E::NUM_CONSTANTS - 1
        ));
    }
}

template<>
void FppTest::Array::setTestVals<String>
    (::String::StringSize80 (&a)[::String::SIZE]) {
    char buf[80];
    for (U32 i = 0; i < ::String::SIZE; i++) {
        FppTest::Utils::setString(buf, sizeof(buf));
        a[i] = buf;
    }
}

template<>
void FppTest::Array::setTestVals<Struct>(S (&a)[Struct::SIZE]) {
    U32 b[3];
    for (U32 i = 0; i < Struct::SIZE; i++) {
        for (U32 j = 0; j < 3; j++) {
            b[j] = FppTest::Utils::getU32();
        }
        a[i].set(FppTest::Utils::getU32(), b);
    }
}

template<>
void FppTest::Array::setTestVals<Uint32Array>(Uint32 (&a)[Uint32Array::SIZE]) {
    Uint32 b;
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        for (U32 j = 0; j < Uint32::SIZE; j++) {
            b[j] = FppTest::Utils::getU32();
        }
        a[i] = b;
    }
}

// Specializations for multi element constructor
template<>
Enum FppTest::Array::getMultiElementConstructedArray<Enum>
    (E (&a)[Enum::SIZE]) {
    return Enum(a[0], a[1], a[2]);
}

template<>
::String FppTest::Array::getMultiElementConstructedArray<::String>
    (::String::StringSize80 (&a)[::String::SIZE]) {
        return ::String(a[0], a[1], a[2]);
}

template<>
Struct FppTest::Array::getMultiElementConstructedArray<Struct>
    (S (&a)[Struct::SIZE]) {
    return Struct(a[0], a[1], a[2]);
}

template<>
Uint32Array FppTest::Array::getMultiElementConstructedArray<Uint32Array>
    (Uint32 (&a)[Uint32Array::SIZE]) {
    return Uint32Array(a[0], a[1], a[2]);
}

// Specializations for serialized size
template <>
U32 FppTest::Array::getSerializedSize<::String>
    (::String::StringSize80 (&a)[::String::SIZE]) {
    U32 serializedSize = 0;

    for (U32 i = 0; i < ::String::SIZE; i++) {
        serializedSize += a[i].length() + sizeof(FwBuffSizeType);   
    }

    return serializedSize;
}

// Instantiate string tests for arrays
using StringTestImplementations = ::testing::Types<
    String::StringSize80,
    String100::StringSize100
>;
INSTANTIATE_TYPED_TEST_SUITE_P(Array, StringTest, StringTestImplementations);

template<>
U32 FppTest::String::getSize<String100::StringSize100>() {
    return 100;
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
