#ifndef SOME_STRUCT_HPP
#define SOME_STRUCT_HPP

#include <FpConfig.hpp>

extern "C" {
  typedef struct {
     U32 mem1;
     F64 mem2;
     U8 mem3;
  } SomeUserStruct;
}

#endif
