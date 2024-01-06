// ======================================================================
// \title Os/Stub/File.hpp
// \brief stub implementation for Os::File, header and test definitions
// ======================================================================
#include <Os/File.hpp>

#ifndef OS_STUB_FILE_HPP
#define OS_STUB_FILE_HPP

namespace Os {

/**
 * FileHandle class definition for stubbed implementations. Tracks state of usage of the file.
 */
class FileHandle {
  public:
    enum State {
        UNOPENED, //!< File has not yet been opened
        OPENED, //!< Opened the given file
        FAILED, //!< Failed to open
        CLOSED, //!< File is closed
    };
    //! Construct in UNOPENED state
    FileHandle();

    //! State of the file handle
    State state;
};

namespace Stub {
/**
 * Enumeration of functions called for stub-os implementation.
 */
enum LastFunctionCalled {
    NONE,
    CONSTRUCT_FN,
    DESTRUCT_FN,
    OPEN_FN,
    CLOSE_FN,
    PREALLOCATE_FN,
    SEEK_FN,
    SEEK_ABSOLUTE_FN,
    FLUSH_FN,
    READ_FN
};

/**
 * Set the next status to be returned by the stubbed Os::File implementation.
 *
 * \param status: status to be returned next
 */
void file_set_next_status(Os::File::Status status);

/**
 * Return the last function called in Os::File::Stub implementation. This is a global implementation.
 * \return last function called as enumerated value
 */
LastFunctionCalled file_get_last_call();
} // Stub
} // Os

#endif
