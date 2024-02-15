# ======================================================================
# \title Os/Models/file.fpp
# \brief FPP type definitions for Os/File.hpp concepts
# ======================================================================

module Os {
@ FPP shadow-enum representing Os::File::Status
enum FileStatus {
    OP_OK, @<  Operation was successful
    DOESNT_EXIST, @<  File doesn't exist (for read)
    NO_SPACE, @<  No space left
    NO_PERMISSION, @<  No permission to read/write file
    BAD_SIZE, @<  Invalid size parameter
    NOT_OPENED, @<  file hasn't been opened yet
    FILE_EXISTS, @< file already exist (for CREATE with O_EXCL enabled)
    NOT_SUPPORTED, @< Kernel or file system does not support operation
    INVALID_MODE, @< Mode for file access is invalid for current operation
    INVALID_ARGUMENT, @< Invalid argument passed in
    OTHER_ERROR, @<  A catch-all for other errors. Have to look in implementation-specific code
}
@ FPP shadow-enum representing Os::File::Mode
enum FileMode {
    OPEN_NO_MODE, @<  File mode not yet selected
    OPEN_READ, @<  Open file for reading
    OPEN_CREATE, @< Open file for writing and truncates file if it exists, ie same flags as creat()
    OPEN_WRITE, @<  Open file for writing
    OPEN_SYNC_WRITE, @<  Open file for writing; writes don't return until data is on disk
    OPEN_APPEND, @< Open file for appending
}
}
