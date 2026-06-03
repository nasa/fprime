// ======================================================================
// \title  DpCompressProc.cpp
// \author kubiak
// \brief  cpp file for DpCompressProc component implementation class
// ======================================================================

#include <cstring>

#include "Svc/DpCompressProc/DpCompressProc.hpp"

#include <Fw/Dp/DpContainer.hpp>

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

DpCompressProc ::DpCompressProc(const char* const compName) : DpCompressProcComponentBase(compName) {}

DpCompressProc ::~DpCompressProc() {}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void DpCompressProc::
    serializeCompressionHeader(
        Fw::LinearBufferBase& serializer,
        const FwSizeStoreType compressed_payload_size,
        const CompressionMetadata& metadata
) {
    const FwDpIdType record_id = this->getIdBase() + RecordId::CompressionRecord;

    // TODO: Assert this size wont overflow
    serializer.serializeFrom(record_id);
    serializer.serializeFrom(static_cast<FwSizeStoreType>(compressed_payload_size + CompressionMetadata::SERIALIZED_SIZE));
    serializer.serializeFrom(metadata);
}

void DpCompressProc ::procRequest_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {

    Fw::ParamValid param_valid;
    Fw::Enabled en_compression = paramGet_ENABLE(param_valid);
    FW_ASSERT((param_valid == Fw::ParamValid::DEFAULT) ||
              (param_valid == Fw::ParamValid::VALID), param_valid);
    if (en_compression == Fw::Enabled::DISABLED) {
        // Bypass compression
        return;
    }

    if (!isConnected_compressChunk_OutputPort(0)) {
        return;
    }

    Fw::DpContainer container(0, fwBuffer);
    container.deserializeHeader();

    FwSizeStoreType prm_chunk_size = paramGet_CHUNK_SIZE(param_valid);
        FW_ASSERT((param_valid == Fw::ParamValid::DEFAULT) ||
                  (param_valid == Fw::ParamValid::VALID), param_valid);

    if (prm_chunk_size > container.getDataSize()) {
        prm_chunk_size = static_cast<FwSizeStoreType>(container.getDataSize());
    }
    const FwSizeStoreType max_chunk_size = prm_chunk_size;
    FW_ASSERT(max_chunk_size != 0);

    Fw::Buffer data_buffer(fwBuffer.getData() + Fw::DpContainer::DATA_OFFSET,
                           fwBuffer.getSize() - Fw::DpContainer::MIN_PACKET_SIZE);
    FW_ASSERT(data_buffer.getSize() > 0,
              static_cast<FwAssertArgType>(data_buffer.getSize()));
  
    auto data_deser = data_buffer.getDeserializer();
    auto data_reser = data_buffer.getSerializer();

    // Keep track of the number of spare bytes available
    // in the original buffer.
    FwSizeType spare_byte_counter = 0;

    // Compression record serialized size
    const FwSizeType compression_header_size =
        sizeof(FwDpIdType) +
        sizeof(FwSizeStoreType) +
        CompressionMetadata::SERIALIZED_SIZE;

    // DpCompressProc processing state machine.
    // Tracks the processing steps that need to occur for
    // each chunk, whether is is compressed or not
    enum {
        // Initial state. First processing state will
        // either be PRE_COMMIT or LAST_COMPRESSED depending
        // on the compression of the first chunk.
        INIT,

        // Have not committed to returning a compressed
        // data product. Stay in this state until at least
        // one chunk is found to be compressible
        PRE_COMMIT,

        // The last chunk was compressed.
        // The next chunk will need to have its own
        // header prepended
        LAST_COMPRESSED,

        // The last chunk was uncompressed.
        // If more uncompressed chunks are found then the
        // size of the last uncompressed segment needs to
        // grow to accomidate this segment
        LAST_UNCOMPRESSED
    } state;

    state = INIT;

    FwSizeStoreType uncompressed_size = 0;
    U8* uncompressed_head = nullptr;

    while (data_deser.getDeserializeSizeLeft() > 0) {

        // Chunk size for this iteration
        const FwSizeStoreType chunk_size =
            // TODO: Check these casts
            static_cast<FwSizeStoreType>(data_deser.getDeserializeSizeLeft()) < max_chunk_size ?
            static_cast<FwSizeStoreType>(data_deser.getDeserializeSizeLeft()) : max_chunk_size;

        // a const U8 * and I need to mutable U8*
        U8* data_offset = data_buffer.getData() +
            (data_buffer.getSize() - data_deser.getDeserializeSizeLeft());

        Fw::Buffer compression_buffer(data_offset, chunk_size);
 
        CompressionAlgorithm alg = CompressionAlgorithm::UNCOMPRESSED;
        FwSizeType min_compression = 0;
        FwSizeType compression_offset = 0;

        // Only attempt compression on chunks larger than
        // 4 times the header size. Sizes below 3 times the header
        // size could trigger negative sizes in the min_compression
        // calculation
        if (chunk_size > 4*compression_header_size) {

            switch (state) {
                case INIT:
                    // Need room for two headers.
                    //  1. This compressed chunk
                    //  2. A potential uncompressed chunk that follows
                    // This is assuming that the initial chunk can be compressed
                    min_compression = chunk_size - 2*compression_header_size;

                    // Compression needs to occur in the buffer with enough
                    // space for the this header
                    compression_offset = compression_header_size;
                    break;
                case PRE_COMMIT:
                    // Need room for three headers.
                    //  1. The initial uncompressed chunk
                    //  2. This compressed chunk
                    //  3. A potential uncompressed chunk that follows
                    min_compression = chunk_size - 3*compression_header_size;

                    // Compression needs to occur in the buffer with enough
                    // space for the this header AND the initial uncompressed header
                    compression_offset = 2*compression_header_size;
                    break;
                case LAST_UNCOMPRESSED:
                case LAST_COMPRESSED:
                    // Need room for two headers.
                    //  1. The next compressed chunk
                    //  2. A potential uncompressed chunk
                    // Can take advantage of compression that has occurred
                    // before this point
                    if (spare_byte_counter > 2*compression_header_size) {
                        min_compression = chunk_size;
                    } else {
                        min_compression = chunk_size - (2*compression_header_size - spare_byte_counter);
                    }
                    // Compression needs to occur in the buffer with enough
                    // space for the this header
                    compression_offset = compression_header_size;
                    break;
                default:
                    FW_ASSERT(false, state);
                    break;
            }

            if (min_compression > chunk_size - compression_offset) {
                min_compression = chunk_size - compression_offset;
            }

            FW_ASSERT(min_compression <= chunk_size,
                      static_cast<FwAssertArgType>(min_compression),
                      static_cast<FwAssertArgType>(chunk_size));

            FW_ASSERT(compression_offset <= chunk_size,
                      static_cast<FwAssertArgType>(compression_offset),
                      static_cast<FwAssertArgType>(chunk_size));

            alg = compressChunk_out(0,
                                    compression_buffer,
                                    min_compression,
                                    compression_offset);

            if (alg != CompressionAlgorithm::UNCOMPRESSED) {
                FW_ASSERT(compression_buffer.getSize() <= min_compression);
            }
        }

        if (alg != CompressionAlgorithm::UNCOMPRESSED) {
            // Data was compressed

            U8* const comp_data_ptr = compression_buffer.getData() + compression_offset;
            // TODO: Check these casts. Really should reject packet if compression_buffer is near FwSizeStoreType::max
            const FwSizeStoreType comp_size = static_cast<FwSizeStoreType>(compression_buffer.getSize()) - static_cast<FwSizeStoreType>(compression_offset);

            // TODO: Does this take care of shrinking this size of spare_byte_counter correctly?
            spare_byte_counter += chunk_size - comp_size;

            // If the first chunk is compressible treat the state as
            // if it was LAST_COMPRESSED. No need to perform the special
            // move and header serialization in PRE_COMMIT
            if (state == INIT) {
                state = LAST_COMPRESSED;
            }

            switch (state) {
                case PRE_COMMIT:
                    // Case A.
                    // 1. Memmove uncompressed data so far by the size of compressed header
                    // 2. Prepend a header to the uncompressed data
                    // 3. Write a compressed header to the compressed data
                    // 4. Serialize compressed data
                    // TODO: Assert that this will not write past current deser location
                    std::memmove(data_buffer.getData() + compression_header_size,
                                 data_buffer.getData(),
                                 uncompressed_size);

                    // Serialize the header bytes to the front of the data
                    serializeCompressionHeader(
                        data_reser,
                        uncompressed_size,
                        CompressionMetadata(CompressionAlgorithm::UNCOMPRESSED)
                    );

                    // Move the serializer past the uncompressed chunk manually
                    data_reser.serializeSkip(uncompressed_size);

                    serializeCompressionHeader(
                        data_reser,
                        comp_size,
                        CompressionMetadata(alg)
                    );

                    data_reser.serializeFrom(comp_data_ptr,
                                             comp_size,
                                             Fw::Serialization::OMIT_LENGTH);
                    break;
                case LAST_COMPRESSED:
                    // Case B
                    // 1. Write header at data_reser location
                    // 2. Serialize compressed data
                    serializeCompressionHeader(
                        data_reser,
                        comp_size,
                        CompressionMetadata(alg)
                    );

                    data_reser.serializeFrom(comp_data_ptr,
                                             comp_size,
                                             Fw::Serialization::OMIT_LENGTH);
                    break;
                case LAST_UNCOMPRESSED:
                    // Case E
                    // 1. Write header for uncompressed data.
                    //    data_reser has been kept at this location
                    // 2. Serialize uncompressed data
                    // 3. Write header for compressed data at data_reser location
                    // 4. Serialize compressed data
                    serializeCompressionHeader(
                        data_reser,
                        uncompressed_size,
                        CompressionMetadata(CompressionAlgorithm::UNCOMPRESSED)
                    );

                    FW_ASSERT(uncompressed_head != nullptr);
                    data_reser.serializeFrom(uncompressed_head,
                                             uncompressed_size,
                                             Fw::Serialization::OMIT_LENGTH);

                    serializeCompressionHeader(
                        data_reser,
                        comp_size,
                        CompressionMetadata(alg)
                    );

                    data_reser.serializeFrom(comp_data_ptr,
                                             comp_size,
                                             Fw::Serialization::OMIT_LENGTH);
                    break;
                default:
                    FW_ASSERT(false, state);
                    break;
            }

            // Last chunk was compressed. Remove history of uncompressed data
            uncompressed_size = 0;
            uncompressed_head = nullptr;
            state = LAST_COMPRESSED;
        } else {
            // Data was uncompressed

            // If the first chunk is not compressible treat the state as
            // if it was PRE_COMMIT
            if (state == INIT) {
                state = PRE_COMMIT;
            }

            switch (state) {
                case PRE_COMMIT:
                    // No work.
                    // Keep looking for a compressible chunk in order to commit
                    // to a compressed structure
                    state = PRE_COMMIT;
                    break;
                case LAST_COMPRESSED:
                    // Case C - First uncompressable chunk
                    //
                    // 1. Mark the location of the start of uncompressible chunks
                    uncompressed_head = compression_buffer.getData();

                    state = LAST_UNCOMPRESSED;
                    break;
                case LAST_UNCOMPRESSED:
                    // Case D - Continued sequence of uncompressable chunks
                    // No work. Keep increasing track of uncompressed_size
                    state = LAST_UNCOMPRESSED;
                    break;
                default:
                    FW_ASSERT(false, state);
                    break;
            }
            uncompressed_size += chunk_size;
        }

        // Jump the deserializer
        data_deser.deserializeSkip(chunk_size);

        // Confirm that the serialized location has not jumped ahead of the deserialize location
        const FwSizeType ser_loc = data_reser.getSize();
        const FwSizeType deser_loc = static_cast<uintptr_t>(data_deser.getBuffAddrLeft() - data_buffer.getData());
        FW_ASSERT(ser_loc <= deser_loc,
                  static_cast<FwAssertArgType>(ser_loc),
                  static_cast<FwAssertArgType>(deser_loc));
    }

    switch (state) {
        case PRE_COMMIT:
            // Never compressed a chunk. Return the buffer
            // unchanged
            log_ACTIVITY_LO_DidNotCompress(container.getId(),
                                           container.getDataSize());
        break;
        case LAST_UNCOMPRESSED:
            // Need to serialize the last bit of uncompressed data
            // 1. Write header for uncompressed data.
            //    data_reser has been kept at this location
            // 2. Serialize uncompressed data
            serializeCompressionHeader(
                data_reser,
                uncompressed_size,
                CompressionMetadata(CompressionAlgorithm::UNCOMPRESSED)
            );

            FW_ASSERT(uncompressed_head != nullptr);
            data_reser.serializeFrom(uncompressed_head,
                                     uncompressed_size,
                                     Fw::Serialization::OMIT_LENGTH);

            /* FALLTHRU */
        case LAST_COMPRESSED:
            // Update buffer and header size to be consistent
            // with the compression achieved

            {
            FwSizeType comp_data_size = data_reser.getSize();
            log_DIAGNOSTIC_CompressionComplete(container.getId(),
                                               container.getDataSize(),
                                               comp_data_size);

            container.setDataSize(comp_data_size);
            container.serializeHeader();
            }
        break;
        default:
            FW_ASSERT(false, state);
    }
}

}  // namespace Svc
