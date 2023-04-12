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

#include "Fw/Types/InternalInterfaceString.hpp"
#include "Fw/Cmd/CmdString.hpp"
#include "Fw/Log/LogString.hpp"
#include "Fw/Tlm/TlmString.hpp"
#include "Fw/Prm/PrmString.hpp"
#include "Fw/Types/SerialBuffer.hpp"

#include "FppTest/types/FormalParamEnumEnumAc.hpp"
#include "FppTest/types/FormalParamArrayArrayAc.hpp"
#include "FppTest/types/FormalParamStructSerializableAc.hpp"
#include "FppTest/types/StringArgsPortAc.hpp"

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
    // Primitive types
    // ----------------------------------------------------------------------

    struct BoolType {
      BoolType();

      bool b;
    };

    struct IntegerType {
      PrimitiveType();

      U32 u32;
    };

    struct PrimitiveTypes {
      PrimitiveTypes();

      U32 u32_1;
      U32 u32_2;
      F32 f32_1;
      F32 f32_2;
      bool b1;
      bool b2;
    };

    // ----------------------------------------------------------------------
    // FPP types
    // ----------------------------------------------------------------------

    struct EnumType {
      EnumType();

      FormalParamEnum en;
    };

    struct EnumTypes {
      EnumTypes();

      FormalParamEnum en1;
      FormalParamEnum en2;
    };

    struct ArrayType {
      ArrayType();

      FormalParamArray arr;
    };

    struct ArrayTypes {
      ArrayTypes();

      FormalParamArray arr1;
      FormalParamArray arr2;
    };

    struct StructType {
      StructType();

      FormalParamStruct str;
    };

    struct StructTypes {
      StructTypes();

      FormalParamStruct str1;
      FormalParamStruct str2;
    };

    // ----------------------------------------------------------------------
    // String types
    // ----------------------------------------------------------------------

    struct PortStringType {
      PortStringType();

      StringTypesPortStrings::StringSize80 str;
    };

    struct PortStringTypes {
      PortStringTypes();

      StringTypesPortStrings::StringSize80 str80_1;
      StringTypesPortStrings::StringSize80 str80_2;
      StringTypesPortStrings::StringSize100 str100_1;
      StringTypesPortStrings::StringSize100 str100_2;
    };

    struct InternalInterfaceStringType {
      InternalInterfaceStringType();

      Fw::InternalInterfaceString str;
    }

    struct InternalInterfaceStringTypes {
      InternalInterfaceStringTypes();

      Fw::InternalInterfaceString str80;
      Fw::InternalInterfaceString str100;
    }

    struct CmdStringType {
      CmdStringType();

      Fw::CmdStringArg str;
    }

    struct CmdStringTypes {
      CmdStringTypes();

      Fw::CmdStringArg str80;
      Fw::CmdStringArg str100;
    }

    struct LogStringType {
      LogStringType();

      Fw::LogStringArg str;
    }

    struct LogStringTypes {
      LogStringTypes();

      Fw::LogStringArg str80;
      Fw::LogStringArg str100;
    }

    struct TlmStringType {
      TlmStringType();

      Fw::TlmString str;
    }

    struct TlmStringTypes {
      TlmStringTypes();

      Fw::TlmString str80;
      Fw::TlmString str100;
    }

    struct PrmStringType {
      PrmStringType();

      Fw::ParamString str;
    }

    struct PrmStringTypes {
      PrmStringTypes();

      Fw::ParamString str80;
      Fw::ParamString str100;
    }

    // ----------------------------------------------------------------------
    // Serial type
    // ----------------------------------------------------------------------

    struct SerialType {
      SerialType();

      U8 data[SERIAL_ARGS_BUFFER_CAPACITY];
      Fw::SerialBuffer buf;
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

    typedef ForamlParamsWithReturn<Empty, BoolType> NoArgsReturn;
    typedef ForamlParamsWithReturn<PrimitiveTypes, PrimitiveType> PrimitiveReturn;
    typedef ForamlParamsWithReturn<EnumTypes, EnumType> EnumReturn;
    typedef ForamlParamsWithReturn<ArrayTypes, ArrayType> ArrayReturn;
    typedef ForamlParamsWithReturn<StructTypes, StructType> StructReturn;

  } // namespace Types

} // namespace FppTest
  
#endif
