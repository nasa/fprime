// ======================================================================
// \title  DpZLibCompressorTester.cpp
// \author kubiak
// \brief  cpp file for DpZLibCompressor component test harness implementation class
// ======================================================================

#include "DpZLibCompressorTester.hpp"

#include <vector>

#include <zlib.h>

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

DpZLibCompressorTester ::DpZLibCompressorTester()
    : DpZLibCompressorGTestBase("DpZLibCompressorTester", DpZLibCompressorTester::MAX_HISTORY_SIZE),
      component("DpZLibCompressor"),
      do_alloc_compression_buffer_(true),
      do_alloc_zlib_buffer_(true),
      active_compression_buffer_(),
      active_zlib_buffer_() {
    this->initComponents();
    this->connectPorts();
}

DpZLibCompressorTester ::~DpZLibCompressorTester() {
    if (active_compression_buffer_.has_value()) {
        delete[] active_compression_buffer_.value();
    }

    if (active_zlib_buffer_.has_value()) {
        delete[] active_zlib_buffer_.value();
    }
}

// ----------------------------------------------------------------------
// Tests
// ----------------------------------------------------------------------

void DpZLibCompressorTester ::compress(const std::vector<U8>& data,
                                       const CompressionAlgorithm exp_alg,
                                       const FwSizeType min_compression,
                                       const FwSizeType write_offset,
                                       const FwSizeType zlib_alloc_size,
                                       const I8 compression_level) {
    paramSet_ZLibBufferSize(zlib_alloc_size, Fw::ParamValid::VALID);
    paramSet_CompressionLevel(compression_level, Fw::ParamValid::VALID);

    this->component.loadParameters();

    std::vector<U8> buffer_data = data;
    Fw::Buffer uncomp_buffer(buffer_data.data(), buffer_data.size());
    const CompressionAlgorithm alg = invoke_to_compressChunk(0, uncomp_buffer, min_compression, write_offset);

    ASSERT_EQ(alg, exp_alg);
    if (alg != CompressionAlgorithm::UNCOMPRESSED) {
        ASSERT_LT(uncomp_buffer.getSize(), min_compression + write_offset);

        std::vector<U8> uncomp_data = zlib_inflate(uncomp_buffer, data.size(), write_offset);
        ASSERT_EQ(uncomp_data.size(), data.size());

        ASSERT_EQ(memcmp(uncomp_data.data(), data.data(), data.size()), 0);

        ASSERT_EVENTS_ZLibMemoryUsage_SIZE(1);

        ASSERT_EVENTS_ZLibCompression_SIZE(1);
        ASSERT_EVENTS_ZLibCompression(0, buffer_data.size(), uncomp_buffer.getSize() - write_offset);
    } else {
        ASSERT_EQ(memcmp(uncomp_buffer.getData(), data.data(), data.size()), 0);

        if (!do_alloc_compression_buffer_) {
            ASSERT_EVENTS_ZLibCompressionBadBuffer_SIZE(1);
            ASSERT_EVENTS_ZLibCompressionBadBuffer(0, buffer_data.size());
        }

        if (!do_alloc_zlib_buffer_) {
            ASSERT_EVENTS_ZLibAllocBadBuffer_SIZE(1);
            ASSERT_EVENTS_ZLibAllocBadBuffer(0, zlib_alloc_size);
        }

        if (do_alloc_compression_buffer_ && do_alloc_zlib_buffer_) {
            ASSERT_EVENTS_ZLibNoCompression_SIZE(1);
            ASSERT_EVENTS_ZLibNoCompression(0, uncomp_buffer.getSize(), min_compression);
        }
    }

    ASSERT_FALSE(active_compression_buffer_.has_value());
    ASSERT_FALSE(active_zlib_buffer_.has_value());
}

std::vector<U8> DpZLibCompressorTester::zlib_inflate(const Fw::Buffer& comp_buffer,
                                                     const FwSizeType uncompressed_size,
                                                     const FwSizeType write_offset) {
    std::vector<U8> out_vec;
    out_vec.resize(uncompressed_size);

    uLongf dest_len = uncompressed_size;
    int zlib_ok = uncompress(out_vec.data(), &dest_len, comp_buffer.getData() + write_offset,
                             comp_buffer.getSize() - write_offset);

    if (zlib_ok != Z_OK) {
        return std::vector<U8>();
    } else {
        return out_vec;
    }
}

void DpZLibCompressorTester ::compress_zlibiniterror(const std::vector<U8>& data,
                                                     const CompressionAlgorithm exp_alg,
                                                     const FwSizeType min_compression,
                                                     const FwSizeType write_offset,
                                                     const FwSizeType zlib_alloc_size,
                                                     const I8 compression_level) {
    paramSet_ZLibBufferSize(zlib_alloc_size, Fw::ParamValid::VALID);
    paramSet_CompressionLevel(compression_level, Fw::ParamValid::VALID);

    this->component.loadParameters();

    std::vector<U8> buffer_data = data;
    Fw::Buffer uncomp_buffer(buffer_data.data(), buffer_data.size());
    const CompressionAlgorithm alg = invoke_to_compressChunk(0, uncomp_buffer, min_compression, write_offset);
    ASSERT_EVENTS_ZLibInitError_SIZE(1);
    ASSERT_EQ(alg, exp_alg);
}

}  // namespace Svc
