// ======================================================================
// \title  DpZLibCompressor.cpp
// \author kubiak
// \brief  cpp file for DpZLibCompressor component implementation class
// ======================================================================

#include "Svc/DpZLibCompressor/DpZLibCompressor.hpp"

#include <cstring>

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

DpZLibCompressor ::DpZLibCompressor(const char* const compName) : DpZLibCompressorComponentBase(compName) {}

DpZLibCompressor ::~DpZLibCompressor() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void DpZLibCompressor::cleanupContext(ZLibCtx& ctx) {
    if (ctx.compression_buffer.getSize() != 0) {
        bufferCompressionReturn_out(0, ctx.compression_buffer);
    }

    if (ctx.zlib_alloc_buffer.getSize() != 0) {
        bufferZLibReturn_out(0, ctx.zlib_alloc_buffer);
    }
}

Svc::CompressionAlgorithm DpZLibCompressor ::compressChunk_handler(FwIndexType portNum,
                                                                   Fw::Buffer& buffer,
                                                                   FwSizeType min_compression,
                                                                   FwSizeType write_offset) {
    ZLibCtx ctx(*this);

    ctx.compression_buffer = bufferCompressionGet_out(0, min_compression);
    if (ctx.compression_buffer.getSize() == 0) {
        log_WARNING_LO_ZLibCompressionBadBuffer(min_compression);

        cleanupContext(ctx);
        return CompressionAlgorithm::UNCOMPRESSED;
    }

    Fw::ParamValid param_valid;
    const FwSizeType zlib_alloc_size = paramGet_ZLibBufferSize(param_valid);
    FW_ASSERT(param_valid == Fw::ParamValid::DEFAULT || param_valid == Fw::ParamValid::VALID, param_valid);

    ctx.zlib_alloc_buffer = bufferZLibGet_out(0, zlib_alloc_size);
    if (ctx.zlib_alloc_buffer.getSize() == 0) {
        log_WARNING_LO_ZLibAllocBadBuffer(zlib_alloc_size);

        cleanupContext(ctx);
        return CompressionAlgorithm::UNCOMPRESSED;
    }

    ctx.zlib_stream.zalloc = DpZLibCompressor::zlib_alloc_fn;
    ctx.zlib_stream.zfree = DpZLibCompressor::zlib_free_fn;
    ctx.zlib_stream.opaque = &ctx;

    const CompressionAlgorithm alg = zlibCompressionHelper(ctx, buffer);

    if (alg != CompressionAlgorithm::UNCOMPRESSED) {
        // Compression completed and the size is <= min size
        // Copy the compressed data into the original buffer

        // Check that there is sufficient room in the buffer
        // for the compressed data. This should be guaranteed
        // by the caller, but double checking
        FW_ASSERT(buffer.getSize() - write_offset >= ctx.compression_buffer.getSize(),
                  static_cast<FwAssertArgType>(buffer.getSize()), static_cast<FwAssertArgType>(write_offset),
                  static_cast<FwAssertArgType>(ctx.compression_buffer.getSize()));

        std::memcpy(buffer.getData() + write_offset, ctx.compression_buffer.getData(),
                    ctx.compression_buffer.getSize());
        buffer.setSize(ctx.compression_buffer.getSize() + write_offset);
    }

    cleanupContext(ctx);
    return alg;
}

CompressionAlgorithm DpZLibCompressor::zlibCompressionHelper(ZLibCtx& ctx, const Fw::Buffer& in_buffer) {
    Fw::ParamValid param_valid;
    const I8 compression_level = paramGet_CompressionLevel(param_valid);
    FW_ASSERT(param_valid == Fw::ParamValid::DEFAULT || param_valid == Fw::ParamValid::VALID, param_valid);

    int zlib_ok = 0;
    zlib_ok = deflateInit(&ctx.zlib_stream, compression_level);
    if (zlib_ok != Z_OK) {
        ctx.comp.log_WARNING_LO_ZLibInitError(
            zlib_ok, ctx.zlib_stream.msg == nullptr ? Fw::String("") : Fw::String(ctx.zlib_stream.msg));
        return CompressionAlgorithm::UNCOMPRESSED;
    }

    if (in_buffer.getSize() > std::numeric_limits<uInt>::max() ||
        ctx.compression_buffer.getSize() > std::numeric_limits<uInt>::max()) {
        ctx.comp.log_WARNING_LO_BufferTooBigForZLib(in_buffer.getSize(), ctx.compression_buffer.getSize(),
                                                    std::numeric_limits<uInt>::max());
        return CompressionAlgorithm::UNCOMPRESSED;
    }

    ctx.zlib_stream.next_in = in_buffer.getData();
    ctx.zlib_stream.avail_in = static_cast<uInt>(in_buffer.getSize());

    ctx.zlib_stream.next_out = ctx.compression_buffer.getData();
    ctx.zlib_stream.avail_out = static_cast<uInt>(ctx.compression_buffer.getSize());

    zlib_ok = deflate(&ctx.zlib_stream, Z_FINISH);

    if (zlib_ok == Z_STREAM_END && ctx.zlib_stream.total_out <= ctx.compression_buffer.getSize()) {
        ctx.compression_buffer.setSize(ctx.zlib_stream.total_out);

        ctx.comp.log_DIAGNOSTIC_ZLibCompression(in_buffer.getSize(), ctx.compression_buffer.getSize());

        ctx.comp.log_DIAGNOSTIC_ZLibMemoryUsage(ctx.bump_allocator, ctx.zlib_alloc_buffer.getSize());

        return CompressionAlgorithm::ZLIB_DEFLATE;
    } else {
        if (zlib_ok != Z_OK) {
            ctx.comp.log_WARNING_LO_ZLibDeflateError(
                zlib_ok, ctx.zlib_stream.msg == nullptr ? Fw::String("") : Fw::String(ctx.zlib_stream.msg));
        }

        ctx.comp.log_DIAGNOSTIC_ZLibNoCompression(in_buffer.getSize(), ctx.compression_buffer.getSize());
        return CompressionAlgorithm::UNCOMPRESSED;
    }

    // Note: Skipping call to deflateEnd. Per the documentation this
    // call frees the dynamically allocated data structures, but this
    // is not necessary with the bump allocator.
    // The call to deflateEnd does not flush any additional output data
}

voidpf DpZLibCompressor::zlib_alloc_fn(voidpf opaque, uInt items, uInt size) {
    FW_ASSERT(opaque != nullptr);
    ZLibCtx* ctx = reinterpret_cast<ZLibCtx*>(opaque);

    FW_ASSERT(ctx->bump_allocator <= ctx->zlib_alloc_buffer.getSize(),
              static_cast<FwAssertArgType>(ctx->bump_allocator),
              static_cast<FwAssertArgType>(ctx->zlib_alloc_buffer.getSize()));
    const FwSizeType free_space = ctx->zlib_alloc_buffer.getSize() - ctx->bump_allocator;

    const FwSizeType alloc_size = static_cast<FwSizeType>(items) * static_cast<FwSizeType>(size);

    if (free_space < alloc_size) {
        return Z_NULL;
    }

    U8* alloc_ptr = ctx->zlib_alloc_buffer.getData() + ctx->bump_allocator;
    ctx->bump_allocator += alloc_size;

    return alloc_ptr;
}

void DpZLibCompressor::zlib_free_fn(voidpf opaque, voidpf address) {
    // No work. Cannot free from bump allocator
}

}  // namespace Svc
