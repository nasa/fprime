# Svc::FileManager Component

## Overview

`Svc::FileManager` provides a set of ground commands for common file and filesystem operations.
It is a wrapper around the OSAL file, filesystem and directory APIs. The component accepts
commands, calls the corresponding OSAL operation, and reports the result through events,
telemetry, and command responses.

## Functionality

`Svc::FileManager` supports common filesystem operations. They are currently:

- CreateDirectory
- RemoveDirectory
- ListDirectory
- MoveFile
- RemoveFile
- AppendFile
- FileSize
- CalculateCrc
- GenerateDp

For each command, the component returns success or failure and emits status
information for operators.

### GenerateDp

`GenerateDp` packages a file into data products. The command takes the file
name and a chunk size, and the file is emitted one chunk at a time. Each chunk
is written as a pair of records: a `FileChunkHeaderRecord` carrying the source
file name, the offset of the chunk within the file and the number of data
bytes, followed by a `FileChunkDataRecord` holding the chunk bytes. Ground
tools reassemble the original file from these records, so the command works
regardless of the size of the buffers allocated to data products.

Chunks are paced by the rate group in the same way as directory listing, one
chunk per tick by default, and the command response is deferred until the last
chunk has been sent. The requested chunk size is clamped to
`FileManagerConfig::GENERATE_DP_MAX_CHUNK_SIZE`, which bounds the read buffer
held by the component. The data product ports are left for the deployment to
connect; if they are not connected the command fails with an event rather than
attempting to allocate a container.


