// ======================================================================
// \title Os/Stub/test/ut/StubDirectoryTests.cpp
// \brief stub implementation for Os::DirectoryInterface testing
// This ensures the delegation of function calls happens properly
// ======================================================================
#include <gtest/gtest.h>
#include "Os/test/ut/directory/CommonTests.hpp"
#include "Os/test/ut/directory/RulesHeaders.hpp"
#include "Os/Stub/test/Directory.hpp"
#include "Os/FileSystem.hpp"

using namespace Os::Stub::Directory::Test;


// Basic file tests
class Interface : public ::testing::Test {
public:
    //! Setup function delegating to UT setUp function
    void SetUp() override {
        StaticData::data = StaticData();
    }

    //! Setup function delegating to UT tearDown function
    void TearDown() override {
        StaticData::data = StaticData();
    }
};


// Ensure that Os::Directory properly calls the implementation constructor
TEST_F(Interface, Construction) {
    Os::Directory directory;
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CONSTRUCT_FN);
}

// Ensure that Os::Directory properly calls the implementation destructor
TEST_F(Interface, Destruction) {
    delete (new Os::Directory);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::DESTRUCT_FN);
}

// Ensure that Os::Directory properly calls the implementation open()
TEST_F(Interface, Open) {
    Os::Directory directory;
    directory.open("/does/not/matter", Os::Directory::OpenMode::CREATE_IF_MISSING);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::OPEN_FN);
}

// Ensure that Os::Directory properly calls the implementation rewind()
TEST_F(Interface, Rewind) {
    Os::Directory directory;
    directory.open("/does/not/matter", Os::Directory::OpenMode::READ);
    directory.rewind();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::REWIND_FN);
}

// Ensure that Os::Directory properly calls the implementation open()
TEST_F(Interface, Read) {
    Os::Directory directory;
    directory.open("/does/not/matter", Os::Directory::OpenMode::READ);
    char buffer[4];
    directory.read(buffer, 4);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::READ_FN);
}

// Ensure that Os::Directory properly calls the implementation open()
TEST_F(Interface, Close) {
    Os::Directory directory;
    directory.close();
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CLOSE_FN);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
