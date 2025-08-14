// ----------------------------------------------------------------------
// PosixTimeTester.hpp
// ----------------------------------------------------------------------

#ifndef POSIX_TIME_TESTER_HPP
#define POSIX_TIME_TESTER_HPP

#include "PosixTimeGTestBase.hpp"
#include "Svc/PosixTime/PosixTime.hpp"

namespace Svc {

class PosixTimeTester : public PosixTimeGTestBase {
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

  public:
    explicit PosixTimeTester(const char* const compName);

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

}  // namespace Svc

#endif
