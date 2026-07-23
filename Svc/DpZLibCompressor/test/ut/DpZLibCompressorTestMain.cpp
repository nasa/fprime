// ======================================================================
// \title  DpZLibCompressorTestMain.cpp
// \author kubiak
// \brief  cpp file for DpZLibCompressor component test main function
// ======================================================================

#include "DpZLibCompressorTester.hpp"
#include "Svc/DpCompressProc/Types/CompressionMetadataSerializableAc.hpp"

#include "STest/STest/Random/Random.hpp"

const FwSizeType header_size = sizeof(FwDpIdType) + Svc::CompressionMetadata::SERIALIZED_SIZE;

TEST(Nominal, Zeros) {
    Svc::DpZLibCompressorTester tester;

    std::vector<U8> zeros(32 * 1024);

    tester.compress(zeros, Svc::CompressionAlgorithm::ZLIB_DEFLATE, zeros.size(), header_size, 1 * 1024 * 1024, 6);
}

TEST(Nominal, Zeros_L9) {
    Svc::DpZLibCompressorTester tester;

    std::vector<U8> zeros(32 * 1024);

    tester.compress(zeros, Svc::CompressionAlgorithm::ZLIB_DEFLATE, zeros.size(), header_size, 1 * 1024 * 1024, 9);
}

TEST(Nominal, RegularData) {
    Svc::DpZLibCompressorTester tester;

    std::vector<U8> data_in(32 * 1024);
    U8 counter = 0;
    for (U8& d : data_in) {
        d = counter;
        counter++;
    }

    tester.compress(data_in, Svc::CompressionAlgorithm::ZLIB_DEFLATE, data_in.size(), header_size, 1 * 1024 * 1024, 6);
}

TEST(Uncompressed, RandomData) {
    Svc::DpZLibCompressorTester tester;

    std::vector<U8> random_in(32 * 1024);
    // Note: Not randomizing seed because I want to make sure
    // the resulting data is consistent between runs. A seed of
    // 42 results in uncompressible data
    STest::Random::SeedValue::set(42);
    for (U8& v : random_in) {
        v = static_cast<U8>(STest::Random::lowerUpper(0, 255));
    }

    tester.compress(random_in, Svc::CompressionAlgorithm::UNCOMPRESSED, random_in.size(), header_size, 1 * 1024 * 1024,
                    6);
}

TEST(Uncompressed, AboveMinSize) {
    Svc::DpZLibCompressorTester tester;

    std::vector<U8> zeros(32 * 1024);

    tester.compress(zeros, Svc::CompressionAlgorithm::UNCOMPRESSED,
                    // 32 KB of zeros compresses to 52 bytes
                    51, header_size, 1 * 1024 * 1024, 6);
}

TEST(OffNominal, BadCompressionAlloc) {
    Svc::DpZLibCompressorTester tester;

    tester.do_alloc_compression_buffer_ = false;
    std::vector<U8> zeros(32 * 1024);

    tester.compress(zeros, Svc::CompressionAlgorithm::UNCOMPRESSED, zeros.size(), header_size, 1 * 1024 * 1024, 6);
}

TEST(OffNominal, BadZLibAlloc) {
    Svc::DpZLibCompressorTester tester;

    tester.do_alloc_zlib_buffer_ = false;
    std::vector<U8> zeros(32 * 1024);

    tester.compress(zeros, Svc::CompressionAlgorithm::UNCOMPRESSED, zeros.size(), header_size, 1 * 1024 * 1024, 6);
}

TEST(OffNominal, InsufficientZLibMem) {
    Svc::DpZLibCompressorTester tester;

    std::vector<U8> zeros(32 * 1024);

    tester.compress_zlibiniterror(zeros, Svc::CompressionAlgorithm::UNCOMPRESSED, zeros.size(), header_size,
                                  // Need 268096 bytes for compression at level 6
                                  268095, 6);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
