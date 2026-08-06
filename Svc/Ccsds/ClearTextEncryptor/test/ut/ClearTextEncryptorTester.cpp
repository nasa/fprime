// ======================================================================
// \title  ClearTextEncryptorTester.cpp
// \author lestarch-autobot
// \brief  cpp file for ClearTextEncryptor component test harness implementation class
// ======================================================================

#include "Svc/Ccsds/ClearTextEncryptor/test/ut/ClearTextEncryptorTester.hpp"
#include "STest/Pick/Pick.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ClearTextEncryptorTester ::ClearTextEncryptorTester()
    : ClearTextEncryptorGTestBase("ClearTextEncryptorTester", ClearTextEncryptorTester::MAX_HISTORY_SIZE),
      component("ClearTextEncryptor") {
    this->initComponents();
    this->connectPorts();
}

ClearTextEncryptorTester ::~ClearTextEncryptorTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ClearTextEncryptorTester ::testEncryptPassThrough() {
    this->clearHistory();

    const U16 sa = static_cast<U16>(STest::Pick::lowerUpper(0, 0xFFFF));
    U8 storage[TEST_BUFFER_SIZE];
    for (FwSizeType i = 0; i < TEST_BUFFER_SIZE; i++) {
        storage[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
    }
    U8 original[TEST_BUFFER_SIZE];
    (void)::memcpy(original, storage, TEST_BUFFER_SIZE);
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;
    context.set_vcId(static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF)));

    this->invoke_to_encryptIn(0, sa, buffer, context);

    ASSERT_from_encryptOut_SIZE(1);
    ASSERT_from_encryptOut(0, Svc::Ccsds::SdlsStatus::SUCCESS, buffer, context);
    ASSERT_from_bufferReturnOut_SIZE(0);
    ASSERT_EQ(::memcmp(storage, original, TEST_BUFFER_SIZE), 0);
}

void ClearTextEncryptorTester ::testReturnPassThrough() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_encryptReturnIn(0, buffer, context);

    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_from_bufferReturnOut(0, buffer, context);
    ASSERT_from_encryptOut_SIZE(0);
}

}  // namespace Ccsds

}  // namespace Svc
