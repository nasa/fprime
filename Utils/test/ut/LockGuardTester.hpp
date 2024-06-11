// ======================================================================
// \title  Util/test/ut/LockGuardTester.hpp
// \author vwong
// \brief  hpp file for LockGuard test harness implementation class
//
// \copyright
//
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef LOCKGUARDTESTER_HPP
#define LOCKGUARDTESTER_HPP

#include "Utils/LockGuard.hpp"
#include <FpConfig.hpp>
#include "gtest/gtest.h"

namespace Utils {

  class LockGuardTester
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object LockGuardTester
      //!
      LockGuardTester();

      //! Destroy object LockGuardTester
      //!
      ~LockGuardTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      void testLocking();

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Initialize components
      //!
      void initComponents();

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

  };

} // end namespace Utils

#endif
