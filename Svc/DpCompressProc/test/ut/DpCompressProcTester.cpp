// ======================================================================
// \title  DpCompressProcTester.cpp
// \author kubiak
// \brief  cpp file for DpCompressProc component test harness implementation class
// ======================================================================

#include "DpCompressProcTester.hpp"
#include "Svc/DpCompressProc/test/ut/AbstractState.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

DpCompressProcTester ::DpCompressProcTester()
    : DpCompressProcGTestBase("DpCompressProcTester", DpCompressProcTester::MAX_HISTORY_SIZE),
      component("DpCompressProc") {
    this->initComponents();
    this->connectPorts();
    this->component.loadParameters();
}

DpCompressProcTester ::~DpCompressProcTester() {}

Svc::CompressionAlgorithm DpCompressProcTester ::from_compressChunk_handler(FwIndexType portNum,
                                                                            Fw::Buffer& buffer,
                                                                            FwSizeType min_compression,
                                                                            FwSizeType write_offset) {
    this->pushFromPortEntry_compressChunk(buffer, min_compression, write_offset);

    AbstractState::Compressible c = static_cast<AbstractState::Compressible>(buffer.getData()[0]);
    U8 sentinel = buffer.getData()[1];
    FwSizeType out_size = 0;

    switch (c) {
        case AbstractState::UNCOMPRESSED:
            abstractState.update_compressed_size_state(buffer.getSize(), c);
            return Svc::CompressionAlgorithm::UNCOMPRESSED;
        case AbstractState::COMPRESSED:
            out_size = buffer.getSize() / 2 + write_offset;
            assert(out_size <= buffer.getSize());

            *(buffer.getData() + write_offset) = c;
            memset(buffer.getData() + write_offset + 1, sentinel, buffer.getSize() / 2 - 1);
            buffer.setSize(out_size);

            abstractState.update_compressed_size_state(out_size - write_offset, c);
            return Svc::CompressionAlgorithm::ZLIB_DEFLATE;
        case AbstractState::MINIMAL_COMPRESSED:

            out_size = min_compression;

            assert(out_size <= buffer.getSize());

            *(buffer.getData() + write_offset) = c;
            memset(buffer.getData() + write_offset + 1, sentinel, out_size - 1 - write_offset);
            buffer.setSize(out_size);

            abstractState.update_compressed_size_state(out_size - write_offset, c);
            return Svc::CompressionAlgorithm::ZLIB_DEFLATE;
        case AbstractState::MAXIMAL_COMPRESSED:
            out_size = 3 + write_offset;
            assert(out_size <= buffer.getSize());

            (buffer.getData() + write_offset)[0] = c;
            (buffer.getData() + write_offset)[1] = sentinel;
            (buffer.getData() + write_offset)[2] = sentinel;
            buffer.setSize(out_size);

            abstractState.update_compressed_size_state(out_size - write_offset, c);
            return Svc::CompressionAlgorithm::ZLIB_DEFLATE;
        default:
            assert(false);
    }
}

void DpCompressProcTester::uncompress_data(Fw::Buffer container_buf,
                                           const FwSizeStoreType chunk_size,
                                           std::vector<U8>& out_vec) {
    std::vector<U8> out_tmp;

    Fw::DpContainer container(0, container_buf);
    container.deserializeHeader();

    Fw::Buffer data_buf(container_buf.getData() + Fw::DpContainer::DATA_OFFSET, container.getDataSize());

    auto data_deser = data_buf.getDeserializer();

    while (data_deser.getDeserializeSizeLeft() > 0) {
        FwDpIdType id;
        Fw::SerializeStatus stat;
        stat = data_deser.deserializeTo(id);
        ASSERT_EQ(stat, Fw::FW_SERIALIZE_OK);

        ASSERT_EQ(id, component.getIdBase() + DpCompressProc::RecordId::CompressionRecord);

        FwSizeStoreType record_size;
        stat = data_deser.deserializeTo(record_size);
        ASSERT_GT(record_size, CompressionMetadata::SERIALIZED_SIZE);
        ASSERT_LE(record_size, data_deser.getDeserializeSizeLeft());

        CompressionMetadata comp_meta;
        stat = data_deser.deserializeTo(comp_meta);
        ASSERT_EQ(stat, Fw::FW_SERIALIZE_OK);

        const FwSizeStoreType payload_size =
            static_cast<FwSizeStoreType>(record_size - CompressionMetadata::SERIALIZED_SIZE);

        switch (comp_meta.get_algorithm()) {
            case CompressionAlgorithm::UNCOMPRESSED: {
                ASSERT_EQ(payload_size % chunk_size, 0);
                for (U32 c = 0; c < payload_size / chunk_size; c++) {
                    const U8* c_raw = data_deser.getBuffAddrLeft() + c * chunk_size;

                    ASSERT_EQ(c_raw[0], AbstractState::UNCOMPRESSED);
                    ASSERT_NE(c_raw[1], 0);
                    for (U32 idx = 2; idx < chunk_size; idx++) {
                        ASSERT_EQ(c_raw[idx], c_raw[1]);
                    }

                    for (U32 idx = 0; idx < chunk_size; idx++) {
                        out_tmp.push_back(c_raw[idx]);
                    }
                }
            } break;
            case CompressionAlgorithm::ZLIB_DEFLATE: {
                const U8* c_raw = data_deser.getBuffAddrLeft();

                if (!(c_raw[0] == AbstractState::COMPRESSED || c_raw[0] == AbstractState::MINIMAL_COMPRESSED ||
                      c_raw[0] == AbstractState::MAXIMAL_COMPRESSED)) {
                    ASSERT_TRUE(false) << std::hex << c_raw[0];
                }

                ASSERT_NE(c_raw[1], 0);
                for (U32 idx = 2; idx < payload_size; idx++) {
                    ASSERT_EQ(c_raw[idx], c_raw[1]);
                }

                out_tmp.push_back(c_raw[0]);
                for (U32 idx = 1; idx < chunk_size; idx++) {
                    out_tmp.push_back(c_raw[1]);
                }
            } break;
            default:
                ASSERT_TRUE(false);
        }

        data_deser.deserializeSkip(payload_size);
    }

    out_vec = out_tmp;
}

