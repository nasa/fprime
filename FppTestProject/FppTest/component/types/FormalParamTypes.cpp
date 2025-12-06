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

Fw::SerializeStatus Empty::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus Empty::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

BoolType::BoolType() {
    val = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
}

Fw::SerializeStatus BoolType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus BoolType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

U32Type::U32Type() {
    val = STest::Pick::any();
}

Fw::SerializeStatus U32Type::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus U32Type::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

I32Type::I32Type() {
    val = static_cast<I32>(STest::Pick::lowerUpper(0, INT32_MAX));
}

Fw::SerializeStatus I32Type::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus I32Type::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

F32Type::F32Type() {
    val = static_cast<F32>(STest::Pick::any());
}

Fw::SerializeStatus F32Type::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus F32Type::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

PrimitiveTypes::PrimitiveTypes() {
    val1 = STest::Pick::any();
    val2 = STest::Pick::any();
    val3 = static_cast<F32>(STest::Pick::any());
    val4 = static_cast<F32>(STest::Pick::any());
    val5 = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    val6 = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
}

Fw::SerializeStatus PrimitiveTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    // TODO(tumbar) Should we check status here?
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    buffer.serializeFrom(val3, mode);
    buffer.serializeFrom(val4, mode);
    buffer.serializeFrom(val5, mode);
    buffer.serializeFrom(val6, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus PrimitiveTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

// ----------------------------------------------------------------------
// FPP types
// ----------------------------------------------------------------------

EnumType::EnumType() {
    val = getRandomFormalParamEnum();
}

Fw::SerializeStatus EnumType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus EnumType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

EnumTypes::EnumTypes() {
    val1 = getRandomFormalParamEnum();
    val2 = getRandomFormalParamEnum();
    val3 = getRandomFormalParamEnum();
    val4 = getRandomFormalParamEnum();
}

Fw::SerializeStatus EnumTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    buffer.serializeFrom(val3, mode);
    buffer.serializeFrom(val4, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus EnumTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}
EnumTypesShort::EnumTypesShort() {
    val1 = getRandomFormalParamEnum();
    val2 = getRandomFormalParamEnum();
}

Fw::SerializeStatus EnumTypesShort::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}
Fw::SerializeStatus EnumTypesShort::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}
ArrayType::ArrayType() {
    getRandomFormalParamArray(val);
}

Fw::SerializeStatus ArrayType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus ArrayType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

ArrayTypes::ArrayTypes() {
    getRandomFormalParamArray(val1);
    getRandomFormalParamArray(val2);
    getRandomFormalStringArray(val3);
    getRandomFormalStringArray(val4);
    getRandomFormalStringArray(val5);
    getRandomFormalStringArray(val6);
}

Fw::SerializeStatus ArrayTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    buffer.serializeFrom(val3, mode);
    buffer.serializeFrom(val4, mode);
    buffer.serializeFrom(val5, mode);
    buffer.serializeFrom(val6, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus ArrayTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

ArrayTypesShort::ArrayTypesShort() {
    getRandomFormalParamArray(val1);
    getRandomFormalParamArray(val2);
}

Fw::SerializeStatus ArrayTypesShort::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus ArrayTypesShort::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

StructType::StructType() {
    val = getRandomFormalParamStruct();
}

Fw::SerializeStatus StructType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus StructType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

StructTypes::StructTypes() {
    val1 = getRandomFormalParamStruct();
    val2 = getRandomFormalParamStruct();
}

// ----------------------------------------------------------------------
// String types
// ----------------------------------------------------------------------

Fw::SerializeStatus StructTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus StructTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

PortStringType::PortStringType() {
    setRandomString(val);
}

Fw::SerializeStatus PortStringType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus PortStringType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

PortStringTypes::PortStringTypes() {
    setRandomString(val1);
    setRandomString(val2);
    setRandomString(val3);
    setRandomString(val4);
}

Fw::SerializeStatus PortStringTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    buffer.serializeFrom(val3, mode);
    buffer.serializeFrom(val4, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus PortStringTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

InternalInterfaceStringType::InternalInterfaceStringType() {
    setRandomString(val);
}

Fw::SerializeStatus InternalInterfaceStringType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus InternalInterfaceStringType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

InternalInterfaceStringTypes::InternalInterfaceStringTypes() {
    setRandomString(val1);
    setRandomString(val2);
}

Fw::SerializeStatus InternalInterfaceStringTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus InternalInterfaceStringTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

CmdStringType::CmdStringType() {
    setRandomString(val, FW_CMD_STRING_MAX_SIZE);
}

Fw::SerializeStatus CmdStringType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus CmdStringType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

CmdStringTypes::CmdStringTypes() {
    setRandomString(val1, FW_CMD_STRING_MAX_SIZE / 2);
    setRandomString(val2, FW_CMD_STRING_MAX_SIZE / 2);
}

Fw::SerializeStatus CmdStringTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus CmdStringTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

LogStringType::LogStringType() {
    setRandomString(val, FW_LOG_STRING_MAX_SIZE);
}

Fw::SerializeStatus LogStringType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus LogStringType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

LogStringTypes::LogStringTypes() {
    setRandomString(val1, FW_LOG_STRING_MAX_SIZE / 2);
    setRandomString(val2, FW_LOG_STRING_MAX_SIZE / 2);
}

Fw::SerializeStatus LogStringTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus LogStringTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

TlmStringType::TlmStringType() {
    setRandomString(val, FW_TLM_STRING_MAX_SIZE);
}

Fw::SerializeStatus TlmStringType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus TlmStringType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

TlmStringTypes::TlmStringTypes() {
    setRandomString(val1, FW_TLM_STRING_MAX_SIZE / 2);
    setRandomString(val2, FW_TLM_STRING_MAX_SIZE / 2);
}

Fw::SerializeStatus TlmStringTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus TlmStringTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

PrmStringType::PrmStringType() {
    setRandomString(val, FW_PARAM_STRING_MAX_SIZE);
}

Fw::SerializeStatus PrmStringType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus PrmStringType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

PrmStringTypes::PrmStringTypes() {
    setRandomString(val1, FW_PARAM_STRING_MAX_SIZE / 2);
    setRandomString(val2, FW_PARAM_STRING_MAX_SIZE / 2);
}

Fw::SerializeStatus PrmStringTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus PrmStringTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

StringType::StringType() {
    setRandomString(val, decltype(val)::STRING_SIZE);
}

Fw::SerializeStatus StringType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    return buffer.serializeFrom(val, mode);
}

Fw::SerializeStatus StringType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

AliasStringArrayType::AliasStringArrayType() {
    getRandomFormalStringArray(val);
}

Fw::SerializeStatus AliasStringArrayType::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus AliasStringArrayType::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

StringTypes::StringTypes() {
    setRandomString(val1, decltype(val1)::STRING_SIZE / 2);
    setRandomString(val2, decltype(val2)::STRING_SIZE / 2);
}

// ----------------------------------------------------------------------
// Serial type
// ----------------------------------------------------------------------

Fw::SerializeStatus StringTypes::serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const {
    buffer.serializeFrom(val1, mode);
    buffer.serializeFrom(val2, mode);
    return Fw::SerializeStatus::FW_SERIALIZE_OK;
}

Fw::SerializeStatus StringTypes::deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) {
    return Fw::SerializeStatus::FW_DESERIALIZE_FORMAT_ERROR;
}

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
