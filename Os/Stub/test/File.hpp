#include "Os/File.hpp"
#include <string.h>

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
    //! Read override function
    typedef Os::File::Status (*ReadOverride)(U8 *buffer, FwSignedSizeType &size, Os::File::WaitType wait, void* pointer);

    //! Write override function
    typedef Os::File::Status (*WriteOverride)(const U8* buffer, FwSignedSizeType &size, Os::File::WaitType wait, void* pointer);

    //! Last function called
    LastFn lastCalled = NONE_FN;
    //! Path of last open call
    const char *openPath = nullptr;
    //! Mode of last open call
    Os::File::Mode openMode = Os::File::MAX_OPEN_MODE;
    //! Overwrite of last open call
    Os::File::OverwriteType openOverwrite = Os::File::OverwriteType::NO_OVERWRITE;
    //! Offset of last preallocate call
    FwSignedSizeType preallocateOffset = -1;
    //! Length of last preallocate call
    FwSignedSizeType preallocateLength = -1;
    //! Offset of last seek call
    FwSignedSizeType seekOffset = -1;
    //! Absolute of last seek call
    Os::File::SeekType seekType = Os::File::SeekType::ABSOLUTE;
    //! Buffer of last read call
    U8 *readBuffer = nullptr;
    //! Size of last read call
    FwSignedSizeType readSize = -1;
    //! Wait of last read call
    Os::File::WaitType readWait = Os::File::WaitType::NO_WAIT;
    //! Buffer of last write call
    const void *writeBuffer = nullptr;
    //! Size of last write call
    FwSignedSizeType writeSize = -1;
    //! Wait of last write call
    Os::File::WaitType  writeWait = Os::File::WaitType::NO_WAIT;

    //! File pointer
    FwSignedSizeType pointer = 0;

    //! Next status to be returned
    Os::File::Status nextStatus = Os::File::Status::OTHER_ERROR;
    //! Return of next size call
    FwSignedSizeType sizeResult = -1;
    //! Return of next position call
    FwSignedSizeType positionResult = 0;
    //! Result of next read call
    U8 *readResult = nullptr;
    //! Size result of next read data
    FwSignedSizeType readResultSize = -1;
    //! Size result of next read call
    FwSignedSizeType readSizeResult = -1;
    //! Result holding buffer of next write call
    U8 *writeResult = nullptr;
    //! Size result of next write data
    FwSignedSizeType writeResultSize = -1;
    //! Size result of next read call
    FwSignedSizeType writeSizeResult = -1;

    // Singleton data
    static Data testData;

    //! Set next status to return
    static void setNextStatus(Os::File::Status status);

    //! Set next size to result
    static void setSizeResult(FwSignedSizeType size);

    //! Set next position to result
    static void setPositionResult(FwSignedSizeType position);

    //! Set next read result
    static void setReadResult(U8 *buffer, FwSignedSizeType size);

    //! Set next read size result
    static void setReadSize(FwSignedSizeType size);

    //! Set next write result
    static void setWriteResult(U8 *buffer, FwSignedSizeType size);

    //! Set next write size result
    static void setWriteSize(FwSignedSizeType size);

    //! Set next read size result
    static void setReadOverride(ReadOverride function, void* pointer);

    //! Set next write size result
    static void setWriteOverride(WriteOverride function, void* pointer);

    //! Basic read implementation
    static Os::File::Status basicRead(U8 *buffer, FwSignedSizeType &size, Os::File::WaitType wait, void* pointer);

    //! Basic write implementation
    static Os::File::Status basicWrite(const U8* buffer, FwSignedSizeType &size, Os::File::WaitType wait, void* pointer);

    //! Read override function
    ReadOverride readOverride = Data::basicRead;
    //! Read override pointer
    void* readOverridePointer = nullptr;

    //! Write override function
    WriteOverride writeOverride = Data::basicWrite;
    //! Write override pointer
    void* writeOverridePointer = nullptr;
};

} // namespace Test
} // namespace File
} // namespace Stub
} // namespace Os
#endif // OS_STUB_FILE_TEST_HPP
