// ======================================================================
// \title  DpZLibCompressor.hpp
// \author kubiak
// \brief  hpp file for DpZLibCompressor component implementation class
// ======================================================================

#ifndef Svc_DpZLibCompressor_HPP
#define Svc_DpZLibCompressor_HPP

#include "Svc/DpZLibCompressor/DpZLibCompressorComponentAc.hpp"

#include <zlib.h>

namespace Svc {

class DpZLibCompressor final : public DpZLibCompressorComponentBase {
  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct DpZLibCompressor object
    DpZLibCompressor(const char* const compName  //!< The component name
    );

    //! Destroy DpZLibCompressor object
    ~DpZLibCompressor();

  private:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for compressChunk
    Svc::CompressionAlgorithm compressChunk_handler(FwIndexType portNum,  //!< The port number
                                                    Fw::Buffer& buffer,
                                                    FwSizeType min_compression,
                                                    FwSizeType write_offset) override;

    struct ZLibCtx {
        Fw::Buffer compression_buffer;

        Fw::Buffer zlib_alloc_buffer;
        FwSizeType bump_allocator;

        z_stream zlib_stream;
        DpZLibCompressor& comp;

        explicit ZLibCtx(DpZLibCompressor& c)
            : compression_buffer(), zlib_alloc_buffer(), bump_allocator(0), zlib_stream(), comp(c) {}
    };

    void cleanupContext(ZLibCtx& ctx);

    CompressionAlgorithm zlibCompressionHelper(ZLibCtx& ctx, const Fw::Buffer& in_buffer);

    static voidpf zlib_alloc_fn(voidpf opaque, uInt items, uInt size);
    static void zlib_free_fn(voidpf opaque, voidpf address);
};

}  // namespace Svc

#endif
