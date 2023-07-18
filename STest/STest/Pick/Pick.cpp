// ======================================================================
// \title  Pick.cpp
// \author bocchino
// \brief  Pick implementation
//
// \copyright
// Copyright (C) 2022 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include "STest/Pick/Pick.hpp"

namespace STest {

  namespace Pick {

    U32 any() {
      return lowerUpper(0, 0xFFFFFFFFU);
    }

  }

}
