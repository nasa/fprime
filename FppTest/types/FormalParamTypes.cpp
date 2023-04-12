// ======================================================================
// \title  FormalParamTypes.cpp
// \author T. Chieu
// \brief  cpp file for port types
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "FormalParamTypes.hpp"

#include "STest/Pick/Pick.hpp"
#include "FppTest/utils/Utils.hpp"

namespace FppTest {

  namespace Types {

    // ----------------------------------------------------------------------
    // Primitive types
    // ----------------------------------------------------------------------

    BoolType::BoolType() {
      b = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    }

    PrimitiveType::PrimitiveType() {
      u32 = STest::Pick::any();
    }

    PrimitiveTypes::PrimitiveTypes() {
      u32_1 = STest::Pick::any();
      u32_2 = STest::Pick::any();
      f32_1 = static_cast<F32>(STest::Pick::any());
      f32_2 = static_cast<F32>(STest::Pick::any());
      b1 = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
      b2 = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    }

    // ----------------------------------------------------------------------
    // FPP types
    // ----------------------------------------------------------------------

    EnumType::EnumType() {
      en = getRandomFormalParamEnum();
    }

    EnumTypes::EnumTypes() {
      en1 = getRandomFormalParamEnum();
      en2 = getRandomFormalParamEnum();
    }

    ArrayType::ArrayType() {
      arr = getRandomFormalParamArray();
    }

    ArrayTypes::ArrayTypes() {
      arr1 = getRandomFormalParamArray();
      arr2 = getRandomFormalParamArray();
    }

    StructType::StructType() {
      str = getRandomFormalParamStruct();
    }

    StructTypes::StructTypes() {
      str1 = getRandomFormalParamStruct();
      str2 = getRandomFormalParamStruct();
    }

    // ----------------------------------------------------------------------
    // String types
    // ----------------------------------------------------------------------
    
    PortStringType::PortStringType() {
      char buf[str.getCapacity()];
      Utils::setString(buf, sizeof(buf));
      str = buf;
    }
    
    PortStringTypes::PortStringTypes() {
      char buf80[str80.getCapacity()];
      char buf100[str100.getCapacity()];

      Utils::setString(buf80, sizeof(buf80));
      Utils::setString(buf100, sizeof(buf100));

      str80_1 = buf80;
      str100_1 = buf100;

      Utils::setString(buf80, sizeof(buf80));
      Utils::setString(buf100, sizeof(buf100));

      str80_2 = buf80;
      str100_2 = buf100;
    }

    InternalInterfaceStringType::InternalInterfaceStringType() {
      char buf[str.getCapacity()];
      Utils::setString(buf, sizeof(buf));
      str = buf;
    }

    InternalInterfaceStringTypes::InternalInterfaceStringTypes() {
    }

    CmdStringType::CmdStringType() {
    }

    CmdStringTypes::CmdStringTypes() {
    }

    LogStringType::LogStringType() {
    }

    LogStringTypes::LogStringTypes() {
    }

    TlmStringType::TlmStringType() {
    }

    TlmStringTypes::TlmStringTypes() {
    }

    PrmStringType::PrmStringType() {
    }

    PrmStringTypes::PrmStringTypes() {
    }

    // ----------------------------------------------------------------------
    // Serial types
    // ----------------------------------------------------------------------

    SerialTypes::SerialTypes() : buf(data, sizeof(data)) {
      U32 len = STest::Pick::lowerUpper(1, SERIAL_ARGS_BUFFER_CAPACITY);

      for (U32 i = 0; i < len; i++) {
        data[i] = Utils::getU8();
      }
    }

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    FormalParamEnum getRandomFormalParamEnum() {
      FormalParamEnum e;

      e = static_cast<FormalParamEnum::T>(STest::Pick::lowerUpper(
        0,
        FormalParamEnum::NUM_CONSTANTS - 1
      ));

      return e;
    }

    FormalParamArray getRandomFormalParamArray() {
      FormalParamArray a;

      for (U32 i = 0; i < FormalParamArray::SIZE; i++) {
        a[i] = STest::Pick::any();
      }

      return a;
    }

    FormalParamStruct getRandomFormalParamStruct() {
      FormalParamStruct s;
      char buf[s.gety().getCapacity()];
      FormalParamStruct::StringSize80 str = buf;

      Utils::setString(buf, sizeof(buf));
      s.set(STest::Pick::any(), str);

      return s;
    }

  } // namespace Types

} // namespace FppTest
