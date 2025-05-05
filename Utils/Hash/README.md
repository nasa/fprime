# Utils::Hash

This directory contains a generic interface for creating hashes 
of data. It provides a mechanism for selecting different implementations
of the hash interface, which could be as simple as an 8 bit checksum or as
complex as a 256 bit SHA256 hash.

The generic interface to the hashing utility is included in `Utils/Hash/Hash.hpp`. 
Its corresponding generic hash buffer class (which is used to hold a computed hash
digest) can be found in `Utils/Hash/HashBuffer.hpp`. Some common implementation code 
for each class is included in `Utils/Hash/HashCommon.cpp` and `Utils/Hash/HashBufferCommon.cpp`.

Specific implementations of the hashing utility are stored in subdirectories in `Utils/Hash/`.
Currently, one such implementation exists in `Utils/Hash/openssl/` which provides a SHA256
hash using the openssl library. Another implementation is also provided which calculates a 
32-bit CRC32, which depends on no external libraries.

A specific implementation can be selected by modifying the `HashConfig.hpp` file.

## Using `hash`

The generic hash interface includes only 4 methods besides the constructor/destructor. A 
description of each method is included below:

`hash.init()` - This method initializes the hash object, priming it for computing a new hash. It 
should be run before beginning to calculate each new hash. It resets any state remaining
from a previously computed hash.

`hash.update(data, len)` - This method updates the hash object with new data to hash. It can be run as many
times as you like, allowing you to continue adding data to the hash state. In this way, a user can hash their data as they read it
from a buffer, or from a file, in segments.

`hash.final(buffer)` - This method returns a hash of all the data given to the hash object via `update` since
the last `init` was run. It returns the hash in `buffer`, which is a `HashBuffer` object.

`hash.hash(data, len, buffer)` - This method runs `init`, `update`, and `final` all in one shot. If you have already
collected all the data that you want to hash into a buffer `data` with length `len`, you can use this static function
to calculate the hash all at once. The computed hash is returned in `buffer`, which is a `HashBuffer` object.

## Configuring `hash`

To configure the `hash` utility to use a specific hashing implementation, modify `HashConfig.hpp` to include 
the header file of the implementation you wish to use. For example, to use SHA256 hashing, your `HashConfig.hpp` 
should look something like:

```
#ifndef UTILS_HASH_CONFIG_HPP
#define UTILS_HASH_CONFIG_HPP

#include <Utils/Hash/openssl/SHA256.hpp>

#endif
```

## Building your own `hash` implementation

The generic interface to `hash` can be implemented using many different hashing algorithms. To construct your own 
please look at the SHA256 implementation as an example, found in the `openssl` subdirectory. Your implementation
should include a few files:

`YourImplementationHash.hpp` - This file must declare the following `#define` constants: `HASH_HANDLE_TYPE`, the handle type for computing the hash internally, `HASH_DIGEST_LENGTH`, the length of the hash digest in bytes, and `HASH_EXTENSION_STRING`, the file extension you would assign to the hash should it be stored into a file (ie. `.SHA256`).

`YourImplementationHash.cpp` - The implementation file for the hashing algorithm. It should implement the methods declared
in `Hash.hpp`, except for those included in `HashCommon.cpp`.

`README.md` - So that those who come after you will understand how your implementation works and what it is used for ;)

