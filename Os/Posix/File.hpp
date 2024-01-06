// ======================================================================
// \title Os/Posix/File.hpp
// \brief posix implementation for Os::File, header and test definitions
// ======================================================================
#ifndef OS_POSIX_FILE_HPP
#define OS_POSIX_FILE_HPP

namespace Os {

/**
 * FileHandle class definition for posix implementations.
 */
class FileHandle {
  public:
    FileHandle();
    //! int type file descriptor used in posix calls
    PlatformIntType file_descriptor;
};
}
#endif // OS_POSIX_FILE_HPP
