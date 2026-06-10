// ======================================================================
// \title Os/test/ut/SandboxedFileTest.cpp
// \brief Unit tests for Os::FilePathValidator and Os::SandboxedFile
// ======================================================================
#include <gtest/gtest.h>
#include <Os/FilePathValidator.hpp>
#include <Os/FileSystem.hpp>
#include <Os/SandboxedFile.hpp>
#include <cstdio>
#include <cstring>

// ======================================================================
// FilePathValidator::resolvePath tests
// ======================================================================

class FilePathValidatorResolveTest : public ::testing::Test {};

TEST_F(FilePathValidatorResolveTest, AbsolutePathUnchanged) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("/data/uplink/file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/data/uplink/file.bin", resolved);
}

TEST_F(FilePathValidatorResolveTest, CollapseDotDot) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status =
        Os::FilePathValidator::resolvePath("/data/uplink/../other/file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/data/other/file.bin", resolved);
}

TEST_F(FilePathValidatorResolveTest, CollapseDot) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("/data/./uplink/./file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/data/uplink/file.bin", resolved);
}

TEST_F(FilePathValidatorResolveTest, DotDotAtRoot) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("/../../../etc/passwd", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/etc/passwd", resolved);
}

TEST_F(FilePathValidatorResolveTest, RelativePathWithBaseDir) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("subdir/file.bin", "/data/uplink/", resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/data/uplink/subdir/file.bin", resolved);
}

TEST_F(FilePathValidatorResolveTest, RelativePathWithDotDot) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("../escape/file.bin", "/data/uplink/", resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/data/escape/file.bin", resolved);
}

TEST_F(FilePathValidatorResolveTest, DoubleSlash) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("/data//uplink///file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/data/uplink/file.bin", resolved);
}

TEST_F(FilePathValidatorResolveTest, NullPath) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath(nullptr, nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::INVALID_PATH, status);
}

TEST_F(FilePathValidatorResolveTest, EmptyPath) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::INVALID_PATH, status);
}

TEST_F(FilePathValidatorResolveTest, RootPath) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("/", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/", resolved);
}

TEST_F(FilePathValidatorResolveTest, TrailingSlash) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("/data/uplink/", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
    ASSERT_STREQ("/data/uplink", resolved);
}

TEST_F(FilePathValidatorResolveTest, RelativeWithoutBaseDir) {
    char resolved[Os::FilePathValidator::MAX_PATH_LENGTH];
    auto status = Os::FilePathValidator::resolvePath("relative/path", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathValidator::INVALID_PATH, status);
}

// ======================================================================
// FilePathValidator::checkContainment tests
// ======================================================================

class FilePathValidatorContainmentTest : public ::testing::Test {};

TEST_F(FilePathValidatorContainmentTest, PathInsideSandbox) {
    auto status = Os::FilePathValidator::checkContainment("/data/uplink/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
}

TEST_F(FilePathValidatorContainmentTest, PathInSubdirectory) {
    auto status = Os::FilePathValidator::checkContainment("/data/uplink/subdir/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
}

TEST_F(FilePathValidatorContainmentTest, PathOutsideSandbox) {
    auto status = Os::FilePathValidator::checkContainment("/data/other/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathValidatorContainmentTest, PathIsPrefixButNotDirectory) {
    // "/data/uplink_extra/file" should NOT match sandbox "/data/uplink/"
    auto status = Os::FilePathValidator::checkContainment("/data/uplink_extra/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathValidatorContainmentTest, PathEqualsDirectory) {
    auto status = Os::FilePathValidator::checkContainment("/data/uplink", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
}

TEST_F(FilePathValidatorContainmentTest, PathAtRoot) {
    auto status = Os::FilePathValidator::checkContainment("/etc/passwd", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathValidatorContainmentTest, AllowedDirNoTrailingSlash) {
    // Allowed dir must end with '/'
    auto status = Os::FilePathValidator::checkContainment("/data/uplink/file.bin", "/data/uplink");
    ASSERT_EQ(Os::FilePathValidator::OUTSIDE_SANDBOX, status);
}

// ======================================================================
// FilePathValidator::validatePath integration tests
// ======================================================================

class FilePathValidatorValidateTest : public ::testing::Test {};

TEST_F(FilePathValidatorValidateTest, ValidAbsolutePath) {
    auto status = Os::FilePathValidator::validatePath("/data/uplink/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
}

TEST_F(FilePathValidatorValidateTest, TraversalAttack) {
    auto status = Os::FilePathValidator::validatePath("/data/uplink/../../etc/passwd", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathValidatorValidateTest, RelativeTraversalAttack) {
    auto status = Os::FilePathValidator::validatePath("../../etc/passwd", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathValidatorValidateTest, ValidRelativePath) {
    auto status = Os::FilePathValidator::validatePath("mission/seq.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
}

TEST_F(FilePathValidatorValidateTest, DotDotThenBackIn) {
    // /data/uplink/../uplink/file.bin resolves to /data/uplink/file.bin -> valid
    auto status = Os::FilePathValidator::validatePath("/data/uplink/../uplink/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::VALID, status);
}

TEST_F(FilePathValidatorValidateTest, NullPath) {
    auto status = Os::FilePathValidator::validatePath(nullptr, "/data/uplink/");
    ASSERT_EQ(Os::FilePathValidator::INVALID_PATH, status);
}

TEST_F(FilePathValidatorValidateTest, NullDirectory) {
    auto status = Os::FilePathValidator::validatePath("/data/uplink/file.bin", nullptr);
    ASSERT_EQ(Os::FilePathValidator::INVALID_PATH, status);
}

// ======================================================================
// SandboxedFile tests
// ======================================================================

class SandboxedFileTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Create a temporary sandbox directory for testing
        Os::FileSystem::createDirectory("/tmp/sandbox_test/");
    }
    void TearDown() override {
        Os::FileSystem::removeFile("/tmp/sandbox_test/test_file.bin");
        Os::FileSystem::removeDirectory("/tmp/sandbox_test/");
    }
};

TEST_F(SandboxedFileTest, ConfigureValid) {
    Os::SandboxedFile file;
    ASSERT_FALSE(file.isConfigured());
    ASSERT_TRUE(file.configure("/tmp/sandbox_test/"));
    ASSERT_TRUE(file.isConfigured());
    ASSERT_STREQ("/tmp/sandbox_test/", file.getSandboxDirectory());
}

TEST_F(SandboxedFileTest, ConfigureAddsTrailingSlash) {
    Os::SandboxedFile file;
    ASSERT_TRUE(file.configure("/tmp/sandbox_test"));
    ASSERT_STREQ("/tmp/sandbox_test/", file.getSandboxDirectory());
}

TEST_F(SandboxedFileTest, ConfigureRejectsRelativePath) {
    Os::SandboxedFile file;
    ASSERT_FALSE(file.configure("relative/path"));
    ASSERT_FALSE(file.isConfigured());
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

TEST_F(SandboxedFileTest, OpenWithoutConfigureRejected) {
    Os::SandboxedFile file;
    auto status = file.open("/tmp/sandbox_test/test_file.bin", Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::NO_PERMISSION, status);
    ASSERT_FALSE(file.isOpen());
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

TEST_F(SandboxedFileTest, GetSandboxDirectoryUnconfigured) {
    Os::SandboxedFile file;
    ASSERT_EQ(nullptr, file.getSandboxDirectory());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
