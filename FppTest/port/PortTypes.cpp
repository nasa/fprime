// ======================================================================
// \title  PortTypes.cpp
// \author T. Chieu
// \brief  cpp file for port types
//
// \copyright
// Copyright (C) 2009-2023 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#include "PortTypes.hpp"

#include "STest/Pick/Pick.hpp"
#include "FppTest/utils/Utils.hpp"

namespace FppTest {

  namespace Port {

    // ----------------------------------------------------------------------
    // Argument types
    // ----------------------------------------------------------------------

    PrimitiveArgs::PrimitiveArgs() {
      u32 = STest::Pick::any();
      u32Ref = STest::Pick::any();
      f32 = static_cast<F32>(STest::Pick::any());
      f32Ref = static_cast<F32>(STest::Pick::any());
      b = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
      bRef = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    }
    
    StringArgs::StringArgs() {
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

    EnumArgs::EnumArgs() {
      en = getRandomPortEnum();
      enRef = getRandomPortEnum();
    }

    ArrayArgs::ArrayArgs() {
      a = getRandomPortArray();
      aRef = getRandomPortArray();
    }

    StructArgs::StructArgs() {
      s = getRandomPortStruct();
      sRef = getRandomPortStruct();
    }

    SerialArgs::SerialArgs() : buf(data, sizeof(data)) {
      U32 len = STest::Pick::lowerUpper(1, SERIAL_ARGS_BUFFER_CAPACITY);

      for (U32 i = 0; i < len; i++) {
        data[i] = Utils::getU8();
      }
    }

    // ----------------------------------------------------------------------
    // Return types
    // ----------------------------------------------------------------------


    BoolReturn::BoolReturn() {
      val = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    }

    PrimitiveReturn::PrimitiveReturn() {
      val = STest::Pick::any();
    }

    EnumReturn::EnumReturn() {
      val = getRandomPortEnum();
    }

    ArrayReturn::ArrayReturn() {
      val = getRandomPortArray();
    }

    StructReturn::StructReturn() {
      val = getRandomPortStruct();
    }

    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    PortEnum getRandomPortEnum() {
      PortEnum e;

      e = static_cast<PortEnum::T>(STest::Pick::lowerUpper(
        0,
        PortEnum::NUM_CONSTANTS - 1
      ));

      return e;
    }

    PortArray getRandomPortArray() {
      PortArray a;

      for (U32 i = 0; i < PortArray::SIZE; i++) {
        a[i] = STest::Pick::any();
      }

      return a;
    }

    PortStruct getRandomPortStruct() {
      PortStruct s;
      char buf[s.gety().getCapacity()];
      PortStruct::StringSize80 str = buf;

      Utils::setString(buf, sizeof(buf));
      s.set(STest::Pick::any(), str);

      return s;
    }

  } // namespace Port

} // namespace FppTest
