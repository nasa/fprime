#ifndef UTILS_HASH_CRC32_CRC32_HPP
#define UTILS_HASH_CRC32_CRC32_HPP

#include "Fw/FPrimeBasicTypes.hpp"

namespace Utils {

// This is an implementation of the IEEE 802.3 CRC32 polynomial, which is
//   0x82608EDB when expressed as a reverse reciprocal form, or 0xEDB88320
//   when expressed in a reverse form.
//
// Note that this specific function does not perform the one's complement
// that is typically performed when calculating a CRC32 hash. Use the Hash
// class implementation instead.
U32 crc32_ieee802_3_update(const U8* data, FwSizeType length, U32 crc);

//! Define the hash handle type for this
//! implementation. This is required.
#ifndef HASH_HANDLE_TYPE
#define HASH_HANDLE_TYPE U32  // NO_CODESONAR  LANG.PREPROC.MACROSTART/END
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

}  // namespace Utils

#endif
