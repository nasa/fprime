// ======================================================================
// \title  Util/test/ut/TokenBucketTester.hpp
// \author vwong
// \brief  hpp file for TokenBucket test harness implementation class
//
// \copyright
//
// Copyright (C) 2009-2020 California Institute of Technology.
//
// ALL RIGHTS RESERVED. United States Government Sponsorship
// acknowledged.
// ======================================================================

#ifndef TOKENBUCKETTESTER_HPP
#define TOKENBUCKETTESTER_HPP

#include "Utils/TokenBucket.hpp"
#include <FpConfig.hpp>
#include "gtest/gtest.h"

namespace Utils {

  class TokenBucketTester
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object TokenBucketTester
      //!
      TokenBucketTester();

      //! Destroy object TokenBucketTester
      //!
      ~TokenBucketTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      void testTriggering();
      void testReconfiguring();
      void testInitialSettings();

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
