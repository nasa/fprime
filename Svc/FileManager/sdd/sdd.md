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
name, a chunk size, a begin and end offset, a container priority and an
emission mode, and the requested range of the file is emitted one chunk at a
time. An end offset of zero means the end of the
file, so a begin and end offset of zero packages the whole file. Ranges let an
operator retransmit part of a file, or spread a downlink over several commands
when a project cannot fit the whole file into data products at once. Each chunk
is written as a pair of records: a `FileChunkHeaderRecord` carrying the source
file name, the offset of the chunk within the file and the number of data
bytes, followed by a `FileChunkDataRecord` holding the chunk bytes. Ground
tools reassemble the original file from these records, so the command works
regardless of the size of the buffers allocated to data products.

The emission mode selects how the chunks are written. In `PACED` mode they are
metered out by the rate group in the same way as directory listing, one chunk
per tick by default, and the command response is deferred until the last chunk
has been sent. In `IMMEDIATE` mode the whole range is written in the command
handler, which suits projects that would rather finish quickly than spread the
work over time.

A priority of zero uses `Svc::FileManagerCfg::DEFAULT_DP_PRIORITY`, which
projects can adjust, while a non-zero priority applies to the containers of
that request only. The requested chunk size is clamped to
`FileManagerConfig::GENERATE_DP_MAX_CHUNK_SIZE`, which bounds the read buffer
held by the component.

Failures during data product generation emit a warning event but still return a
successful command response. A bad file name or a transient resource problem
therefore does not stop a command sequence that happens to contain the command,
while operators still see exactly what went wrong in the event log.

Because each chunk header carries the absolute offset
within the source file, chunks produced by separate commands reassemble
correctly on the ground with no extra bookkeeping. The data product ports are
left for the deployment to
connect; if they are not connected the command fails with an event rather than
attempting to allocate a container.


