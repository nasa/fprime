// ======================================================================
// \title  AESDecryptorTester.cpp
// \author vivi
// \brief  cpp file for AESDecryptor component test harness implementation class
// ======================================================================

#include "AESDecryptorTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AESDecryptorTester ::AESDecryptorTester()
    : AESDecryptorGTestBase("AESDecryptorTester", AESDecryptorTester::MAX_HISTORY_SIZE), component("AESDecryptor") {
    this->initComponents();
    this->connectPorts();
}

AESDecryptorTester ::~AESDecryptorTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void AESDecryptorTester ::toDo() {
    // TODO
}

}  // namespace Ccsds

}  // namespace Svc
