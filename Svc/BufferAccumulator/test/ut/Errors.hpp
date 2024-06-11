// ======================================================================
// \title  Errors.hpp
// \author bocchino, mereweth
// \brief  Test errors
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Errors_HPP
#define Svc_Errors_HPP

#include "BufferAccumulatorTester.hpp"

namespace Svc {

namespace Errors {

class BufferAccumulatorTester : public Svc::BufferAccumulatorTester {
 public:
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  //! Queue full
  void QueueFull(void);

  //! Run PartialDrain command in off-nominal ways
  void PartialDrain(void);
};

}  // namespace Errors

}  // namespace Svc

#endif
