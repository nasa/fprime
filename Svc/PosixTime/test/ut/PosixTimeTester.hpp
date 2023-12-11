// ----------------------------------------------------------------------
// PosixTimeTester.hpp
// ----------------------------------------------------------------------

#ifndef POSIX_TIME_TESTER_HPP
#define POSIX_TIME_TESTER_HPP

#include "Svc/PosixTime/PosixTime.hpp"
#include "PosixTimeGTestBase.hpp"

namespace Svc {

  class PosixTimeTester :
    public PosixTimeGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      explicit PosixTimeTester(const char *const compName);

      ~PosixTimeTester();

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

    public:

      void getTime();

      // ----------------------------------------------------------------------
      // The component under test
      // ----------------------------------------------------------------------

    private:

      PosixTime component;

  };

};

#endif
