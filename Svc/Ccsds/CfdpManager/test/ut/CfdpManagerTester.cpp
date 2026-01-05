// ======================================================================
// \title  CfdpManagerTester.cpp
// \author campuzan
// \brief  cpp file for CfdpManager component test harness implementation class
// ======================================================================

#include "CfdpManagerTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

CfdpManagerTester ::CfdpManagerTester()
    : CfdpManagerGTestBase("CfdpManagerTester", CfdpManagerTester::MAX_HISTORY_SIZE),
      component("CfdpManager") {
    this->component.configure();
    this->initComponents();
    this->connectPorts();
}

CfdpManagerTester ::~CfdpManagerTester() { }

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void CfdpManagerTester ::testMetaDataPdu() {
    
}

}  // namespace Ccsds

}  // namespace Svc
