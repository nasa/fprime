// ======================================================================
// \title  Accumulate.hpp
// \author bocchino, mereweth
// \brief  Test accumulate mode
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Accumulate_HPP
#define Svc_Accumulate_HPP

#include "BufferAccumulatorTester.hpp"

namespace Svc {

namespace Accumulate {

class BufferAccumulatorTester : public Svc::BufferAccumulatorTester {
 public:
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  //! Send some buffers
  void OK(void);
};

}  // namespace Accumulate

}  // namespace Svc

#endif
