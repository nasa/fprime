// ======================================================================
// \title Os/Stub/test/ut/StubFileTests.cpp
// \brief tests using stub implementation for Os::File interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/File.hpp"
#include "Os/test/ut/file/CommonTests.hpp"
#include "Os/test/ut/file/RulesHeaders.hpp"
#include "Os/Stub/File.hpp"

namespace Os {
namespace Test {
namespace File {

namespace Stub {
    /**
     * Data that supports the stub tests
     */
    struct TestData {
        /**
         * Enumeration of last function called
         */
        enum LastFn {
            NONE_FN,
            CONSTRUCT_FN,
            DESTRUCT_FN,
            OPEN_FN,
            CLOSE_FN,
            SIZE_FN,
            POSITION_FN,
            PREALLOCATE_FN,
            SEEK_FN,
            FLUSH_FN,
            READ_FN,
            WRITE_FN
        };
        //! Next status to be returned
        Os::File::Status nextStatus = Os::File::Status::OTHER_ERROR;
        //! Last function called
        LastFn lastCalled = NONE_FN;
        //! Path of last open call
        const char *openPath = nullptr;
        //! Mode of last open call
        Os::File::Mode openMode = Os::File::MAX_OPEN_MODE;
        //! Overwrite of last open call
        bool openOverwrite = false;
        //! Return of next size call
        FwSizeType sizeResult = -1;
        //! Return of next position call
        FwSizeType positionResult = -1;
        //! Offset of last preallocate call
        FwSizeType preallocateOffset = -1;
        //! Length of last preallocate call
        FwSizeType preallocateLength = -1;
        //! Offset of last seek call
        FwSizeType seekOffset = -1;
        //! Absolute of last seek call
        bool seekAbsolute = false;
        //! Buffer of last read call
        U8 *readBuffer = nullptr;
        //! Size of last read call
        FwSizeType readSize = -1;
        //! Wait of last read call
        bool readWait = false;
        //! Buffer of last write call
        const void *writeBuffer = nullptr;
        //! Size of last write call
        FwSizeType writeSize = -1;
        //! Wait of last write call
        bool writeWait = false;
    };
    TestData TEST_DATA;

    void testConstructHandler() {
        TEST_DATA.lastCalled = TestData::CONSTRUCT_FN;
    }

    void testDestructHandler() {
        TEST_DATA.lastCalled = TestData::DESTRUCT_FN;
    }

    Os::File::Status testOpenHandler(const char *filepath, Os::File::Mode open_mode, bool overwrite) {
        TEST_DATA.openPath = filepath;
        TEST_DATA.openMode = open_mode;
        TEST_DATA.openOverwrite = overwrite;
        TEST_DATA.lastCalled = TestData::OPEN_FN;
        return TEST_DATA.nextStatus;
    }

    void testCloseHandler() {
        TEST_DATA.lastCalled = TestData::CLOSE_FN;
    }

    Os::File::Status testSizeHandler(FwSizeType& size_result) {
        TEST_DATA.lastCalled = TestData::SIZE_FN;
        size_result = TEST_DATA.sizeResult;
        return TEST_DATA.nextStatus;
    }

    Os::File::Status testPositionHandler(FwSizeType& position_result) {
        TEST_DATA.lastCalled = TestData::POSITION_FN;
        position_result = TEST_DATA.positionResult;
        return TEST_DATA.nextStatus;
    }

    Os::File::Status testPreallocateHandler(FwSizeType offset, FwSizeType length) {
        TEST_DATA.preallocateOffset = offset;
        TEST_DATA.preallocateLength = length;
        TEST_DATA.lastCalled = TestData::PREALLOCATE_FN;
        return TEST_DATA.nextStatus;

    }

    Os::File::Status testSeekHandler(FwSizeType offset, bool absolute) {
        TEST_DATA.seekOffset = offset;
        TEST_DATA.seekAbsolute = absolute;
        TEST_DATA.lastCalled = TestData::SEEK_FN;
        return TEST_DATA.nextStatus;
    }

    Os::File::Status testFlushHandler() {
        TEST_DATA.lastCalled = TestData::FLUSH_FN;
        return TEST_DATA.nextStatus;
    }

    Os::File::Status testReadHandler(U8 *buffer, FwSizeType &size, bool wait) {
        TEST_DATA.readBuffer = buffer;
        TEST_DATA.readSize = size;
        TEST_DATA.readWait = wait;
        TEST_DATA.lastCalled = TestData::READ_FN;
        return TEST_DATA.nextStatus;
    }

