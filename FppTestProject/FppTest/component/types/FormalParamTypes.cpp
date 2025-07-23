// ======================================================================
// \title  FormalParamTypes.cpp
// \author T. Chieu
// \brief  cpp file for formal param types
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include <cstdint>

#include "FormalParamTypes.hpp"
#include "FppTest/utils/Utils.hpp"
#include "Fw/Types/StringTemplate.hpp"
#include "STest/Pick/Pick.hpp"

namespace FppTest {

namespace Types {

// ----------------------------------------------------------------------
// Primitive types
// ----------------------------------------------------------------------

BoolType::BoolType() {
    val = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
}

U32Type::U32Type() {
    val = STest::Pick::any();
}

I32Type::I32Type() {
    val = static_cast<I32>(STest::Pick::lowerUpper(0, INT32_MAX));
}

F32Type::F32Type() {
    val = static_cast<F32>(STest::Pick::any());
}

PrimitiveTypes::PrimitiveTypes() {
    val1 = STest::Pick::any();
    val2 = STest::Pick::any();
    val3 = static_cast<F32>(STest::Pick::any());
    val4 = static_cast<F32>(STest::Pick::any());
    val5 = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    val6 = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
}

// ----------------------------------------------------------------------
// FPP types
// ----------------------------------------------------------------------

EnumType::EnumType() {
    val = getRandomFormalParamEnum();
}

EnumTypes::EnumTypes() {
    val1 = getRandomFormalParamEnum();
    val2 = getRandomFormalParamEnum();
    val3 = getRandomFormalParamEnum();
    val4 = getRandomFormalParamEnum();
}

ArrayType::ArrayType() {
    getRandomFormalParamArray(val);
}

ArrayTypes::ArrayTypes() {
    getRandomFormalParamArray(val1);
    getRandomFormalParamArray(val2);
    getRandomFormalStringArray(val3);
    getRandomFormalStringArray(val4);
    getRandomFormalStringArray(val5);
    getRandomFormalStringArray(val6);
}

StructType::StructType() {
    val = getRandomFormalParamStruct();
}

StructTypes::StructTypes() {
    val1 = getRandomFormalParamStruct();
    val2 = getRandomFormalParamStruct();
}

// ----------------------------------------------------------------------
// String types
// ----------------------------------------------------------------------

PortStringType::PortStringType() {
    setRandomString(val);
}

PortStringTypes::PortStringTypes() {
    setRandomString(val1);
    setRandomString(val2);
    setRandomString(val3);
    setRandomString(val4);
}

InternalInterfaceStringType::InternalInterfaceStringType() {
    setRandomString(val);
}

InternalInterfaceStringTypes::InternalInterfaceStringTypes() {
    setRandomString(val1);
    setRandomString(val2);
}

CmdStringType::CmdStringType() {
    setRandomString(val, FW_CMD_STRING_MAX_SIZE);
}

CmdStringTypes::CmdStringTypes() {
    setRandomString(val1, FW_CMD_STRING_MAX_SIZE / 2);
    setRandomString(val2, FW_CMD_STRING_MAX_SIZE / 2);
}

LogStringType::LogStringType() {
    setRandomString(val, FW_LOG_STRING_MAX_SIZE);
}

LogStringTypes::LogStringTypes() {
    setRandomString(val1, FW_LOG_STRING_MAX_SIZE / 2);
    setRandomString(val2, FW_LOG_STRING_MAX_SIZE / 2);
}

TlmStringType::TlmStringType() {
    setRandomString(val, FW_TLM_STRING_MAX_SIZE);
}

TlmStringTypes::TlmStringTypes() {
    setRandomString(val1, FW_TLM_STRING_MAX_SIZE / 2);
    setRandomString(val2, FW_TLM_STRING_MAX_SIZE / 2);
}

PrmStringType::PrmStringType() {
    setRandomString(val, FW_PARAM_STRING_MAX_SIZE);
}

PrmStringTypes::PrmStringTypes() {
    setRandomString(val1, FW_PARAM_STRING_MAX_SIZE / 2);
    setRandomString(val2, FW_PARAM_STRING_MAX_SIZE / 2);
}

StringType::StringType() {
    setRandomString(val, decltype(val)::STRING_SIZE);
}

AliasStringArrayType::AliasStringArrayType() {
    getRandomFormalStringArray(val);
}

StringTypes::StringTypes() {
    setRandomString(val1, decltype(val1)::STRING_SIZE / 2);
    setRandomString(val2, decltype(val2)::STRING_SIZE / 2);
}

// ----------------------------------------------------------------------
// Serial type
// ----------------------------------------------------------------------

SerialType::SerialType() : val(data, sizeof(data)) {
    U32 len = STest::Pick::lowerUpper(1, SERIAL_ARGS_BUFFER_CAPACITY);

    for (U32 i = 0; i < len; i++) {
        data[i] = Utils::getNonzeroU8();
    }
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void setRandomString(Fw::StringBase& str) {
    char buf[str.getCapacity()];
    Utils::setString(buf, sizeof(buf));
    str = buf;
}

void setRandomString(Fw::StringBase& str, U32 size) {
    char buf[size];
    Utils::setString(buf, size);
    str = buf;
}

FormalParamEnum getRandomFormalParamEnum() {
    FormalParamEnum e;

    e = static_cast<FormalParamEnum::T>(STest::Pick::lowerUpper(0, FormalParamEnum::NUM_CONSTANTS - 1));

    return e;
}

void getRandomFormalParamArray(FormalParamArray& a) {
    for (U32 i = 0; i < FormalParamArray::SIZE; i++) {
        a[i] = STest::Pick::any();
    }
}

void getRandomFormalStringArray(FormalAliasStringArray& a) {
    for (U32 i = 0; i < FormalParamArray::SIZE; i++) {
        setRandomString(a[i], static_cast<U32>(a[i].getCapacity()));
    }
}

FormalParamStruct getRandomFormalParamStruct() {
    FormalParamStruct s;

    char buf[s.get_y().getCapacity()];
    Utils::setString(buf, sizeof(buf));

    Fw::StringTemplate<80> str(buf);

    Utils::setString(buf, sizeof(buf));
    Fw::StringTemplate<80> str2(buf);

    s.set(STest::Pick::any(), str, str2);

    return s;
}

}  // namespace Types

}  // namespace FppTest
