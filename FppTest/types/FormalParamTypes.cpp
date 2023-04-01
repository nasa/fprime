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
    // Multiple types
    // ----------------------------------------------------------------------

    PrimitiveTypes::PrimitiveTypes() {
      u32 = STest::Pick::any();
      u32Ref = STest::Pick::any();
      f32 = static_cast<F32>(STest::Pick::any());
      f32Ref = static_cast<F32>(STest::Pick::any());
      b = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
      bRef = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    }
    
    StringTypes::StringTypes() {
      char buf80[str80.getCapacity()];
      char buf100[str100.getCapacity()];

      Utils::setString(buf80, sizeof(buf80));
      Utils::setString(buf100, sizeof(buf100));

      str80 = buf80;
      str100 = buf100;

      Utils::setString(buf80, sizeof(buf80));
      Utils::setString(buf100, sizeof(buf100));

      str80Ref = buf80;
      str100Ref = buf100;
    }

    EnumTypes::EnumTypes() {
      en = getRandomFormalParamEnum();
      enRef = getRandomFormalParamEnum();
    }

    ArrayTypes::ArrayTypes() {
      a = getRandomFormalParamArray();
      aRef = getRandomFormalParamArray();
    }

    StructTypes::StructTypes() {
      s = getRandomFormalParamStruct();
      sRef = getRandomFormalParamStruct();
    }

    SerialTypes::SerialTypes() : buf(data, sizeof(data)) {
      U32 len = STest::Pick::lowerUpper(1, SERIAL_ARGS_BUFFER_CAPACITY);

      for (U32 i = 0; i < len; i++) {
        data[i] = Utils::getU8();
      }
    }

    // ----------------------------------------------------------------------
    // Single types
    // ----------------------------------------------------------------------


    BoolType::BoolType() {
      val = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    }

    PrimitiveType::PrimitiveType() {
      val = STest::Pick::any();
    }

    EnumType::EnumType() {
      val = getRandomFormalParamEnum();
    }

    ArrayType::ArrayType() {
      val = getRandomFormalParamArray();
    }

    StructType::StructType() {
      val = getRandomFormalParamStruct();
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
