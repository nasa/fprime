#ifndef UTILS_SHA256_CONFIG_HPP
#define UTILS_SHA256_CONFIG_HPP

// Include the sha library:
#include <Utils/Hash/openssl/sha.h>

//! Define the hash handle type for this 
//! implementation. This is required.
#ifndef HASH_HANDLE_TYPE
#define HASH_HANDLE_TYPE SHA256_CTX
#endif

//! Define the size of a hash digest in bytes for this
//! implementation. This is required.
#ifndef HASH_DIGEST_LENGTH
#define HASH_DIGEST_LENGTH (SHA256_DIGEST_LENGTH)
#endif

//! Define the string to be used as a filename 
//! extension (ie. file.txt.SHA256) for this
//! implementation. This is required.
#ifndef HASH_EXTENSION_STRING
#define HASH_EXTENSION_STRING (".SHA256")
#endif

#endif
