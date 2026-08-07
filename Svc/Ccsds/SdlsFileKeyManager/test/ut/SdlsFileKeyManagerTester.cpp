// ======================================================================
// \title  SdlsFileKeyManagerTester.cpp
// \author lestarch-autobot
// \brief  cpp file for SdlsFileKeyManager component test harness implementation class
// ======================================================================

#include "Svc/Ccsds/SdlsFileKeyManager/test/ut/SdlsFileKeyManagerTester.hpp"
#include <cstring>
#include "Os/File.hpp"
#include "Os/FileSystem.hpp"
#include "STest/Pick/Pick.hpp"

namespace Svc {

namespace Ccsds {

static const char* const TEST_KEY_FILE = "sdls_key_test.bin";
static const char* const TEST_MISSING_FILE = "sdls_key_test_missing.bin";

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

SdlsFileKeyManagerTester ::SdlsFileKeyManagerTester()
    : SdlsFileKeyManagerGTestBase("SdlsFileKeyManagerTester", SdlsFileKeyManagerTester::MAX_HISTORY_SIZE),
      component("SdlsFileKeyManager"),
      m_keyData() {
    this->initComponents();
    this->connectPorts();
}

SdlsFileKeyManagerTester ::~SdlsFileKeyManagerTester() {
    (void)Os::FileSystem::removeFile(TEST_KEY_FILE);
    this->component.deinit();
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void SdlsFileKeyManagerTester ::testNominalRead() {
    this->clearHistory();

    const FwSizeType keySize = static_cast<FwSizeType>(STest::Pick::lowerUpper(1, SdlsCfg::MAX_SDLS_KEY_SIZE));
    this->writeKeyFile(TEST_KEY_FILE, keySize);
    this->component.configure(TEST_KEY_FILE, keySize);

    SdlsKeyBuffer key;
    const SdlsStatus status = this->invoke_to_keyGet(0, key);

    ASSERT_EQ(status, SdlsStatus::SUCCESS);
    ASSERT_EQ(key.getSize(), keySize);
    ASSERT_EQ(::memcmp(key.getBuffAddr(), this->m_keyData, static_cast<size_t>(keySize)), 0);
    ASSERT_EVENTS_SIZE(0);
}

void SdlsFileKeyManagerTester ::testMissingFile() {
    this->clearHistory();

    const FwSizeType keySize = static_cast<FwSizeType>(STest::Pick::lowerUpper(1, SdlsCfg::MAX_SDLS_KEY_SIZE));
    this->component.configure(TEST_MISSING_FILE, keySize);

    // Simulate a reused buffer holding a previously-read key
    SdlsKeyBuffer key;
    ::memset(key.getBuffAddr(), 0xAB, static_cast<size_t>(key.getCapacity()));
    ASSERT_EQ(key.setBuffLen(keySize), Fw::FW_SERIALIZE_OK);

    const SdlsStatus status = this->invoke_to_keyGet(0, key);

    ASSERT_EQ(status, SdlsStatus::KEY_ERROR);
    ASSERT_EQ(key.getSize(), 0);
    for (FwSizeType i = 0; i < key.getCapacity(); i++) {
        ASSERT_EQ(key.getBuffAddr()[i], 0);
    }
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_KeyReadFailed_SIZE(1);
    ASSERT_EVENTS_KeyReadFailed(0, static_cast<I32>(Os::File::DOESNT_EXIST), 0, keySize);
}

void SdlsFileKeyManagerTester ::testShortFile() {
    this->clearHistory();

    const FwSizeType keySize = static_cast<FwSizeType>(STest::Pick::lowerUpper(2, SdlsCfg::MAX_SDLS_KEY_SIZE));
    const FwSizeType fileSize = static_cast<FwSizeType>(STest::Pick::lowerUpper(1, static_cast<U32>(keySize - 1)));
    this->writeKeyFile(TEST_KEY_FILE, fileSize);
    this->component.configure(TEST_KEY_FILE, keySize);

    SdlsKeyBuffer key;
    const SdlsStatus status = this->invoke_to_keyGet(0, key);

    ASSERT_EQ(status, SdlsStatus::KEY_ERROR);
    ASSERT_EQ(key.getSize(), 0);
    ASSERT_EVENTS_SIZE(1);
    ASSERT_EVENTS_KeyReadFailed_SIZE(1);
    ASSERT_EVENTS_KeyReadFailed(0, static_cast<I32>(Os::File::OP_OK), fileSize, keySize);
}

// ----------------------------------------------------------------------
// Helper functions
// ----------------------------------------------------------------------

void SdlsFileKeyManagerTester ::writeKeyFile(const char* path, FwSizeType size) {
    for (FwSizeType i = 0; i < size; i++) {
        this->m_keyData[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 0xFF));
    }
    Os::File file;
    ASSERT_EQ(file.open(path, Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    FwSizeType writeSize = size;
    ASSERT_EQ(file.write(this->m_keyData, writeSize, Os::File::WaitType::WAIT), Os::File::OP_OK);
    ASSERT_EQ(writeSize, size);
}

}  // namespace Ccsds

}  // namespace Svc
