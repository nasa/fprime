// ----------------------------------------------------------------------
// PosixTimeTester.hpp
// ----------------------------------------------------------------------

#ifndef POSIXTIME_TESTER_HPP
#define POSIXTIME_TESTER_HPP

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

      PosixTimeTester(const char *const compName);

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
