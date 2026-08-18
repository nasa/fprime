

module Os {

    @ File open mode permission bits
    @ Constant values are derived from standard
    @ Unix values, but should not be assumed
    @ to match
    constant FILE_MODE_IRUSR = 0x040
    constant FILE_MODE_IWUSR = 0x080
    constant FILE_MODE_IXUSR = 0x100
    constant FILE_MODE_IRWXU = 0x1C0

    constant FILE_MODE_IRGRP = 0x008
    constant FILE_MODE_IWGRP = 0x010
    constant FILE_MODE_IXGRP = 0x020
    constant FILE_MODE_IRWXG = 0x038

    constant FILE_MODE_IROTH = 0x001
    constant FILE_MODE_IWOTH = 0x002
    constant FILE_MODE_IXOTH = 0x004
    constant FILE_MODE_IRWXO = 0x007

    constant FILE_MODE_ISUID = 0x800
    constant FILE_MODE_ISGID = 0x400
    constant FILE_MODE_ISVTX = 0x200

    @ File creation mode used when calling Os::File "open" with
    @ the OPEN_CREATE mode flag.
    @ The FILE_DEFAULT_CREATE_MODE constant @ is a bitfield of the
    @ above FILE_MODE_* bits that should be present in the created file
    @ The default value corresponds to a mode of 666 (a+rw) on Linux
    constant FILE_DEFAULT_CREATE_MODE = FILE_MODE_IRUSR + FILE_MODE_IWUSR + \
                                        FILE_MODE_IRGRP + FILE_MODE_IWGRP + \
                                        FILE_MODE_IROTH + FILE_MODE_IWOTH
}