    Os::File::Status testWriteHandler(const void *buffer, FwSizeType &size, bool wait) {
        TEST_DATA.writeBuffer = buffer;
        TEST_DATA.writeSize = size;
        TEST_DATA.writeWait = wait;
        TEST_DATA.lastCalled = TestData::WRITE_FN;
        return TEST_DATA.nextStatus;
    }
}

/**
 * Set up for the test ensures that the test can run at all
 */
void setUp(bool requires_io) {
    if (requires_io) {
        GTEST_SKIP() << "Cannot run tests requiring functional i/o";
    }
    Stub::TEST_DATA = Stub::TestData();
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    Os::Test::File::Stub::TEST_DATA.positionResult = 0;
    Os::Stub::File::Handlers::constructHandler = Stub::testConstructHandler;
    Os::Stub::File::Handlers::destructHandler = Stub::testDestructHandler;
    Os::Stub::File::Handlers::openHandler = Stub::testOpenHandler;
    Os::Stub::File::Handlers::closeHandler = Stub::testCloseHandler;
    Os::Stub::File::Handlers::sizeHandler = Stub::testSizeHandler;
    Os::Stub::File::Handlers::positionHandler = Stub::testPositionHandler;
    Os::Stub::File::Handlers::preallocateHandler = Stub::testPreallocateHandler;
    Os::Stub::File::Handlers::seekHandler = Stub::testSeekHandler;
    Os::Stub::File::Handlers::flushHandler = Stub::testFlushHandler;
    Os::Stub::File::Handlers::readHandler = Stub::testReadHandler;
    Os::Stub::File::Handlers::writeHandler = Stub::testWriteHandler;
}
std::vector<std::shared_ptr<const std::string> > FILES;
/**
 * Tear down for the tests cleans up the test file used
 */
void tearDown() {}

class StubsTester : public Tester {
    /**
     * Check if the test file exists.
     * @return true if it exists, false otherwise.
     */
    bool exists(const std::string &filename) const override {
        for (size_t i = 0; i < FILES.size(); i++) {
            if (filename == *FILES.at(i)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Get a filename, randomly if random is true, otherwise use a basic filename.
     * @param random: true if filename should be random, false if predictable
     * @return: filename to use for testing
     */
    std::shared_ptr<const std::string> get_filename(bool random) const override {
        std::shared_ptr<const std::string> filename = std::shared_ptr<const std::string>(new std::string("DOES-NOT-MATTER"), std::default_delete<std::string>());
        FILES.push_back(filename);
        return filename;
    }

    /**
     * Posix tester is fully functional
     * @return true
     */
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
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::CONSTRUCT_FN);
}

// Ensure that Os::File properly routes destructor calls to the `destructInternal` function.
TEST_F(Interface, Destruction) {
    delete (new Os::File);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::DESTRUCT_FN);
}

// Ensure that Os::File properly routes open calls to the `openInternal` function.
TEST_F(Interface, Open) {
    const char* path = "/does/not/matter";
    Os::File file;
    ASSERT_EQ(file.open(path, Os::File::OPEN_CREATE, true), Os::File::Status::OP_OK);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::OPEN_FN);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.openPath, path);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.openMode, Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.openOverwrite, true);
}

// Ensure that Os::File properly routes close calls to the `closeInternal` function.
TEST_F(Interface, Close) {
    Os::File file;
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, true), Os::File::OP_OK);
    file.close();
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::CLOSE_FN);
}

// Ensure that Os::File properly routes close calls to the `sizeInternal` function.
TEST_F(Interface, Size) {
    FwSizeType sizeResult = -1;
    Os::File file;
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, true), Os::File::OP_OK);
    Os::Test::File::Stub::TEST_DATA.sizeResult = 30;
    ASSERT_EQ(file.size(sizeResult), Os::File::OP_OK);
    ASSERT_EQ(sizeResult, 30);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::SIZE_FN);
}

// Ensure that Os::File properly routes close calls to the `positionInternal` function.
TEST_F(Interface, Position) {
    FwSizeType positionResult = -1;
    Os::File file;
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, true), Os::File::OP_OK);
    Os::Test::File::Stub::TEST_DATA.positionResult = 50;
    ASSERT_EQ(file.position(positionResult), Os::File::OP_OK);
    ASSERT_EQ(positionResult, 50);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::POSITION_FN);
}

// Ensure that Os::File properly routes preallocate calls to the `preallocateInternal` function.
TEST_F(Interface, Preallocate) {
    Os::File file;
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, true), Os::File::OP_OK);
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OTHER_ERROR;
    ASSERT_EQ(file.preallocate(0, 0), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::PREALLOCATE_FN);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.preallocateOffset, 0);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.preallocateLength, 0);

}

// Ensure that Os::File properly routes seek calls to the `seekInternal` function.
TEST_F(Interface, Seek) {
    Os::File file;
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, true), Os::File::OP_OK);
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OTHER_ERROR;
    ASSERT_EQ(file.seek(0, true), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::SEEK_FN);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.seekOffset, 0);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.seekAbsolute, true);
}

// Ensure that Os::File properly routes flush calls to the `flushInternal` function.
TEST_F(Interface, Flush) {
    Os::File file;
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, true), Os::File::OP_OK);
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OTHER_ERROR;
    ASSERT_EQ(file.flush(), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::FLUSH_FN);
}

// Ensure that Os::File properly routes flush calls to the `flushInternal` function.
TEST_F(Interface, Read) {
    U8 buffer[] = {0xab, 0xcd, 0xef};
    FwSizeType size = sizeof buffer;
    Os::File file;
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_READ, true), Os::File::OP_OK);
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OTHER_ERROR;
    ASSERT_EQ(file.read(buffer, size, true), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::READ_FN);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.readBuffer, buffer);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.readSize, size);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.readWait, true);
}

// Ensure that Os::File properly routes statuses returned from the `flushInternal` function back to the caller.
TEST_F(Interface, Write) {
    U8 buffer[] = {0xab, 0xcd, 0xef};
    FwSizeType size = sizeof buffer;
    Os::File file;
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OP_OK;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_WRITE, true), Os::File::OP_OK);
    Os::Test::File::Stub::TEST_DATA.nextStatus = Os::File::OTHER_ERROR;
    ASSERT_EQ(file.write(buffer, size, true), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.lastCalled, Os::Test::File::Stub::TestData::WRITE_FN);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.writeBuffer, buffer);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.writeSize, size);
    ASSERT_EQ(Os::Test::File::Stub::TEST_DATA.writeWait, true);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
