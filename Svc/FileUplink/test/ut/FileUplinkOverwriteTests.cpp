// ======================================================================
// \title  FileUplinkOverwriteTests.cpp
// \brief  TEST_F entries for overwrite truncation regression tests.
//
// Placement: Svc/FileUplink/test/ut/FileUplinkOverwriteTests.cpp
// Add to CMakeLists.txt sources alongside the other test .cpp files.
// ======================================================================

#include "FileUplinkTester.hpp"
#include <gtest/gtest.h>

namespace Svc {

// Fixture — identical pattern to the existing FileUplink fixture.
class FileUplinkOverwrite : public ::testing::Test {
  protected:
    FileUplinkTester tester;
};

TEST_F(FileUplinkOverwrite, OverwriteWithSmallerFile) {
    tester.overwriteWithSmallerFile();
}

TEST_F(FileUplinkOverwrite, OverwriteSameSizeFile) {
    tester.overwriteSameSizeFile();
}

TEST_F(FileUplinkOverwrite, OverwriteWithLargerFile) {
    tester.overwriteWithLargerFile();
}

}  // namespace Svc
