#ifndef UTILS_CRC32_CONFIG_HPP
#define UTILS_CRC32_CONFIG_HPP

// Include the lic crc c library:
extern "C" {
    #include <Utils/Hash/libcrc/lib_crc.h>
}

//! Define the hash handle type for this 
//! implementation. This is required.
#ifndef HASH_HANDLE_TYPE
#define HASH_HANDLE_TYPE U32
#endif

//! Define the size of a hash digest in bytes for this
//! implementation. This is required.
#ifndef HASH_DIGEST_LENGTH
#define HASH_DIGEST_LENGTH (4)
#endif

//! Define the string to be used as a filename 
//! extension (ie. file.txt.SHA256) for this
//! implementation. This is required.
#ifndef HASH_EXTENSION_STRING
#define HASH_EXTENSION_STRING (".CRC32")
#endif

#endif
