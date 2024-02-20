// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#ifndef TESTER_HPP
#define TESTER_HPP

#include "ActiveTextLoggerGTestBase.hpp"
#include "Svc/ActiveTextLogger/ActiveTextLogger.hpp"

namespace Svc {

  class ActiveTextLoggerTester :
    public ActiveTextLoggerGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      //! Construct object ActiveTextLoggerTester
      //!
      ActiveTextLoggerTester();

      //! Destroy object ActiveTextLoggerTester
      //!
      ~ActiveTextLoggerTester();

    public:

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

      void run_nominal_test();
      void run_off_nominal_test();

    private:

      // ----------------------------------------------------------------------
      // Helper methods
      // ----------------------------------------------------------------------

      //! Connect ports
      //!
      void connectPorts();

      //! Initialize components
      //!
      void initComponents();

    private:

      // ----------------------------------------------------------------------
      // Variables
      // ----------------------------------------------------------------------

      //! The component under test
      //!
      ActiveTextLogger component;

  };

} // end namespace Svc

#endif
