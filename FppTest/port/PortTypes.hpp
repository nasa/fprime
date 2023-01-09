// ======================================================================
// \title  PortTypes.hpp
// \author T. Chieu
// \brief  hpp file for port types
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef FPP_TEST_PORT_STRUCTS_HPP
#define FPP_TEST_PORT_STRUCTS_HPP

#include "Fw/Types/SerialBuffer.hpp"

#include "FppTest/port/PortEnumEnumAc.hpp"
#include "FppTest/port/PortArrayArrayAc.hpp"
#include "FppTest/port/PortStructSerializableAc.hpp"
#include "FppTest/port/StringArgsPortAc.hpp"

#define SERIAL_ARGS_BUFFER_CAPACITY 256

namespace FppTest {

  namespace Port {

    // PortType template
    template <typename ArgType, typename ReturnType>
    struct PortType {
      ArgType args;
    };

    // Empty type
    struct Empty {};

    // ----------------------------------------------------------------------
    // Argument types
    // ----------------------------------------------------------------------

    struct PrimitiveArgs {
      PrimitiveArgs();

      U32 u32;
      U32 u32Ref;
      F32 f32;
      F32 f32Ref;
      bool b;
      bool bRef;
    };

    struct StringArgs {
      StringArgs();

      StringArgsPortStrings::StringSize80 str80;
      StringArgsPortStrings::StringSize80 str80Ref;
      StringArgsPortStrings::StringSize100 str100;
      StringArgsPortStrings::StringSize100 str100Ref;
    };

    struct EnumArgs {
      EnumArgs();

      PortEnum en;
      PortEnum enRef;
    };

    struct ArrayArgs {
      ArrayArgs();

      PortArray a;
      PortArray aRef;
    };

    struct StructArgs {
      StructArgs();

      PortStruct s;
      PortStruct sRef;
    };

    struct SerialArgs {
      SerialArgs();

      U8 data[SERIAL_ARGS_BUFFER_CAPACITY];
      Fw::SerialBuffer buf;
    };

    // ----------------------------------------------------------------------
    // Return types
    // ----------------------------------------------------------------------

    struct BoolReturn {
      BoolReturn();

      bool val;
    };

    struct PrimitiveReturn {
      PrimitiveReturn();

      U32 val;
    };

    struct EnumReturn {
      EnumReturn();

      PortEnum val;
    };

    struct ArrayReturn {
      ArrayReturn();

      PortArray val;
    };

    struct StructReturn {
      StructReturn();

      PortStruct val;
    };

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------
    
    PortEnum getRandomPortEnum();
    PortArray getRandomPortArray();
    PortStruct getRandomPortStruct();

    // ----------------------------------------------------------------------
    // Typedefs
    // ----------------------------------------------------------------------

    typedef PortType<Empty, Empty> NoArgsPort;
    typedef PortType<PrimitiveArgs, Empty> PrimitiveArgsPort;
    typedef PortType<StringArgs, Empty> StringArgsPort;
    typedef PortType<EnumArgs, Empty> EnumArgsPort;
    typedef PortType<ArrayArgs, Empty> ArrayArgsPort;
    typedef PortType<StructArgs, Empty> StructArgsPort;
    typedef PortType<SerialArgs, Empty> SerialArgsPort;

    typedef PortType<Empty, BoolReturn> NoArgsReturnPort;
    typedef PortType<PrimitiveArgs, PrimitiveReturn> PrimitiveReturnPort;
    typedef PortType<EnumArgs, EnumReturn> EnumReturnPort;
    typedef PortType<ArrayArgs, ArrayReturn> ArrayReturnPort;
    typedef PortType<StructArgs, StructReturn> StructReturnPort;

  } // namespace Port

} // namespace FppTest
  
#endif
