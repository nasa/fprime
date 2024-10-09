// ======================================================================
// \title Os/Stub/test/ut/StubFileTests.cpp
// \brief tests using stub implementation for Os::File interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/File.hpp"
#include "Os/Stub/test/File.hpp"
#include "Os/test/ut/file/CommonTests.hpp"
#include "Os/test/ut/file/RulesHeaders.hpp"

namespace Os {
namespace Test {
namespace File {

//! Set up for the test ensures that the test can run at all
//!
void setUp(bool requires_io) {
    if (requires_io) {
        GTEST_SKIP() << "Cannot run tests requiring functional i/o";
    }
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::setPositionResult(0);
}
std::vector<std::shared_ptr<const std::string> > FILES;
//! Tear down for the tests cleans up the test file used
//!
void tearDown() {}

class StubsTester : public Tester {
    //! Check if the test file exists.
    //! \return true if it exists, false otherwise.
    //!
    bool exists(const std::string &filename) const override {
        for (size_t i = 0; i < FILES.size(); i++) {
            if (filename == *FILES.at(i)) {
                return true;
            }
        }
        return false;
    }

    //! Get a filename, randomly if random is true, otherwise use a basic filename.
    //! \param random: true if filename should be random, false if predictable
    //! \return: filename to use for testing
    //!
    std::shared_ptr<const std::string> get_filename(bool random) const override {
        std::shared_ptr<const std::string> filename = std::shared_ptr<const std::string>(new std::string("DOES-NOT-MATTER"), std::default_delete<std::string>());
        FILES.push_back(filename);
        return filename;
    }

    //! Posix tester is fully functional
    //! \return true
    //!
    bool functional() const override {
        return false;
    }

};

std::unique_ptr<Os::Test::File::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::File::Tester>(new Os::Test::File::StubsTester());
}


} // namespace File
} // namespace Test
} // namespace Os

// Ensure that Os::File properly routes constructor calls to the `constructInternal` function.
TEST_F(Interface, Construction) {
    Os::File file;
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::CONSTRUCT_FN);
}

// Ensure that Os::File properly routes destructor calls to the `destructInternal` function.
TEST_F(Interface, Destruction) {
    delete (new Os::File);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::DESTRUCT_FN);
}

// Ensure that Os::File properly routes open calls to the `openInternal` function.
TEST_F(Interface, Open) {
    const char* path = "/does/not/matter";
    Os::File file;
    ASSERT_EQ(file.open(path, Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE), Os::File::Status::OP_OK);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::OPEN_FN);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.openPath, path);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.openMode, Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.openOverwrite, Os::File::OverwriteType::OVERWRITE);
}

// Ensure that Os::File properly routes close calls to the `closeInternal` function.
TEST_F(Interface, Close) {
    Os::File file;
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    file.close();
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::CLOSE_FN);
}

// Ensure that Os::File properly routes close calls to the `sizeInternal` function.
TEST_F(Interface, Size) {
    FwSignedSizeType sizeResult = -1;
    Os::File file;
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::setSizeResult(30);
    ASSERT_EQ(file.size(sizeResult), Os::File::OP_OK);
    ASSERT_EQ(sizeResult, 30);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::SIZE_FN);
}

// Ensure that Os::File properly routes close calls to the `positionInternal` function.
TEST_F(Interface, Position) {
    FwSignedSizeType positionResult = -1;
    Os::File file;
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::setPositionResult(50);
    ASSERT_EQ(file.position(positionResult), Os::File::OP_OK);
    ASSERT_EQ(positionResult, 50);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::POSITION_FN);
}

// Ensure that Os::File properly routes preallocate calls to the `preallocateInternal` function.
TEST_F(Interface, Preallocate) {
    Os::File file;
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OTHER_ERROR);
    ASSERT_EQ(file.preallocate(0, 0), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::PREALLOCATE_FN);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.preallocateOffset, 0);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.preallocateLength, 0);

}

// Ensure that Os::File properly routes seek calls to the `seekInternal` function.
TEST_F(Interface, Seek) {
    Os::File file;
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OTHER_ERROR);
    ASSERT_EQ(file.seek(0, Os::File::SeekType::ABSOLUTE), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::SEEK_FN);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.seekOffset, 0);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.seekType, Os::File::SeekType::ABSOLUTE);
}

// Ensure that Os::File properly routes flush calls to the `flushInternal` function.
TEST_F(Interface, Flush) {
    Os::File file;
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OTHER_ERROR);
    ASSERT_EQ(file.flush(), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::FLUSH_FN);
}

// Ensure that Os::File properly routes flush calls to the `flushInternal` function.
TEST_F(Interface, Read) {
    U8 buffer[] = {0xab, 0xcd, 0xef};
    FwSignedSizeType size = static_cast<FwSignedSizeType>(sizeof buffer);
    FwSignedSizeType original_size = size;
    Os::File file;
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_READ, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OTHER_ERROR);
    ASSERT_EQ(file.read(buffer, size, Os::File::WaitType::WAIT), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::READ_FN);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.readBuffer, buffer);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.readSize, original_size);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.readWait, Os::File::WaitType::WAIT);
}

// Ensure that Os::File properly routes statuses returned from the `flushInternal` function back to the caller.
TEST_F(Interface, Write) {
    U8 buffer[] = {0xab, 0xcd, 0xef};
    FwSignedSizeType size = static_cast<FwSignedSizeType>(sizeof buffer);
    FwSignedSizeType original_size = size;
    Os::File file;
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OP_OK);
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_WRITE, Os::File::OverwriteType::OVERWRITE), Os::File::OP_OK);
    Os::Stub::File::Test::StaticData::setNextStatus(Os::File::OTHER_ERROR);
    ASSERT_EQ(file.write(buffer, size, Os::File::WaitType::WAIT), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.lastCalled, Os::Stub::File::Test::StaticData::WRITE_FN);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.writeBuffer, buffer);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.writeSize, original_size);
    ASSERT_EQ(Os::Stub::File::Test::StaticData::data.writeWait, Os::File::WaitType::WAIT);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
