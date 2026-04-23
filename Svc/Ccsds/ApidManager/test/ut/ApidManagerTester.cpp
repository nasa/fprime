// ======================================================================
// \title  ApidManagerTester.cpp
// \author thomas-bc
// \brief  cpp file for ApidManager component test harness implementation class
// ======================================================================

#include "ApidManagerTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ApidManagerTester::ApidManagerTester()
    : ApidManagerGTestBase("ApidManagerTester", ApidManagerTester::MAX_HISTORY_SIZE), component("ApidManager") {
    this->initComponents();
    this->connectPorts();
}

ApidManagerTester::~ApidManagerTester() {}

}  // namespace Ccsds

}  // namespace Svc