void DpCompressProcTester::test_chunks(const FwSizeStoreType chunk_size, std::vector<AbstractState::Chunk> chunks) {
    Fw::Buffer container_buf = this->abstractState.build_compress_buffer(chunk_size, chunks);

    test_chunks_helper(chunk_size, chunks, container_buf);

    delete[] container_buf.getData();
}

void DpCompressProcTester::test_chunks_helper(const FwSizeStoreType chunk_size,
                                              std::vector<AbstractState::Chunk> chunks,
                                              Fw::Buffer container_buf) {
    this->clearHistory();
    this->component.log_ACTIVITY_LO_DidNotCompress_ThrottleClear();

    abstractState.success_ = false;
    abstractState.reset_compressed_size_state();

    paramSet_CHUNK_SIZE(chunk_size, Fw::ParamValid::VALID);
    paramSet_ENABLE(Fw::Enabled::ENABLED, Fw::ParamValid::VALID);
    this->component.loadParameters();

    this->invoke_to_procRequest(0, container_buf);

    bool should_compress = false;
    for (auto chunk : chunks) {
        if (chunk.compressible != AbstractState::UNCOMPRESSED) {
            should_compress = true;
            break;
        }
    }

    if (should_compress) {
        ASSERT_EVENTS_DidNotCompress_SIZE(0);
    } else {
        ASSERT_EVENTS_DidNotCompress_SIZE(1);
    }

    Fw::DpContainer container_out(0, container_buf);
    container_out.deserializeHeader();

    if (should_compress) {
        // Expect compressed chunks to compress by 50%
        FwSizeType exp_size = this->abstractState.expected_compressed_size(chunk_size, chunks);

        ASSERT_EQ(container_out.getDataSize(), exp_size);

        std::vector<U8> uncompressed_data;
        uncompress_data(container_buf, chunk_size, uncompressed_data);
        ASSERT_EQ(uncompressed_data.size(), chunk_size * chunks.size());

        abstractState.check_uncompressed_data(uncompressed_data, chunk_size, chunks);

        ASSERT_EVENTS_CompressionComplete_SIZE(1);
    } else {
        ASSERT_EQ(container_out.getDataSize(), chunk_size * chunks.size());

        Fw::Buffer exp_buf = this->abstractState.build_compress_buffer(chunk_size, chunks);

        ASSERT_EQ(std::memcmp(container_buf.getData() + Fw::DpContainer::DATA_OFFSET,
                              exp_buf.getData() + Fw::DpContainer::DATA_OFFSET, container_out.getDataSize()),
                  0);

        delete[] exp_buf.getData();

        ASSERT_EVENTS_CompressionComplete_SIZE(0);
    }

    abstractState.success_ = true;
}

void DpCompressProcTester::test_chunks_disabled(const FwSizeStoreType chunk_size,
                                                std::vector<AbstractState::Chunk> chunks) {
    Fw::Buffer container_buf = this->abstractState.build_compress_buffer(chunk_size, chunks);

    test_chunks_helper(chunk_size, chunks, container_buf);

    this->clearHistory();

    abstractState.success_ = false;
    abstractState.reset_compressed_size_state();

    paramSet_CHUNK_SIZE(4096, Fw::ParamValid::VALID);
    paramSet_ENABLE(Fw::Enabled::DISABLED, Fw::ParamValid::VALID);
    this->component.loadParameters();

    const FwSizeType pre_comp_size = container_buf.getSize();

    this->invoke_to_procRequest(0, container_buf);

    ASSERT_EQ(container_buf.getSize(), pre_comp_size);

    delete[] container_buf.getData();
}

}  // namespace Svc
