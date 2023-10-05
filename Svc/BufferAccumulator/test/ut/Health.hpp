// ======================================================================
// \title  Health.hpp
// \author bocchino, mereweth
// \brief  Interface for Buffer Accumulator health tests
//
// \copyright
// Copyright (C) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Health_HPP
#define Svc_Health_HPP

#include "BufferAccumulatorTester.hpp"

namespace Svc {

namespace Health {

class BufferAccumulatorTester : public Svc::BufferAccumulatorTester {
 public:
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  //! Health ping test
  void Ping(void);
};

}  // namespace Health

}  // namespace Svc

#endif
