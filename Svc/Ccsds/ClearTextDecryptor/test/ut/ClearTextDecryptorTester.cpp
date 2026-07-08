// ======================================================================
// \title  ClearTextDecryptorTester.cpp
// \author lestarch-autobot
// \brief  cpp file for ClearTextDecryptor component test harness implementation class
// ======================================================================

#include "Svc/Ccsds/ClearTextDecryptor/test/ut/ClearTextDecryptorTester.hpp"
#include "STest/Pick/Pick.hpp"

namespace Svc {

namespace Ccsds {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

ClearTextDecryptorTester ::ClearTextDecryptorTester()
    : ClearTextDecryptorGTestBase("ClearTextDecryptorTester", ClearTextDecryptorTester::MAX_HISTORY_SIZE),
      component("ClearTextDecryptor") {
    this->initComponents();
    this->connectPorts();
}

ClearTextDecryptorTester ::~ClearTextDecryptorTester() {
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void ClearTextDecryptorTester ::testDecryptPassThrough() {
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

    const Svc::Ccsds::SdlsStatus status = this->invoke_to_decryptIn(0, sa, buffer, context);

    ASSERT_EQ(status, Svc::Ccsds::SdlsStatus::SUCCESS);
    ASSERT_from_decryptOut_SIZE(1);
    ASSERT_from_decryptOut(0, buffer, context);
    ASSERT_from_bufferReturnOut_SIZE(0);
    ASSERT_EQ(::memcmp(storage, original, TEST_BUFFER_SIZE), 0);
}

void ClearTextDecryptorTester ::testReturnPassThrough() {
    this->clearHistory();

    U8 storage[TEST_BUFFER_SIZE];
    Fw::Buffer buffer(storage, sizeof storage);
    ComCfg::FrameContext context;

    this->invoke_to_decryptReturnIn(0, buffer, context);

    ASSERT_from_bufferReturnOut_SIZE(1);
    ASSERT_from_bufferReturnOut(0, buffer, context);
    ASSERT_from_decryptOut_SIZE(0);
}

}  // namespace Ccsds

}  // namespace Svc
