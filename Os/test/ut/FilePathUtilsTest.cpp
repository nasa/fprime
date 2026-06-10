// ======================================================================
// \title Os/test/ut/FilePathUtilsTest.cpp
// \brief Unit tests for Os::FilePathUtils
// ======================================================================
#include <gtest/gtest.h>
#include <Os/FilePathUtils.hpp>

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
    auto status = Os::FilePathUtils::isSubDirectory("../../etc/passwd", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::OUTSIDE_SANDBOX, status);
}

TEST_F(FilePathUtilsSubDirTest, ValidRelativePath) {
    auto status = Os::FilePathUtils::isSubDirectory("mission/seq.bin", "/data/uplink/");
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
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

TEST_F(FilePathUtilsSubDirTest, OutputsResolvedPath) {
    char resolved[Os::FilePathUtils::MAX_PATH_LENGTH];
    auto status = Os::FilePathUtils::isSubDirectory("mission/seq.bin", "/data/uplink/", resolved, sizeof(resolved));
    ASSERT_EQ(Os::FilePathUtils::VALID, status);
    ASSERT_STREQ("/data/uplink/mission/seq.bin", resolved);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
