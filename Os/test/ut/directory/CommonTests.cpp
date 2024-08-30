// ======================================================================
// \title Os/test/ut/directory/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/directory/CommonTests.hpp"

#include <Os/FileSystem.hpp> // for setup
// ----------------------------------------------------------------------
// Test Fixture
// ----------------------------------------------------------------------

std::unique_ptr<Os::Test::Directory::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::Directory::Tester>(new Os::Test::Directory::Tester());
}

Functionality::Functionality() : tester(get_tester_implementation()) {}

void Functionality::SetUp() {
    Os::FileSystem::createDirectory("./test_directory");
    Os::FileSystem::touch("./test_directory/test_file");
}

void Functionality::TearDown() {
    // No teardown required
}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

// Lock then unlock directory
TEST_F(Functionality, OpenDirectory) {
    Os::Directory dir;
    dir.open("./test_directory");
    char filename [256];
    dir.read(filename, 256);
    dir.close();

}

