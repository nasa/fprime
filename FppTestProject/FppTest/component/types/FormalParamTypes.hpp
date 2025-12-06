// ======================================================================
// \title  FormalParamTypes.hpp
// \author T. Chieu
// \brief  hpp file for formal param types
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_FORMAL_PARAM_TYPES_HPP
#define FPP_TEST_FORMAL_PARAM_TYPES_HPP

#include "Fw/Cmd/CmdString.hpp"
#include "Fw/Log/LogString.hpp"
#include "Fw/Prm/PrmString.hpp"
#include "Fw/Tlm/TlmString.hpp"
#include "Fw/Types/InternalInterfaceString.hpp"
#include "Fw/Types/SerialBuffer.hpp"
#include "Fw/Types/StringTemplate.hpp"

#include "FppTest/component/active/FormalAliasArrayAliasAc.hpp"
#include "FppTest/component/active/FormalAliasEnumAliasAc.hpp"
#include "FppTest/component/active/FormalAliasStringAliasAc.hpp"
#include "FppTest/component/active/FormalAliasStringArrayArrayAc.hpp"
#include "FppTest/component/active/FormalParamArrayArrayAc.hpp"
#include "FppTest/component/active/FormalParamEnumEnumAc.hpp"
#include "FppTest/component/active/FormalParamStructSerializableAc.hpp"

#define SERIAL_ARGS_BUFFER_CAPACITY 256

namespace FppTest {

namespace Types {

template <typename ArgType, typename ReturnType>
struct FormalParamsWithReturn {
    ArgType args;
};

// Empty type
struct Empty : Fw::Serializable {
    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

template <typename ArgType>
using FormalParams = FormalParamsWithReturn<ArgType, Empty>;

// ----------------------------------------------------------------------
// Primitive types
// ----------------------------------------------------------------------

struct BoolType : Fw::Serializable {
    BoolType();

