// ======================================================================
// \title Os/test/ut/FilePathUtilsTest.cpp
// \brief Unit tests for Os::FilePathUtils
// ======================================================================
#include <gtest/gtest.h>
#include <Fw/Types/FileNameString.hpp>
#include <Os/FilePathUtils.hpp>
#include <Os/FileSystem.hpp>

// ======================================================================
// FilePathUtils::resolvePath tests
// ======================================================================

class FilePathUtilsResolveTest : public ::testing::Test {};

TEST_F(FilePathUtilsResolveTest, AbsolutePathUnchanged) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("/data/uplink/file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/uplink/file.bin", resolved);
}

TEST_F(FilePathUtilsResolveTest, CollapseDotDot) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("/data/uplink/../other/file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/other/file.bin", resolved);
}

TEST_F(FilePathUtilsResolveTest, CollapseDot) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("/data/./uplink/./file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/uplink/file.bin", resolved);
}

TEST_F(FilePathUtilsResolveTest, DotDotAtRoot) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("/../../../etc/passwd", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/etc/passwd", resolved);
}

TEST_F(FilePathUtilsResolveTest, RelativePathWithBaseDir) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("subdir/file.bin", "/data/uplink/", resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/uplink/subdir/file.bin", resolved);
}

TEST_F(FilePathUtilsResolveTest, RelativePathWithDotDot) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("../escape/file.bin", "/data/uplink/", resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/escape/file.bin", resolved);
}

TEST_F(FilePathUtilsResolveTest, DoubleSlash) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("/data//uplink///file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/uplink/file.bin", resolved);
}

TEST_F(FilePathUtilsResolveTest, NullPath) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath(nullptr, nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::INVALID_PATH, status);
}

TEST_F(FilePathUtilsResolveTest, EmptyPath) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::INVALID_PATH, status);
}

TEST_F(FilePathUtilsResolveTest, RootPath) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("/", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/", resolved);
}

TEST_F(FilePathUtilsResolveTest, TrailingSlash) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("/data/uplink/", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/uplink", resolved);
}

TEST_F(FilePathUtilsResolveTest, RelativeWithoutBaseDir) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::resolvePath("relative/path", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::INVALID_PATH, status);
}

TEST_F(FilePathUtilsResolveTest, BufferTooSmall) {
    char resolved[5];  // Too small for "/data/uplink/file.bin"
    auto status = Os::FilePathUtils::resolvePath("/data/uplink/file.bin", nullptr, resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::TOO_LONG, status);
}

TEST_F(FilePathUtilsResolveTest, StringBaseOverload) {
    Fw::FileNameString path("/data/./uplink/../uplink/file.bin");
    Fw::FileNameString baseDir("/");
    Fw::FileNameString resolved;
    auto status = Os::FilePathUtils::resolvePath(path, baseDir, resolved);
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/uplink/file.bin", resolved.toChar());
}

// ======================================================================
// FilePathUtils::isSubDirectory tests
// ======================================================================

class FilePathUtilsSubDirTest : public ::testing::Test {};

TEST_F(FilePathUtilsSubDirTest, PathInsideSandbox) {
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
}

TEST_F(FilePathUtilsSubDirTest, PathInSubdirectory) {
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink/subdir/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
}

TEST_F(FilePathUtilsSubDirTest, PathOutsideSandbox) {
    auto status = Os::FilePathUtils::isSubDirectory("/data/other/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathUtilsSubDirTest, PathIsPrefixButNotDirectory) {
    // "/data/uplink_extra/file" should NOT match sandbox "/data/uplink/"
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink_extra/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathUtilsSubDirTest, PathEqualsDirectory) {
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
}

TEST_F(FilePathUtilsSubDirTest, PathAtRoot) {
    auto status = Os::FilePathUtils::isSubDirectory("/etc/passwd", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathUtilsSubDirTest, TraversalAttack) {
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink/../../etc/passwd", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathUtilsSubDirTest, RelativeTraversalAttack) {
    // Relative paths are now resolved against CWD. Unless CWD is within
    // /data/uplink/, a relative ../../ path should be outside sandbox.
    // Since test CWD is unlikely to be /data/uplink/, this should fail.
    auto status = Os::FilePathUtils::isSubDirectory("../../etc/passwd", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathUtilsSubDirTest, DotDotThenBackIn) {
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink/../uplink/file.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
}

TEST_F(FilePathUtilsSubDirTest, NullPath) {
    auto status = Os::FilePathUtils::isSubDirectory(nullptr, "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::INVALID_PATH, status);
}

TEST_F(FilePathUtilsSubDirTest, NullDirectory) {
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink/file.bin", nullptr);
    ASSERT_EQ(Os::FilePathUtils::INVALID_PATH, status);
}

TEST_F(FilePathUtilsSubDirTest, NonNormalizedAllowedDirectory) {
    // allowedDirectory with `.` segment should still work after normalization
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink/file.bin", "/data/./uplink/");
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
}

TEST_F(FilePathUtilsSubDirTest, DotDotInAllowedDirectory) {
    // allowedDirectory with `..` segment should still work after normalization
    auto status = Os::FilePathUtils::isSubDirectory("/data/uplink/file.bin", "/data/../data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
}

TEST_F(FilePathUtilsSubDirTest, RelativePathResolvedAgainstCwd) {
    // Get CWD, then construct a relative path that should resolve inside sandbox "/"
    char cwd[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto fsStatus = Os::FileSystem::getWorkingDirectory(cwd, sizeof(cwd));
    ASSERT_EQ(Os::FileSystem::Status::OP_OK, fsStatus);

    // With sandbox "/", any resolved path should be valid
    auto status = Os::FilePathUtils::isSubDirectory("somefile.bin", "/");
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
}

TEST_F(FilePathUtilsSubDirTest, StringBaseOverload) {
    Fw::FileNameString path("/data/uplink/file.bin");
    Fw::FileNameString dir("/data/uplink/");
    auto status = Os::FilePathUtils::isSubDirectory(path, dir);
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
