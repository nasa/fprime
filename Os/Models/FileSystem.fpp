# ======================================================================
# \title Os/Models/FileSystem.fpp
# \brief FPP type definitions for Os/FileSystem.hpp concepts
# ======================================================================

module Os {
@ FPP shadow-enum representing Os::FileSystem::Status
enum FileSystemStatus {
        OP_OK,            @<  Operation was successful
        ALREADY_EXISTS,   @<  File already exists
        NO_SPACE,         @<  No space left
        NO_PERMISSION,    @<  No permission to write
        NOT_DIR,          @<  Path is not a directory
        IS_DIR,           @<  Path is a directory
        NOT_EMPTY,        @<  directory is not empty
        INVALID_PATH,     @<  Path is too long, too many sym links, etc.
        DOESNT_EXIST,     @<  Path doesn't exist
        FILE_LIMIT,       @<  Too many files or links
        BUSY,             @<  Operand is in use by the system or by a process
        NO_MORE_FILES,    @<  Directory stream has no more files
        BUFFER_TOO_SMALL, @<  Buffer size is too small to hold full path (for getWorkingDirectory)
        EXDEV_ERROR,      @<  Operation not supported across devices (e.g. rename)
        OVERFLOW_ERROR,   @<  Operation failed due to overflow in calculation of the result
        NOT_SUPPORTED,    @<  Operation is not supported by the current implementation
        OTHER_ERROR,      @<  other OS-specific error
}

}
