// ======================================================================
// \title  DpZLibCompressorTester.hpp
// \author kubiak
// \brief  hpp file for DpZLibCompressor component test harness implementation class
// ======================================================================

#ifndef Svc_DpZLibCompressorTester_HPP
#define Svc_DpZLibCompressorTester_HPP

#include "Svc/DpZLibCompressor/DpZLibCompressor.hpp"
#include "Svc/DpZLibCompressor/DpZLibCompressorGTestBase.hpp"

#include "Fw/Buffer/Buffer.hpp"

#include "Fw/Types/Optional.hpp"

#include <vector>

#include <zlib.h>

namespace Svc {

class DpZLibCompressorTester final : public DpZLibCompressorGTestBase {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 10;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object DpZLibCompressorTester
    DpZLibCompressorTester();

    //! Destroy object DpZLibCompressorTester
    ~DpZLibCompressorTester();

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run compression and check results
    void compress(const std::vector<U8>& data,
                  const CompressionAlgorithm exp_alg,
                  const FwSizeType min_compression,
                  const FwSizeType write_offset,
                  const FwSizeType zlib_alloc_size,
                  const I8 compression_level);

    //! Run compression and check results
    void compress_zlibiniterror(const std::vector<U8>& data,
                                const CompressionAlgorithm exp_alg,
                                const FwSizeType min_compression,
                                const FwSizeType write_offset,
                                const FwSizeType zlib_alloc_size,
                                const I8 compression_level);

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

    std::vector<U8> zlib_inflate(const Fw::Buffer& comp_buffer,
                                 const FwSizeType uncompressed_size,
                                 const FwSizeType write_offset);

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! Default handler implementation for from_bufferCompressionGet
    Fw::Buffer from_bufferCompressionGet_handler(FwIndexType portNum,  //!< The port number
                                                 FwSizeType size       //!< The requested size
    ) {
        if (do_alloc_compression_buffer_ && !active_compression_buffer_.has_value()) {
            U8* mem = new U8[size];
            active_compression_buffer_ = mem;
            return Fw::Buffer(active_compression_buffer_.value(), size);
        } else {
            return Fw::Buffer();
        }
    }

    //! Default handler implementation for from_bufferCompressionReturn
    void from_bufferCompressionReturn_handler(FwIndexType portNum,  //!< The port number
                                              Fw::Buffer& fwBuffer  //!< The buffer
    ) {
        ASSERT_TRUE(active_compression_buffer_.has_value());
        ASSERT_EQ(fwBuffer.getData(), active_compression_buffer_.value());

        delete[] active_compression_buffer_.value();
        active_compression_buffer_.reset();
    }

    //! Default handler implementation for from_bufferZLibGet
    Fw::Buffer from_bufferZLibGet_handler(FwIndexType portNum,  //!< The port number
                                          FwSizeType size       //!< The requested size
    ) {
        if (do_alloc_zlib_buffer_ && !active_zlib_buffer_.has_value()) {
            U8* mem = new U8[size];
            active_zlib_buffer_ = mem;
            return Fw::Buffer(active_zlib_buffer_.value(), size);
        } else {
            return Fw::Buffer();
        }
    }

    //! Default handler implementation for from_bufferZLibReturn
    void from_bufferZLibReturn_handler(FwIndexType portNum,  //!< The port number
                                       Fw::Buffer& fwBuffer  //!< The buffer
    ) {
        ASSERT_TRUE(active_zlib_buffer_.has_value());
        ASSERT_EQ(fwBuffer.getData(), active_zlib_buffer_.value());

        delete[] active_zlib_buffer_.value();
        active_zlib_buffer_.reset();
    }

    //! The component under test
    DpZLibCompressor component;

  public:
    bool do_alloc_compression_buffer_;
    bool do_alloc_zlib_buffer_;

  private:
    Fw::Optional<U8*> active_compression_buffer_;
    Fw::Optional<U8*> active_zlib_buffer_;
};

}  // namespace Svc

#endif
