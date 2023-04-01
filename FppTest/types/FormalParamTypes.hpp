// ======================================================================
// \title  FormalParamTypes.hpp
// \author T. Chieu
// \brief  hpp file for port types
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_FORMAL_PARAM_TYPES_HPP
#define FPP_TEST_FORMAL_PARAM_TYPES_HPP

#include "Fw/Types/SerialBuffer.hpp"

#include "FppTest/types/FormalParamEnumEnumAc.hpp"
#include "FppTest/types/FormalParamArrayArrayAc.hpp"
#include "FppTest/types/FormalParamStructSerializableAc.hpp"
#include "FppTest/port/StringArgsPortAc.hpp"

#define SERIAL_ARGS_BUFFER_CAPACITY 256

namespace FppTest {

  namespace Types {

    template <typename ArgType, typename ReturnType>
    struct FormalParamsWithReturn {
      ArgType args;
    };

    // Empty type
    struct Empty {};

    template <typename ArgType>
    using FormalParams = FormalParamsWithReturn<ArgType, Empty>;

    // ----------------------------------------------------------------------
    // Multiple types
    // ----------------------------------------------------------------------

    struct PrimitiveTypes {
      PrimitiveTypes();

      U32 u32;
      U32 u32Ref;
      F32 f32;
      F32 f32Ref;
      bool b;
      bool bRef;
    };

    struct StringTypes {
      StringTypes();

      StringTypesPortStrings::StringSize80 str80;
      StringTypesPortStrings::StringSize80 str80Ref;
      StringTypesPortStrings::StringSize100 str100;
      StringTypesPortStrings::StringSize100 str100Ref;
    };

    struct EnumTypes {
      EnumTypes();

      FormalParamEnum en;
      FormalParamEnum enRef;
    };

    struct ArrayTypes {
      ArrayTypes();

      FormalParamArray a;
      FormalParamArray aRef;
    };

    struct StructTypes {
      StructTypes();

      FormalParamStruct s;
      FormalParamStruct sRef;
    };

    struct SerialTypes {
      SerialTypes();

      U8 data[SERIAL_ARGS_BUFFER_CAPACITY];
      Fw::SerialBuffer buf;
    };

    // ----------------------------------------------------------------------
    // Single types
    // ----------------------------------------------------------------------

    struct BoolType {
      BoolType();

      bool val;
    };

    struct PrimitiveType {
      PrimitiveType();

      U32 val;
    };

    struct EnumType {
      EnumType();

      FormalParamEnum val;
    };

    struct ArrayType {
      ArrayType();

      FormalParamArray val;
    };

    struct StructType {
      StructType();

      FormalParamStruct val;
    };

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------
    
    FormalParamEnum getRandomFormalParamEnum();
    FormalParamArray getRandomFormalParamArray();
    FormalParamStruct getRandomFormalParamStruct();

    // ----------------------------------------------------------------------
    // Typedefs
    // ----------------------------------------------------------------------

    typedef ForamlParams<Empty> NoArgs;
    typedef ForamlParams<PrimitiveTypes> PrimitiveArgs;
    typedef ForamlParams<StringTypes> StringArgs;
    typedef ForamlParams<EnumTypes> EnumArgs;
    typedef ForamlParams<ArrayTypes> ArrayArgs;
    typedef ForamlParams<StructTypes> StructArgs;
    typedef ForamlParams<SerialTypes> SerialArgs;

    typedef ForamlParamsWithReturn<Empty, BoolType> NoArgsReturn;
    typedef ForamlParamsWithReturn<PrimitiveTypes, PrimitiveType> PrimitiveReturn;
    typedef ForamlParamsWithReturn<EnumTypes, EnumType> EnumReturn;
    typedef ForamlParamsWithReturn<ArrayTypes, ArrayType> ArrayReturn;
    typedef ForamlParamsWithReturn<StructTypes, StructType> StructReturn;

  } // namespace Types

} // namespace FppTest
  
#endif
