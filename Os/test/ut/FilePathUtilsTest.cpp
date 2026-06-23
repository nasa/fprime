// ======================================================================
// \title Os/test/ut/FilePathUtilsTest.cpp
// \brief Unit tests for Os::FilePathUtils
// ======================================================================
#include <gtest/gtest.h>
#include <Fw/Types/FileNameString.hpp>
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

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
