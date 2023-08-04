// ======================================================================
// \title  Drain.hpp
// \author bocchino, mereweth
// \brief  Test drain mode
//
// \copyright
// Copyright (c) 2017 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Svc_Drain_HPP
#define Svc_Drain_HPP

#include "Tester.hpp"

namespace Svc {

namespace Drain {

class Tester : public Svc::Tester {
 public:
  // ----------------------------------------------------------------------
  // Tests
  // ----------------------------------------------------------------------

  //! Send some buffers
  void OK(void);

  //! Run PartialDrain command in nominal way
  void PartialDrainOK(void);
};

}  // namespace Drain

}  // namespace Svc

#endif
