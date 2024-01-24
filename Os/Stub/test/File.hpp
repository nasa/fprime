#include "Os/File.hpp"
#ifndef OS_STUB_FILE_TEST_HPP
#define OS_STUB_FILE_TEST_HPP
namespace Os {
namespace Stub {
namespace File {
namespace Test {

/**
* Data that supports the stubbed File implementation.
*/
struct Data {
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
    //! Last function called
    LastFn lastCalled = NONE_FN;
    //! Path of last open call
    const char *openPath = nullptr;
    //! Mode of last open call
    Os::File::Mode openMode = Os::File::MAX_OPEN_MODE;
    //! Overwrite of last open call
    bool openOverwrite = false;
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

    //! Next status to be returned
    Os::File::Status nextStatus = Os::File::Status::OTHER_ERROR;
    //! Return of next size call
    FwSizeType sizeResult = -1;
    //! Return of next position call
    FwSizeType positionResult = -1;
    //! Result of next read call
    U8 *readResult = nullptr;
    //! Size of next read call
    FwSizeType readSizeResult = -1;

    // Singleton data
    static TestData TEST_DATA;
};



    void constructInternal() {
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

    void testCloseHandler(void* pointer) {
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
        // Copy read data if set
        if (nullptr != TEST_DATA.readResult) {
            size = FW_MIN(size, TEST_DATA.readSizeResult);
            (void) ::memcpy(buffer, TEST_DATA.readResult, size);
        }
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


} // namespace Test
} // namespace File
} // namespace Stub
} // namespace Os
#endif // OS_STUB_FILE_TEST_HPP