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
#include <Fw/Types/BasicTypes.hpp>
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
      TokenBucketTester(void);

      //! Destroy object TokenBucketTester
      //!
      ~TokenBucketTester(void);

    public:

      // ---------------------------------------------------------------------- 
      // Tests
      // ---------------------------------------------------------------------- 

      void testTriggering(void);
      void testReconfiguring(void);
      void testInitialSettings(void);

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Initialize components
      //!
      void initComponents(void);

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

  };

} // end namespace Utils

#endif
