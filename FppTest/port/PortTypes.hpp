#ifndef FPP_TEST_PORT_STRUCTS_HPP
#define FPP_TEST_PORT_STRUCTS_HPP

#include "STest/Pick/Pick.hpp"

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
      PrimitiveArgs() {
        u32 = STest::Pick::any();
        u32Ref = STest::Pick::any();
        f32 = static_cast<F32>(STest::Pick::any());
        f32Ref = static_cast<F32>(STest::Pick::any());
        b = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
        bRef = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
      }

      U32 u32;
      U32 u32Ref;
      F32 f32;
      F32 f32Ref;
      bool b;
      bool bRef;
    };

    // ----------------------------------------------------------------------
    // Return types
    // ----------------------------------------------------------------------

    struct BoolReturn {
      BoolReturn() {
        val = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
      }

      bool val;
    };

    struct PrimitiveReturn {
      PrimitiveReturn() {
        val = STest::Pick::any();
      }

      U32 val;
    };

    // ----------------------------------------------------------------------
    // Typedefs
    // ----------------------------------------------------------------------

    typedef PortType<Empty, Empty> NoArgsPort;
    typedef PortType<PrimitiveArgs, Empty> PrimitiveArgsPort;

    typedef PortType<Empty, BoolReturn> NoArgsReturnPort;
    typedef PortType<PrimitiveArgs, PrimitiveReturn> PrimitiveReturnPort;

  } // namespace Port

} // namespace FppTest
  
#endif
