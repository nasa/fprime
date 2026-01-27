// ======================================================================
// \title  main.cpp
// \author T. Chieu, A. Tumbar, R. Bocchino
// \brief  main cpp file for FPP array tests
// ======================================================================

#include "FppTest/array/AliasOfArrayAliasAc.hpp"
#include "FppTest/array/AliasStringArrayAc.hpp"
#include "FppTest/array/C_AArrayAc.hpp"
#include "FppTest/array/EnumArrayAc.hpp"
#include "FppTest/array/SM_AArrayAc.hpp"
#include "FppTest/array/StringArrayAc.hpp"
#include "FppTest/array/StructArrayAc.hpp"
#include "FppTest/array/Uint32ArrayArrayAc.hpp"
#include "FppTest/typed_tests/ArrayTest.hpp"
#include "FppTest/typed_tests/StringTest.hpp"
#include "FppTest/utils/Utils.hpp"
#include "Fw/Types/ExternalString.hpp"
#include "Fw/Types/StringTemplate.hpp"
#include "STest/Random/Random.hpp"
#include "gtest/gtest.h"

namespace FppTest {

namespace Array {

// ----------------------------------------------------------------------
// Test instantiations
// ----------------------------------------------------------------------

// Array tests
using ArrayTestImplementations =
    ::testing::Types<AliasOfArray, AliasString, C_A, Enum, SM_A, String, Struct, Uint32Array>;
INSTANTIATE_TYPED_TEST_SUITE_P(FppTest, ArrayTest, ArrayTestImplementations);

// String tests
using StringTestImplementations = ::testing::Types<Fw::StringTemplate<80>, Fw::StringTemplate<100>>;
INSTANTIATE_TYPED_TEST_SUITE_P(Array, StringTest, StringTestImplementations);

// ----------------------------------------------------------------------
// Template specializations for AliasOfArray type
// ----------------------------------------------------------------------

template <>
void setDefaultVals<AliasOfArray>(E (&a)[Enum::SIZE]) {
    a[0] = E::A;
    a[1] = E::B;
    a[2] = E::C;
}

template <>
void setTestVals<AliasOfArray>(EA (&a)[AliasOfArray::SIZE]) {
    a[0] = static_cast<EA::T>(STest::Pick::startLength(EA::B, EA::NUM_CONSTANTS - 1));

    for (U32 i = 1; i < Enum::SIZE; i++) {
        a[i] = static_cast<EA::T>(STest::Pick::startLength(EA::A, EA::NUM_CONSTANTS - 1));
    }
}

template <>
AliasOfArray getMultiElementConstructedArray<AliasOfArray>(EA (&a)[AliasOfArray::SIZE]) {
    return AliasOfArray({a[0], a[1], a[2]});
}

template <>
U32 getSerializedSize<AliasString>(Fw::ExternalString (&a)[AliasString::SIZE]) {
    U32 serializedSize = 0;

    for (U32 i = 0; i < AliasString::SIZE; i++) {
        serializedSize += static_cast<U32>(a[i].serializedSize());
    }

    return serializedSize;
}

// ----------------------------------------------------------------------
// Template specializations for AliasString type
// ----------------------------------------------------------------------

static char aliasStringDefaultValsBuffer[AliasString::SIZE][AliasString::ELEMENT_BUFFER_SIZE];
static char aliasStringTestValsBuffer[AliasString::SIZE][AliasString::ELEMENT_BUFFER_SIZE];

template <>
void setDefaultVals<AliasString>(Fw::ExternalString (&a)[AliasString::SIZE]) {
    for (U32 i = 0; i < AliasString::SIZE; i++) {
        a[i].setBuffer(aliasStringDefaultValsBuffer[i], AliasString::ELEMENT_BUFFER_SIZE);
    }
}

template <>
void setTestVals<AliasString>(Fw::ExternalString (&a)[AliasString::SIZE]) {
    for (U32 i = 0; i < AliasString::SIZE; i++) {
        a[i].setBuffer(aliasStringTestValsBuffer[i], AliasString::ELEMENT_BUFFER_SIZE);
        FppTest::Utils::setString(aliasStringTestValsBuffer[i], AliasString::ELEMENT_BUFFER_SIZE, 1);
    }
}

template <>
AliasString getMultiElementConstructedArray<AliasString>(Fw::ExternalString (&a)[String::SIZE]) {
    return AliasString({Fw::String(a[0]), Fw::String(a[1]), Fw::String(a[2])});
}

// ----------------------------------------------------------------------
// Template specializations for C_A type
// ----------------------------------------------------------------------

template <>
void setDefaultVals<C_A>(U32 (&a)[C_A::SIZE]) {
    for (U32 i = 0; i < C_A::SIZE; i++) {
        a[i] = 0;
    }
}

template <>
void setTestVals<C_A>(U32 (&a)[C_A::SIZE]) {
    for (U32 i = 1; i < C_A::SIZE; i++) {
        a[i] = STest::Pick::any();
    }
}

template <>
C_A getMultiElementConstructedArray<C_A>(U32 (&a)[C_A::SIZE]) {
    return C_A({a[0], a[1], a[2]});
}

// ----------------------------------------------------------------------
// Template specializations for Enum type
// ----------------------------------------------------------------------

template <>
void setDefaultVals<Enum>(E (&a)[Enum::SIZE]) {
    a[0] = E::A;
    a[1] = E::B;
    a[2] = E::C;
}

template <>
void setTestVals<Enum>(E (&a)[Enum::SIZE]) {
    a[0] = static_cast<E::T>(STest::Pick::startLength(E::B, E::NUM_CONSTANTS - 1));

    for (U32 i = 1; i < Enum::SIZE; i++) {
        a[i] = static_cast<E::T>(STest::Pick::startLength(E::A, E::NUM_CONSTANTS - 1));
    }
}

template <>
Enum getMultiElementConstructedArray<Enum>(E (&a)[Enum::SIZE]) {
    return Enum({a[0], a[1], a[2]});
}

// ----------------------------------------------------------------------
// Template specializations for SM_A type
// ----------------------------------------------------------------------

template <>
void setDefaultVals<SM_A>(U32 (&a)[SM_A::SIZE]) {
    for (U32 i = 0; i < SM_A::SIZE; i++) {
        a[i] = 0;
    }
}

template <>
void setTestVals<SM_A>(U32 (&a)[SM_A::SIZE]) {
    for (U32 i = 1; i < SM_A::SIZE; i++) {
        a[i] = STest::Pick::any();
    }
}

template <>
SM_A getMultiElementConstructedArray<SM_A>(U32 (&a)[SM_A::SIZE]) {
    return SM_A({a[0], a[1], a[2]});
}

// ----------------------------------------------------------------------
// Template specializations for String type
// ----------------------------------------------------------------------

static char stringDefaultValsBuffer[String::SIZE][String::ELEMENT_BUFFER_SIZE];
static char stringTestValsBuffer[String::SIZE][String::ELEMENT_BUFFER_SIZE];

template <>
void setDefaultVals<String>(Fw::ExternalString (&a)[String::SIZE]) {
    for (U32 i = 0; i < String::SIZE; i++) {
        a[i].setBuffer(stringDefaultValsBuffer[i], String::ELEMENT_BUFFER_SIZE);
    }
}

template <>
void setTestVals<String>(Fw::ExternalString (&a)[String::SIZE]) {
    for (U32 i = 0; i < String::SIZE; i++) {
        a[i].setBuffer(stringTestValsBuffer[i], String::ELEMENT_BUFFER_SIZE);
        FppTest::Utils::setString(stringTestValsBuffer[i], String::ELEMENT_BUFFER_SIZE, 1);
    }
}

template <>
String getMultiElementConstructedArray<String>(Fw::ExternalString (&a)[String::SIZE]) {
    return String({Fw::String(a[0]), Fw::String(a[1]), Fw::String(a[2])});
}

template <>
U32 getSerializedSize<String>(Fw::ExternalString (&a)[String::SIZE]) {
    U32 serializedSize = 0;

    for (U32 i = 0; i < String::SIZE; i++) {
        serializedSize += static_cast<U32>(a[i].serializedSize());
    }

    return serializedSize;
}

// ----------------------------------------------------------------------
// Template specializations for Struct type
// ----------------------------------------------------------------------

template <>
void setTestVals<Struct>(S (&a)[Struct::SIZE]) {
    U32 b[3];
    for (U32 i = 0; i < Struct::SIZE; i++) {
        for (U32 j = 0; j < 3; j++) {
            b[j] = FppTest::Utils::getNonzeroU32();
        }
        a[i].set(FppTest::Utils::getNonzeroU32(), b);
    }
}

template <>
Struct getMultiElementConstructedArray<Struct>(S (&a)[Struct::SIZE]) {
    return Struct({a[0], a[1], a[2]});
}

// ----------------------------------------------------------------------
// Template specializations for Uint32Array type
// ----------------------------------------------------------------------

template <>
void setTestVals<Uint32Array>(Uint32 (&a)[Uint32Array::SIZE]) {
    Uint32 b;
    for (U32 i = 0; i < Uint32Array::SIZE; i++) {
        for (U32 j = 0; j < Uint32::SIZE; j++) {
            b[j] = FppTest::Utils::getNonzeroU32();
        }
        a[i] = b;
    }
}

template <>
Uint32Array getMultiElementConstructedArray<Uint32Array>(Uint32 (&a)[Uint32Array::SIZE]) {
    return Uint32Array({a[0], a[1], a[2]});
}

}  // namespace Array

}  // namespace FppTest

// ----------------------------------------------------------------------
// Main program
// ----------------------------------------------------------------------

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();

    return RUN_ALL_TESTS();
}
