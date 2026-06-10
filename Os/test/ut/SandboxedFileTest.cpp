// ======================================================================
// \title Os/test/ut/SandboxedFileTest.cpp
// \brief Unit tests for Os::SandboxedFile
// ======================================================================
#include <gtest/gtest.h>
#include <Os/FileSystem.hpp>
#include <Os/SandboxedFile.hpp>
#include <cstdio>
#include <cstring>

// ======================================================================
// SandboxedFile tests
// ======================================================================

class SandboxedFileTest : public ::testing::Test {
  protected:
    void SetUp() override { Os::FileSystem::createDirectory("/tmp/sandbox_test/"); }
    void TearDown() override {
        Os::FileSystem::removeFile("/tmp/sandbox_test/test_file.bin");
        Os::FileSystem::removeDirectory("/tmp/sandbox_test/");
    }
};

TEST_F(SandboxedFileTest, ConfigureValid) {
    Os::SandboxedFile file;
    ASSERT_TRUE(file.isConfigured());  // default is "/"
    file.configure("/tmp/sandbox_test/");
    ASSERT_TRUE(file.isConfigured());
    ASSERT_STREQ("/tmp/sandbox_test/", file.getSandboxDirectory());
}

TEST_F(SandboxedFileTest, OpenWithinSandbox) {
    Os::SandboxedFile file;
    file.configure("/tmp/sandbox_test/");
    auto status = file.open("/tmp/sandbox_test/test_file.bin", Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::OP_OK, status);
    ASSERT_TRUE(file.isOpen());
    file.close();
}

TEST_F(SandboxedFileTest, OpenOutsideSandboxRejected) {
    Os::SandboxedFile file;
    file.configure("/tmp/sandbox_test/");
    auto status = file.open("/tmp/outside_sandbox.bin", Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::NO_PERMISSION, status);
    ASSERT_FALSE(file.isOpen());
}

TEST_F(SandboxedFileTest, TraversalAttackRejected) {
    Os::SandboxedFile file;
    file.configure("/tmp/sandbox_test/");
    auto status = file.open("/tmp/sandbox_test/../../etc/passwd", Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::NO_PERMISSION, status);
    ASSERT_FALSE(file.isOpen());
}

TEST_F(SandboxedFileTest, DefaultConfigAllowsAnyAbsolutePath) {
    Os::SandboxedFile file;
    // Default sandbox is "/" — any absolute path is allowed
    auto status = file.open("/tmp/sandbox_test/test_file.bin", Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::OP_OK, status);
    ASSERT_TRUE(file.isOpen());
    file.close();
}

TEST_F(SandboxedFileTest, WriteAndRead) {
    Os::SandboxedFile file;
    file.configure("/tmp/sandbox_test/");

    // Write data
    auto status = file.open("/tmp/sandbox_test/test_file.bin", Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::OP_OK, status);
    const U8 writeData[] = {0x01, 0x02, 0x03, 0x04};
    FwSizeType writeSize = sizeof(writeData);
    status = file.write(writeData, writeSize, Os::File::WAIT);
    ASSERT_EQ(Os::File::OP_OK, status);
    ASSERT_EQ(sizeof(writeData), writeSize);
    file.close();

    // Read data back
    status = file.open("/tmp/sandbox_test/test_file.bin", Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::OP_OK, status);
    U8 readData[sizeof(writeData)];
    FwSizeType readSize = sizeof(readData);
    status = file.read(readData, readSize, Os::File::WAIT);
    ASSERT_EQ(Os::File::OP_OK, status);
    ASSERT_EQ(sizeof(writeData), readSize);
    ASSERT_EQ(0, std::memcmp(writeData, readData, sizeof(writeData)));
    file.close();
}

TEST_F(SandboxedFileTest, GetSandboxDirectoryDefault) {
    Os::SandboxedFile file;
    ASSERT_STREQ("/", file.getSandboxDirectory());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
