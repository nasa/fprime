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

void DpCompressProc::serializeCompressionHeader(Fw::LinearBufferBase& serializer,
                                                const FwSizeStoreType compressed_payload_size,
                                                const CompressionMetadata& metadata) {
    const FwDpIdType record_id = this->getIdBase() + RecordId::CompressionRecord;

    Fw::SerializeStatus ok;

    ok = serializer.serializeFrom(record_id);
    FW_ASSERT(ok == Fw::FW_SERIALIZE_OK, ok);
    ok = serializer.serializeFrom(
        static_cast<FwSizeStoreType>(compressed_payload_size + CompressionMetadata::SERIALIZED_SIZE));
    FW_ASSERT(ok == Fw::FW_SERIALIZE_OK, ok);
    ok = serializer.serializeFrom(metadata);
    FW_ASSERT(ok == Fw::FW_SERIALIZE_OK, ok);
}

void DpCompressProc ::procRequest_handler(FwIndexType portNum, Fw::Buffer& fwBuffer) {
    Fw::ParamValid param_valid;
    Fw::Enabled en_compression = paramGet_ENABLE(param_valid);
    FW_ASSERT((param_valid == Fw::ParamValid::DEFAULT) || (param_valid == Fw::ParamValid::VALID), param_valid);
    if (en_compression == Fw::Enabled::DISABLED) {
        // Bypass compression
        return;
    }

    if (!isConnected_compressChunk_OutputPort(0)) {
        return;
    }

    Fw::DpContainer container(0, fwBuffer);
    container.deserializeHeader();

    FwSizeType prm_chunk_size = paramGet_CHUNK_SIZE(param_valid);
    FW_ASSERT((param_valid == Fw::ParamValid::DEFAULT) || (param_valid == Fw::ParamValid::VALID), param_valid);

    if (prm_chunk_size > container.getDataSize()) {
        prm_chunk_size = container.getDataSize();
    }
    const FwSizeType max_chunk_size = prm_chunk_size;
    FW_ASSERT(max_chunk_size <= std::numeric_limits<FwSizeStoreType>::max(),
              static_cast<FwAssertArgType>(max_chunk_size));
    if (max_chunk_size <= 0) {
        // Chunk size is invalid. Give up
        return;
    }

    Fw::Buffer data_buffer(fwBuffer.getData() + Fw::DpContainer::DATA_OFFSET,
                           fwBuffer.getSize() - Fw::DpContainer::MIN_PACKET_SIZE);
    FW_ASSERT(data_buffer.getSize() > 0, static_cast<FwAssertArgType>(data_buffer.getSize()));

    auto data_deser = data_buffer.getDeserializer();
    auto data_reser = data_buffer.getSerializer();

    // Compression record serialized size
    const FwSizeType compression_header_size =
        sizeof(FwDpIdType) + sizeof(FwSizeStoreType) + CompressionMetadata::SERIALIZED_SIZE;

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
        // grow to accommodate this segment
        LAST_UNCOMPRESSED
    } state;

    state = INIT;

    FwSizeStoreType uncompressed_size = 0;
    U8* uncompressed_head = nullptr;
    Fw::SerializeStatus ser_stat = Fw::FW_SERIALIZE_OK;

    while (data_deser.getDeserializeSizeLeft() > 0) {
        // Chunk size for this iteration
        const FwSizeType chunk_size_tmp = (data_deser.getDeserializeSizeLeft() < max_chunk_size)
                                              ? static_cast<FwSizeType>(data_deser.getDeserializeSizeLeft())
                                              : max_chunk_size;

        // Ensure that FwSizeStoreType is large enough to hold the chunk_size
        FW_ASSERT(chunk_size_tmp > 0, static_cast<FwAssertArgType>(chunk_size_tmp));
        FW_ASSERT(chunk_size_tmp <= std::numeric_limits<FwSizeStoreType>::max(),
                  static_cast<FwAssertArgType>(chunk_size_tmp));
        const FwSizeStoreType chunk_size = static_cast<FwSizeStoreType>(chunk_size_tmp);
        // a const U8 * and I need to mutable U8*
        U8* data_offset = data_buffer.getData() + (data_buffer.getSize() - data_deser.getDeserializeSizeLeft());

        Fw::Buffer compression_buffer(data_offset, chunk_size);

        // Jump the deserializer to mark the data as read
        ser_stat = data_deser.deserializeSkip(chunk_size);
        FW_ASSERT(ser_stat == Fw::FW_SERIALIZE_OK, ser_stat);

        CompressionAlgorithm alg = CompressionAlgorithm::UNCOMPRESSED;
        FwSizeType min_compression = 0;
        FwSizeType compression_offset = 0;

        // Only attempt compression on chunks larger than
        // 4 times the header size. Sizes below 3 times the header
        // size could trigger negative sizes in the min_compression
        // calculation
        if (chunk_size > 4 * compression_header_size) {
            switch (state) {
                case INIT:
                    // Need room for two headers.
                    //  1. This compressed chunk
                    //  2. A potential uncompressed chunk that follows
                    // This is assuming that the initial chunk can be compressed
                    min_compression = chunk_size - 2 * compression_header_size;

                    // Compression needs to occur in the buffer with enough
                    // space for the this header
                    compression_offset = compression_header_size;
                    break;
                case PRE_COMMIT:
                    // Need room for three headers.
                    //  1. The initial uncompressed chunk
                    //  2. This compressed chunk
                    //  3. A potential uncompressed chunk that follows
                    min_compression = chunk_size - 3 * compression_header_size;

                    // Compression needs to occur in the buffer with enough
                    // space for the this header AND the initial uncompressed header
                    compression_offset = 2 * compression_header_size;
                    break;
                case LAST_UNCOMPRESSED:
                case LAST_COMPRESSED:
                    // Need room for two headers.
                    //  1. The next compressed chunk
                    //  2. A potential uncompressed chunk
                    // Can take advantage of compression that has occurred
                    // before this point
                    // Note: Could keep track of the number of available spare bytes
                    // in the buffer, but that adds a complexity for very little benefit
                    min_compression = chunk_size - (2 * compression_header_size);
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

            FW_ASSERT(min_compression <= chunk_size, static_cast<FwAssertArgType>(min_compression),
                      static_cast<FwAssertArgType>(chunk_size));

            FW_ASSERT(compression_offset <= chunk_size, static_cast<FwAssertArgType>(compression_offset),
                      static_cast<FwAssertArgType>(chunk_size));

            alg = compressChunk_out(0, compression_buffer, min_compression, compression_offset);

            if (alg != CompressionAlgorithm::UNCOMPRESSED) {
                FW_ASSERT(compression_buffer.getSize() <= min_compression);
            }
        }

        if (alg != CompressionAlgorithm::UNCOMPRESSED) {
            // Data was compressed

            U8* const comp_data_ptr = compression_buffer.getData() + compression_offset;

            const FwSizeType compressed_size_tmp =
                static_cast<FwSizeType>(compression_buffer.getSize()) - static_cast<FwSizeType>(compression_offset);
            FW_ASSERT(compressed_size_tmp <= std::numeric_limits<FwSizeStoreType>::max());
            FW_ASSERT(compressed_size_tmp < chunk_size);
            const FwSizeStoreType compressed_size = static_cast<FwSizeStoreType>(compressed_size_tmp);

            // If the first chunk is compressible treat the state as
            // if it was LAST_COMPRESSED. No need to perform the special
            // move and header serialization in PRE_COMMIT
            if (state == INIT) {
                state = LAST_COMPRESSED;
            }

            const FwSizeType deser_loc = data_deser.getSize() - data_deser.getDeserializeSizeLeft();
            switch (state) {
                case PRE_COMMIT:
                    // Case A.
                    // 1. Memmove uncompressed data so far by the size of compressed header
                    // 2. Prepend a header to the uncompressed data
                    // 3. Write a compressed header to the compressed data
                    // 4. Serialize compressed data

                    // Assert the next few serialization operations will stay within the
                    // data that has been read so far
                    FW_ASSERT(deser_loc >=
                                  (uncompressed_size +
                                   2 * static_cast<FwSizeType>(CompressionMetadata::SERIALIZED_SIZE) + compressed_size),
                              static_cast<FwAssertArgType>(deser_loc), uncompressed_size,
                              CompressionMetadata::SERIALIZED_SIZE, compressed_size);
                    std::memmove(data_buffer.getData() + compression_header_size, data_buffer.getData(),
                                 uncompressed_size);

                    // Serialize the header bytes to the front of the data
                    serializeCompressionHeader(data_reser, uncompressed_size,
                                               CompressionMetadata(CompressionAlgorithm::UNCOMPRESSED));

                    // Move the serializer past the uncompressed chunk manually
                    ser_stat = data_reser.serializeSkip(uncompressed_size);
                    FW_ASSERT(ser_stat == Fw::FW_SERIALIZE_OK, ser_stat);

                    serializeCompressionHeader(data_reser, compressed_size, CompressionMetadata(alg));

                    ser_stat = data_reser.serializeFrom(comp_data_ptr, compressed_size, Fw::Serialization::OMIT_LENGTH);
                    FW_ASSERT(ser_stat == Fw::FW_SERIALIZE_OK, ser_stat);
                    break;
                case LAST_COMPRESSED:
                    // Case B
                    // 1. Write header at data_reser location
                    // 2. Serialize compressed data
                    serializeCompressionHeader(data_reser, compressed_size, CompressionMetadata(alg));

                    ser_stat = data_reser.serializeFrom(comp_data_ptr, compressed_size, Fw::Serialization::OMIT_LENGTH);
                    FW_ASSERT(ser_stat == Fw::FW_SERIALIZE_OK, ser_stat);
                    break;
                case LAST_UNCOMPRESSED:
                    // Case E
                    // 1. Write header for uncompressed data.
                    //    data_reser has been kept at this location
                    // 2. Serialize uncompressed data
                    // 3. Write header for compressed data at data_reser location
                    // 4. Serialize compressed data
                    serializeCompressionHeader(data_reser, uncompressed_size,
                                               CompressionMetadata(CompressionAlgorithm::UNCOMPRESSED));

                    FW_ASSERT(uncompressed_head != nullptr);
                    ser_stat =
                        data_reser.serializeFrom(uncompressed_head, uncompressed_size, Fw::Serialization::OMIT_LENGTH);
                    FW_ASSERT(ser_stat == Fw::FW_SERIALIZE_OK, ser_stat);

                    serializeCompressionHeader(data_reser, compressed_size, CompressionMetadata(alg));

                    ser_stat = data_reser.serializeFrom(comp_data_ptr, compressed_size, Fw::Serialization::OMIT_LENGTH);
                    FW_ASSERT(ser_stat == Fw::FW_SERIALIZE_OK, ser_stat);
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
                    // Case C - First incompressible chunk
                    //
                    // 1. Mark the location of the start of incompressible chunks
                    uncompressed_head = compression_buffer.getData();

                    state = LAST_UNCOMPRESSED;
                    break;
                case LAST_UNCOMPRESSED:
                    // Case D - Continued sequence of incompressible chunks
                    // No work. Keep increasing track of uncompressed_size
                    state = LAST_UNCOMPRESSED;
                    break;
                default:
                    FW_ASSERT(false, state);
                    break;
            }
            uncompressed_size = static_cast<FwSizeStoreType>(uncompressed_size + chunk_size);
        }

        // Confirm that the serialized location has not jumped ahead of the deserialize location
        const FwSizeType ser_loc = data_reser.getSize();
        const FwSizeType deser_loc = static_cast<uintptr_t>(data_deser.getBuffAddrLeft() - data_buffer.getData());
        FW_ASSERT(ser_loc <= deser_loc, static_cast<FwAssertArgType>(ser_loc), static_cast<FwAssertArgType>(deser_loc));
    }

    switch (state) {
        case PRE_COMMIT:
            // Never compressed a chunk. Return the buffer
            // unchanged
            log_ACTIVITY_LO_DidNotCompress(container.getId(), container.getDataSize());
            break;
        case LAST_UNCOMPRESSED:
            // Need to serialize the last bit of uncompressed data
            // 1. Write header for uncompressed data.
            //    data_reser has been kept at this location
            // 2. Serialize uncompressed data
            serializeCompressionHeader(data_reser, uncompressed_size,
                                       CompressionMetadata(CompressionAlgorithm::UNCOMPRESSED));

            FW_ASSERT(uncompressed_head != nullptr);
            ser_stat = data_reser.serializeFrom(uncompressed_head, uncompressed_size, Fw::Serialization::OMIT_LENGTH);
            FW_ASSERT(ser_stat == Fw::FW_SERIALIZE_OK, ser_stat);

            /* FALLTHRU */
        case LAST_COMPRESSED:
            // Update buffer and header size to be consistent
            // with the compression achieved

            {
                FwSizeType comp_data_size = data_reser.getSize();
                log_DIAGNOSTIC_CompressionComplete(container.getId(), container.getDataSize(), comp_data_size);

                container.setDataSize(comp_data_size);
                container.serializeHeader();
            }
            break;
        default:
            FW_ASSERT(false, state);
    }
}

}  // namespace Svc
