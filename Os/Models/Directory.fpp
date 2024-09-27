# ======================================================================
# \title Os/Models/Directory.fpp
# \brief FPP type definitions for Os/Directory.hpp concepts
# ======================================================================

module Os {
@ FPP shadow-enum representing Os::Directory::Status
enum DirectoryStatus {
    OP_OK,          @<  Operation was successful
    DOESNT_EXIST,   @<  Directory doesn't exist
    NO_PERMISSION,  @<  No permission to read directory
    NOT_OPENED,     @<  Directory hasn't been opened yet
    NOT_DIR,        @<  Path is not a directory
    NO_MORE_FILES,  @<  Directory stream has no more files
    FILE_LIMIT,     @<  Directory has more files than can be read
    BAD_DESCRIPTOR, @<  Directory stream descriptor is invalid
    ALREADY_EXISTS, @<  Directory already exists
    NOT_SUPPORTED,  @<  Operation is not supported by the current implementation
    OTHER_ERROR,    @<  A catch-all for other errors. Have to look in implementation-specific code
}

enum DirectoryOpenMode {
    READ,               @<  Error if directory doesn't exist
    CREATE_IF_MISSING,  @<  Create directory if it doesn't exist
    CREATE_EXCLUSIVE,   @<  Create directory and error if it already exists
}
}
