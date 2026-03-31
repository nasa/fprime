#include <Os/FileUtils.hpp>
#include <Fw/Types/String.hpp>
#include "gtest/gtest.h"

#include <cstring>

// -------------------------------------------------------
// Tests for char* overload
// -------------------------------------------------------

TEST(FileUtils, ResolveSimplePath) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/my/folder/something", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/my/folder/something");
}

TEST(FileUtils, ResolveRootPath) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/");
}

TEST(FileUtils, ResolveDotDotFromRoot) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/../../", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/");
}

TEST(FileUtils, ResolveDotDotInMiddle) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/my/folder/../something", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/my/something");
}

TEST(FileUtils, ResolveDotInMiddle) {
    char result[256];
    Os::FileUtils::Status status =
        Os::FileUtils::resolveUnixPath("/my/./folder/./something", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/my/folder/something");
}

TEST(FileUtils, ResolveDotDotBeyondRoot) {
    char result[256];
    Os::FileUtils::Status status =
        Os::FileUtils::resolveUnixPath("/a/b/../../../../c", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/c");
}

TEST(FileUtils, ResolveDoubleSlash) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/my//folder///something", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/my/folder/something");
}

TEST(FileUtils, ResolveTrailingSlash) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/my/folder/", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/my/folder");
}

TEST(FileUtils, ResolveComplexPath) {
    char result[256];
    Os::FileUtils::Status status =
        Os::FileUtils::resolveUnixPath("/a/b/c/../../d/./e/../f", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/a/d/f");
}

TEST(FileUtils, ResolveDotDotAtEnd) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/a/b/..", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/a");
}

TEST(FileUtils, ResolveDotAtEnd) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/a/b/.", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/a/b");
}

TEST(FileUtils, ResolveAllDotDotToRoot) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/a/..", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/");
}

TEST(FileUtils, InvalidRelativePath) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("relative/path", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::INVALID_PATH);
}

TEST(FileUtils, InvalidEmptyPath) {
    char result[256];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::INVALID_PATH);
}

TEST(FileUtils, BufferTooSmall) {
    char result[5];  // Too small for "/my/folder"
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/my/folder", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::BUFFER_TOO_SMALL);
}

TEST(FileUtils, BufferExactSize) {
    // "/a" needs 3 bytes: '/', 'a', '\0'
    char result[3];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/a", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/a");
}

TEST(FileUtils, BufferMinimumForRoot) {
    // "/" needs 2 bytes: '/', '\0'
    char result[2];
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath("/", result, sizeof(result));
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(result, "/");
}

// -------------------------------------------------------
// Tests for Fw::String overload
// -------------------------------------------------------

TEST(FileUtils, FwStringResolveSimple) {
    Fw::String source("/my/folder/something");
    Fw::String destination;
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath(source, destination);
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(destination.toChar(), "/my/folder/something");
}

TEST(FileUtils, FwStringResolveDotDot) {
    Fw::String source("/my/folder/../something");
    Fw::String destination;
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath(source, destination);
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(destination.toChar(), "/my/something");
}

TEST(FileUtils, FwStringResolveDot) {
    Fw::String source("/my/./folder/./something");
    Fw::String destination;
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath(source, destination);
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(destination.toChar(), "/my/folder/something");
}

TEST(FileUtils, FwStringResolveDotDotFromRoot) {
    Fw::String source("/../../");
    Fw::String destination;
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath(source, destination);
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(destination.toChar(), "/");
}

TEST(FileUtils, FwStringResolveComplex) {
    Fw::String source("/a/b/c/../../d/./e/../f");
    Fw::String destination;
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath(source, destination);
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(destination.toChar(), "/a/d/f");
}

TEST(FileUtils, FwStringResolveRoot) {
    Fw::String source("/");
    Fw::String destination;
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath(source, destination);
    EXPECT_EQ(status, Os::FileUtils::OP_OK);
    EXPECT_STREQ(destination.toChar(), "/");
}

TEST(FileUtils, FwStringInvalidRelativePath) {
    Fw::String source("relative/path");
    Fw::String destination;
    Os::FileUtils::Status status = Os::FileUtils::resolveUnixPath(source, destination);
    EXPECT_EQ(status, Os::FileUtils::INVALID_PATH);
}

extern "C" {
void fileUtilsTest();
}

void fileUtilsTest() {
    // This function intentionally left empty.
    // All tests are run via the GoogleTest framework.
}
