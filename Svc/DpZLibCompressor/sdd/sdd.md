# Svc::DpZLibCompressor

`Svc::DpZLibCompressor` is a Data Product Compressor using ZLib and the DEFLATE algorithm to compress data product chunks. `Svc::DpZLibCompressor` is responsible for receiving chunks of data to compress from DpCompressProc and attempting to compress them.

## Usage Examples

See DpCompressProc

## Port Descriptions
| Name | Description |
|---|---|
|compressChunk| Receive chunks of data to compress. See DpCompressProc for more information|
|bufferCompressionGet| Get an `Fw::Buffer`. This compression buffer is used as a temporary buffer to store the results of the compression|
|bufferCompressionReturn| Return the compression buffer |
|bufferZLibGet| Get an `Fw::Buffer`. This zlib buffer is used as a memory pool to implement malloc/free for zlib through an internal bump allocator|
|bufferZLibReturn| Return the zlib buffer |

## Component States

None. DpZLibCompressor is stateless between invocations

## Sequence Diagrams

See DpCompressProc

## Parameters
| Name | Description |
|---|---|
| CompressionLevel | Compression level to pass to zlib in the range [0, 9] inclusive. 0 indicates no compression and 9 indicates maximum compression at the cost of runtime |
| ZLibBufferSize | Size of the ZLib allocator buffer to request through the bufferZLibGet port |

## Requirements
Add requirements in the chart below
| Name | Description | Validation |
|---|---|---|
| SVC-DPZLIBCOMPRESSOR-001 | `Svc::DpZLibCompressor` shall attempt to compress incoming data chunks per the protocol defined by DpCompressProc | Unit Test
| SVC-DPZLIBCOMPRESSOR-002 | `Svc::DpZLibCompressor` shall either return `Svc::CompressionAlgorithm::UNCOMPRESSED` or `Svc::CompressionAlgorithm::ZLIB_DEFLATE` from the compressChunk input port | Unit Test

## Events

| Name                  | Description                                                               |
|-----------------------|---------------------------------------------------------------------------|
| ZLibCompressBadBuffer | WARNING\_LO Unable to allocate a compression buffer through BufferManager |
| ZLibAllocBadBuffer    | WARNING\_LO Unable to allocate an alloc buffer through BufferManager      |
| ZLibInitError         | WARNING\_LO ZLib error during deflateInit call                            |
| ZLibDeflateError      | WARNING\_LO ZLib error during deflate call                                |
| BufferTooBigForZLib   | WARNING\_LO Buffer size exceeds zlib integer size limits                  |
| ZLibNoCompression     | DIAGNOSTIC  ZLib unable to compress buffer                                |
| ZLibCompression       | DIAGNOSTIC  ZLib compressed buffer                                        |
| ZLibMemoryUsage       | DIAGNOSTIC  ZLib maximum memory usage during compression                  |


## Change Log
| Date | Description |
|---|---|
|04/14/2026| Initial Draft |
