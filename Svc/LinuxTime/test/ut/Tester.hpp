// ----------------------------------------------------------------------
// Tester.hpp 
// ----------------------------------------------------------------------

#ifndef TESTER_HPP
#define TESTER_HPP

#include "../../LinuxTimeImpl.hpp"
#include "GTestBase.hpp"

namespace Svc {

  class Tester :
    public TimeGTestBase
  {

      // ----------------------------------------------------------------------
      // Construction and destruction 
      // ----------------------------------------------------------------------

    public:

      Tester(const char *const compName);

      ~Tester(void);

      // ----------------------------------------------------------------------
      // Tests 
      // ----------------------------------------------------------------------

    public:

      void getTime(void);

      // ----------------------------------------------------------------------
      // The component under test 
      // ----------------------------------------------------------------------

    private:

      LinuxTimeImpl linuxTime;

  };

};

#endif
