// ======================================================================
// \title Os/Posix/test/ut/PosixDirectoryTests.cpp
// \brief tests for posix implementation for Os::Directory
// ======================================================================
#include "Os/test/ut/directory/RulesHeaders.hpp"
#include "Os/test/ut/directory/CommonTests.hpp"
#include "Os/Posix/Task.hpp"
#include <gtest/gtest.h>
#include "STest/Scenario/Scenario.hpp"
#include "STest/Pick/Pick.hpp"
#include "Fw/Types/String.hpp"

#include <fcntl.h> // for ::open()

namespace Os {
namespace Test {
namespace Directory {


//! Maximum number of files per test directory
//! Intentionally low to have a decent probability of having an empty directory
static const FwIndexType MAX_FILES_PER_DIRECTORY = 4;
static const std::string FILENAME_PREFIX = "test_file_";
static const std::string TEST_DIRECTORY_PATH = "./test_directory";

//! Set up function as defined by the unit test implementor
void setUp(Os::Test::Directory::Tester* tester) {
    tester->m_path = TEST_DIRECTORY_PATH;
    ::mkdir(tester->m_path.c_str(), 0777);
    // Files are named test_file_0, test_file_1, ...
    std::vector<std::string> filenames;
    // fileCount can be 0 (empty directory)
    FwSizeType fileCount = STest::Pick::lowerUpper(0, MAX_FILES_PER_DIRECTORY);
    for (FwSizeType i = 0; i < fileCount; i++) {
        tester->m_filenames.push_back(FILENAME_PREFIX + std::to_string(i));
    }
    for (auto filename : tester->m_filenames) {
        ::open((tester->m_path + "/" + filename).c_str(), O_CREAT);
    }
}

//! Tear down function as defined by the unit test implementor
void tearDown(Os::Test::Directory::Tester* tester) {
    for (auto filename : tester->m_filenames) {
        ::unlink((tester->m_path + "/" + filename).c_str());
    }
    ::rmdir(tester->m_path.c_str());
}

}  // namespace Test
}  // namespace FileSystem
}  // namespace Os




// ----------------------------------------------------------------------
// Posix Test Cases:
//
// No posix-specific tests - tests are pulled in from CommonTests.cpp
// ----------------------------------------------------------------------

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
