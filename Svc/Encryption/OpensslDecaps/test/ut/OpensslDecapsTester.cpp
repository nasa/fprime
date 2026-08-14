// ======================================================================
// \title  OpensslDecapsTester.cpp
// \author vivi
// \brief  cpp file for OpensslDecaps component test harness implementation class
// ======================================================================

#include "OpensslDecapsTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

OpensslDecapsTester ::OpensslDecapsTester()
    : OpensslDecapsGTestBase("OpensslDecapsTester", OpensslDecapsTester::MAX_HISTORY_SIZE), component("OpensslDecaps") {
    this->initComponents();
    this->connectPorts();
}

OpensslDecapsTester ::~OpensslDecapsTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void OpensslDecapsTester ::toDo() {
    // TODO
}

}  // namespace Ccsds

}  // namespace Svc
