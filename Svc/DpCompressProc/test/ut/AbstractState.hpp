// ======================================================================
// \title  AbstractState.hpp
// \author Gerik Kubiak
// \brief  Header file for abstract state
//
// \copyright
// Copyright (C) 2024 California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government sponsorship
// acknowledged.
// ======================================================================

#ifndef Svc_AbstractState_HPP
#define Svc_AbstractState_HPP

#include <cstring>
#include <vector>

#include "Fw/Types/Assert.hpp"
#include "Os/File.hpp"
#include "STest/Pick/Pick.hpp"
#include "Svc/DpCompressProc/DpCompressProc.hpp"
#include "TestUtils/OnChangeChannel.hpp"

namespace Svc {

class AbstractState {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

  public:
    // ----------------------------------------------------------------------
    // Constructors
    // ----------------------------------------------------------------------

    //! Construct an AbstractState object
    AbstractState() { param_enabled_ = true; }

    enum Compressible { UNCOMPRESSED = 'U', COMPRESSED = 'C', MINIMAL_COMPRESSED = 'M', MAXIMAL_COMPRESSED = 'X' };

    struct Chunk {
        Compressible compressible;
        U8 sentinel;

        Chunk(Compressible c, U8 s) : compressible(c), sentinel(s) {}
    };

    Fw::Buffer build_compress_buffer(FwSizeStoreType chunk_size, std::vector<Chunk> chunks);

    FwSizeType expected_compressed_size(FwSizeStoreType chunk_size, std::vector<Chunk> chunks);

    void check_uncompressed_data(std::vector<U8> data_vec, FwSizeStoreType chunk_size, std::vector<Chunk> chunks);

    void set_chunk_state(const FwSizeStoreType chunk_size, std::vector<AbstractState::Chunk> chunks) {
        chunk_size_ = chunk_size;
        chunks_ = chunks;
    }

    void reset_compressed_size_state() {
        exp_compressed_size_ = 0;
        last_uncompressed_ = false;
    }

    void update_compressed_size_state(FwSizeType compressed_size, AbstractState::Compressible compressible) {
        exp_compressed_size_ += compressed_size;

        if (compressible != AbstractState::UNCOMPRESSED || !last_uncompressed_) {
            // Expect a header for this chunk
            exp_compressed_size_ +=
                sizeof(FwDpIdType) + sizeof(FwSizeStoreType) + Svc::CompressionMetadata::SERIALIZED_SIZE;
        }
        last_uncompressed_ = compressible == AbstractState::UNCOMPRESSED;
    }

    bool param_enabled_;

  public:
    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    FwSizeStoreType chunk_size_;
    std::vector<AbstractState::Chunk> chunks_;
    bool success_;

    FwSizeType exp_compressed_size_;
    bool last_uncompressed_;
};

}  // namespace Svc

#endif
