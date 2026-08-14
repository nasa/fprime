// ======================================================================
// \title  AESEncryptorTester.cpp
// \author vivi
// \brief  cpp file for AESEncryptor component test harness implementation class
// ======================================================================

#include "AESEncryptorTester.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

AESEncryptorTester ::AESEncryptorTester()
    : AESEncryptorGTestBase("AESEncryptorTester", AESEncryptorTester::MAX_HISTORY_SIZE), component("AESEncryptor") {
    this->initComponents();
    this->connectPorts();
}

AESEncryptorTester ::~AESEncryptorTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void AESEncryptorTester ::toDo() {
    // TODO
}

}  // namespace Ccsds

}  // namespace Svc
