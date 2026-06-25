// ======================================================================
// \title  DpCompressProcTestMain.cpp
// \author kubiak
// \brief  cpp file for DpCompressProc component test main function
// ======================================================================

#include "AbstractState.hpp"
#include "DpCompressProcTester.hpp"
#include "STest/STest/Random/Random.hpp"
#include "Svc/DpCompressProc/test/ut/Rules/Testers.hpp"

TEST(Nominal, Compressible) {
    const FwSizeType chunk_size = 4096;
    std::vector<Svc::AbstractState::Chunk> chunks = {Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0xA5),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0x55),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0x23)};
    Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);
    Svc::Testers::procRequest.testState.abstractState.param_enabled_ = true;

    Svc::Testers::procRequest.CompressTest();
}

TEST(Nominal, Disabled) {
    const FwSizeType chunk_size = 4096;
    std::vector<Svc::AbstractState::Chunk> chunks = {Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0xA5),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0x55),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0x23)};
    Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);
    Svc::Testers::procRequest.testState.abstractState.param_enabled_ = false;

    Svc::Testers::procRequest.CompressTest();
}

TEST(Nominal, MinimalCompressible) {
    const FwSizeType chunk_size = 4096;
    std::vector<Svc::AbstractState::Chunk> chunks = {
        Svc::AbstractState::Chunk(Svc::AbstractState::MINIMAL_COMPRESSED, 0xA5),
        Svc::AbstractState::Chunk(Svc::AbstractState::MINIMAL_COMPRESSED, 0x55),
        Svc::AbstractState::Chunk(Svc::AbstractState::MINIMAL_COMPRESSED, 0x23)};
    Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);
    Svc::Testers::procRequest.testState.abstractState.param_enabled_ = true;

    Svc::Testers::procRequest.CompressTest();
}

TEST(Nominal, MaximalCompressible) {
    const FwSizeType chunk_size = 4096;
    std::vector<Svc::AbstractState::Chunk> chunks = {
        Svc::AbstractState::Chunk(Svc::AbstractState::MAXIMAL_COMPRESSED, 0xA5),
        Svc::AbstractState::Chunk(Svc::AbstractState::MAXIMAL_COMPRESSED, 0x55),
        Svc::AbstractState::Chunk(Svc::AbstractState::MAXIMAL_COMPRESSED, 0x23)};
    Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);

    Svc::Testers::procRequest.CompressTest();
}

TEST(Nominal, Uncompressible) {
    const FwSizeType chunk_size = 4096;
    std::vector<Svc::AbstractState::Chunk> chunks = {Svc::AbstractState::Chunk(Svc::AbstractState::UNCOMPRESSED, 0xA5),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::UNCOMPRESSED, 0x55),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::UNCOMPRESSED, 0x23)};
    Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);
    Svc::Testers::procRequest.testState.abstractState.param_enabled_ = true;

    Svc::Testers::procRequest.CompressTest();
}

TEST(Nominal, MixedFirstCompressible) {
    const FwSizeType chunk_size = 4096;
    std::vector<Svc::AbstractState::Chunk> chunks = {Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0xA5),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::UNCOMPRESSED, 0x55),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::UNCOMPRESSED, 0x23)};
    Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);

    Svc::Testers::procRequest.CompressTest();
}

TEST(Nominal, MixedFirstUncompressible) {
    const FwSizeType chunk_size = 4096;
    std::vector<Svc::AbstractState::Chunk> chunks = {Svc::AbstractState::Chunk(Svc::AbstractState::UNCOMPRESSED, 0xA5),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::UNCOMPRESSED, 0xA4),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0x55),
                                                     Svc::AbstractState::Chunk(Svc::AbstractState::COMPRESSED, 0x23)};
    Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);
    Svc::Testers::procRequest.testState.abstractState.param_enabled_ = true;

    Svc::Testers::procRequest.CompressTest();
}

TEST(Nominal, FailureHelper) {
    const char* chunk_str = "MUUMMMUMMUMMCUUXUUUXXCCCCCCCCCXCMCMMUCUMMXXXUXMCUCCCUUXCXXXUXC";
    // const char* chunk_str = "MUUMMMUMMUMM";
    // const char* chunk_str = "MUUM";

    const FwSizeType chunk_size = 512;
    std::vector<Svc::AbstractState::Chunk> chunks;
    for (size_t idx = 0; idx < strlen(chunk_str); idx++) {
        Svc::AbstractState::Chunk chunk(static_cast<Svc::AbstractState::Compressible>(chunk_str[idx]),
                                        static_cast<U8>(idx | 0x80));
        chunks.push_back(chunk);
    };
    Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);
    Svc::Testers::procRequest.testState.abstractState.param_enabled_ = true;

    Svc::Testers::procRequest.CompressTest();
}

TEST(Nominal, Random) {
    const int iterations = 10000;

    U32 seed = STest::Random::SeedValue::getFromTime();
    // U32 seed = 87206;
    fprintf(stderr, "Random seed %u\n", seed);
    STest::Random::SeedValue::set(seed);

    // Max FwSizeStoreType is U16 so the total container size
    // should be smaller than this
    // This would be 128 512 byte chunks
    const FwSizeType chunk_size = 512;
    for (int i = 0; i < iterations; i++) {
        U32 num_chunks = STest::Random::lowerUpper(1, 100);
        std::vector<Svc::AbstractState::Chunk> chunks;

        for (U32 c = 0; c < num_chunks; c++) {
            U32 c_idx = STest::Random::lowerUpper(0, 3);
            Svc::AbstractState::Compressible compressible = c_idx == 0   ? Svc::AbstractState::COMPRESSED
                                                            : c_idx == 1 ? Svc::AbstractState::MINIMAL_COMPRESSED
                                                            : c_idx == 2 ? Svc::AbstractState::MAXIMAL_COMPRESSED
                                                                         : Svc::AbstractState::UNCOMPRESSED;
            U8 sentinel = static_cast<U8>(STest::Random::lowerUpper(1, 0xFF));
            Svc::AbstractState::Chunk chunk(compressible, sentinel);

            chunks.push_back(chunk);
        }

        Svc::Testers::procRequest.testState.abstractState.set_chunk_state(chunk_size, chunks);
        Svc::Testers::procRequest.testState.abstractState.param_enabled_ = true;

        Svc::Testers::procRequest.CompressTest();
        if (!Svc::Testers::procRequest.testState.abstractState.success_) {
            fprintf(stderr, "Failure on iteration %d\n", i);
            fprintf(stderr, " ");
            for (auto c : chunks) {
                fprintf(stderr, "%c", c.compressible);
            }
            fprintf(stderr, "\n");
        }
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
