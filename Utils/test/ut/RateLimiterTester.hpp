// ======================================================================
// \title  Util/test/ut/RateLimiterTester.hpp
// \author vwong
// \brief  hpp file for RateLimiter test harness implementation class
//
// \copyright
//
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef RATELIMITERTESTER_HPP
#define RATELIMITERTESTER_HPP

#include "Utils/RateLimiter.hpp"
#include <FpConfig.hpp>
#include "gtest/gtest.h"

namespace Utils {

  class RateLimiterTester
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object RateLimiterTester
      //!
      RateLimiterTester();

      //! Destroy object RateLimiterTester
      //!
      ~RateLimiterTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      void testCounterTriggering();
      void testTimeTriggering();
      void testCounterAndTimeTriggering();

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
