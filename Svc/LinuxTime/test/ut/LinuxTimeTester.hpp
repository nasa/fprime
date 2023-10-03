// ----------------------------------------------------------------------
// LinuxTimeTester.hpp
// ----------------------------------------------------------------------

#ifndef TESTER_HPP
#define TESTER_HPP

#include "../../LinuxTimeImpl.hpp"
#include "TimeGTestBase.hpp"

namespace Svc {

  class LinuxTimeTester :
    public TimeGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction
      // ----------------------------------------------------------------------

    public:

      LinuxTimeTester(const char *const compName);

      ~LinuxTimeTester();

      // ----------------------------------------------------------------------
      // Tests
      // ----------------------------------------------------------------------

    public:

      void getTime();

      // ----------------------------------------------------------------------
      // The component under test
      // ----------------------------------------------------------------------

    private:

      LinuxTimeImpl linuxTime;

  };

};

#endif