    bool val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct U32Type : Fw::Serializable {
    U32Type();

    U32 val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct I32Type : Fw::Serializable {
    I32Type();

    I32 val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct F32Type : Fw::Serializable {
    F32Type();

    F32 val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct PrimitiveTypes : Fw::Serializable {
    PrimitiveTypes();

    U32 val1;
    U32 val2;
    F32 val3;
    F32 val4;
    bool val5;
    bool val6;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

// ----------------------------------------------------------------------
// FPP types
// ----------------------------------------------------------------------

struct EnumType : Fw::Serializable {
    EnumType();

    FormalParamEnum val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct EnumTypes : Fw::Serializable {
    EnumTypes();

    FormalParamEnum val1;
    FormalParamEnum val2;
    FormalAliasEnum val3;
    FormalAliasEnum val4;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct EnumTypesShort : Fw::Serializable {
    EnumTypesShort();

    FormalParamEnum val1;
    FormalParamEnum val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct ArrayType : Fw::Serializable {
    ArrayType();

    FormalParamArray val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct ArrayTypes : Fw::Serializable {
    ArrayTypes();

    FormalParamArray val1;
    FormalParamArray val2;
    FormalAliasArray val3;
    FormalAliasArray val4;
    FormalAliasStringArray val5;
    FormalAliasStringArray val6;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct ArrayTypesShort : Fw::Serializable {
    ArrayTypesShort();

    FormalParamArray val1;
    FormalParamArray val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct StructType : Fw::Serializable {
    StructType();

    FormalParamStruct val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct StructTypes : Fw::Serializable {
    StructTypes();

    FormalParamStruct val1;
    FormalParamStruct val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

// ----------------------------------------------------------------------
// String types
// ----------------------------------------------------------------------

using String1 = Fw::StringTemplate<80>;
using String2 = Fw::StringTemplate<100>;

struct PortStringType : Fw::Serializable {
    PortStringType();

    String1 val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct PortStringTypes : Fw::Serializable {
    PortStringTypes();

    String1 val1;
    String1 val2;
    String2 val3;
    String2 val4;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct InternalInterfaceStringType : Fw::Serializable {
    InternalInterfaceStringType();

    Fw::InternalInterfaceString val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct InternalInterfaceStringTypes : Fw::Serializable {
    InternalInterfaceStringTypes();

    Fw::InternalInterfaceString val1;
    Fw::InternalInterfaceString val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct CmdStringType : Fw::Serializable {
    CmdStringType();

    Fw::CmdStringArg val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct CmdStringTypes : Fw::Serializable {
    CmdStringTypes();

    Fw::CmdStringArg val1;
    Fw::CmdStringArg val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct LogStringType : Fw::Serializable {
    LogStringType();

    Fw::LogStringArg val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct LogStringTypes : Fw::Serializable {
    LogStringTypes();

    Fw::LogStringArg val1;
    Fw::LogStringArg val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct TlmStringType : Fw::Serializable {
    TlmStringType();

    Fw::TlmString val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct TlmStringTypes : Fw::Serializable {
    TlmStringTypes();

    Fw::TlmString val1;
    Fw::TlmString val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct PrmStringType : Fw::Serializable {
    PrmStringType();

    Fw::ParamString val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct PrmStringTypes : Fw::Serializable {
    PrmStringTypes();

    Fw::ParamString val1;
    Fw::ParamString val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct StringType : Fw::Serializable {
    StringType();

    String1 val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct AliasStringArrayType : Fw::Serializable {
    AliasStringArrayType();

    FormalAliasStringArray val;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

struct StringTypes : Fw::Serializable {
    StringTypes();

    String1 val1;
    String1 val2;

    Fw::SerializeStatus serializeTo(Fw::SerialBufferBase& buffer, Fw::Endianness mode) const override;
    Fw::SerializeStatus deserializeFrom(Fw::SerialBufferBase& buffer, Fw::Endianness mode) override;
};

// ----------------------------------------------------------------------
// Serial type
// ----------------------------------------------------------------------

struct SerialType {
    SerialType();

    U8 data[SERIAL_ARGS_BUFFER_CAPACITY];
    Fw::SerialBuffer val;
};

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void setRandomString(Fw::StringBase& str);
void setRandomString(Fw::StringBase& str, U32 size);
FormalParamEnum getRandomFormalParamEnum();
void getRandomFormalParamArray(FormalParamArray& a);
FormalParamStruct getRandomFormalParamStruct();
void getRandomFormalStringArray(FormalAliasStringArray& a);

// ----------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------

typedef FormalParams<Empty> NoParams;
typedef FormalParams<BoolType> BoolParam;
typedef FormalParams<U32Type> U32Param;
typedef FormalParams<I32Type> I32Param;
typedef FormalParams<F32Type> F32Param;
typedef FormalParams<PrimitiveTypes> PrimitiveParams;
typedef FormalParams<EnumType> EnumParam;
typedef FormalParams<EnumTypes> EnumParams;
typedef FormalParams<ArrayType> ArrayParam;
typedef FormalParams<ArrayTypes> ArrayParams;
typedef FormalParams<StructType> StructParam;
typedef FormalParams<StructTypes> StructParams;
typedef FormalParams<PortStringType> PortStringParam;
typedef FormalParams<PortStringTypes> PortStringParams;
typedef FormalParams<InternalInterfaceStringType> InternalInterfaceStringParam;
typedef FormalParams<InternalInterfaceStringTypes> InternalInterfaceStringParams;
typedef FormalParams<CmdStringType> CmdStringParam;
typedef FormalParams<CmdStringTypes> CmdStringParams;
typedef FormalParams<LogStringType> LogStringParam;
typedef FormalParams<LogStringTypes> LogStringParams;
typedef FormalParams<TlmStringType> TlmStringParam;
typedef FormalParams<TlmStringTypes> TlmStringParams;
typedef FormalParams<PrmStringType> PrmStringParam;
typedef FormalParams<PrmStringTypes> PrmStringParams;
typedef FormalParams<SerialType> SerialParam;

typedef FormalParamsWithReturn<Empty, BoolType> NoParamReturn;
typedef FormalParamsWithReturn<PrimitiveTypes, U32Type> PrimitiveReturn;
typedef FormalParamsWithReturn<EnumTypes, EnumType> EnumReturn;
typedef FormalParamsWithReturn<StringTypes, StringType> StringReturn;
typedef FormalParamsWithReturn<StringTypes, FormalAliasString> StringAliasReturn;
typedef FormalParamsWithReturn<ArrayTypes, ArrayType> ArrayReturn;
typedef FormalParamsWithReturn<ArrayTypes, FormalAliasStringArray> ArrayStringAliasReturn;
typedef FormalParamsWithReturn<StructTypes, StructType> StructReturn;

}  // namespace Types

}  // namespace FppTest

#endif
