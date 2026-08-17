// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include <vector>

#include "gtest/gtest.h"

#include "CFDP/Checksum/Checksum.hpp"

using namespace CFDP;

const U8 data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};

const U32 expectedValue = (data[0] << 3 * 8) + (data[1] << 2 * 8) + (data[2] << 1 * 8) + data[3] + (data[4] << 3 * 8) +
                          (data[5] << 2 * 8) + (data[6] << 1 * 8) + data[7];

// Each parameter is a list of split offsets partitioning data[0..8)
class ChecksumSplits : public ::testing::TestWithParam<std::vector<U32>> {};

TEST_P(ChecksumSplits, Accumulate) {
    const std::vector<U32>& splits = this->GetParam();
    Checksum checksum;
    U32 offset = 0;
    for (U32 split : splits) {
        checksum.update(&data[offset], offset, split - offset);
        offset = split;
    }
    checksum.update(&data[offset], offset, 8 - offset);
    ASSERT_EQ(expectedValue, checksum.getValue());
}

INSTANTIATE_TEST_SUITE_P(Checksum,
                         ChecksumSplits,
                         ::testing::Values(std::vector<U32>({}),
                                           std::vector<U32>({4}),
                                           std::vector<U32>({3}),
                                           std::vector<U32>({5}),
                                           std::vector<U32>({2, 5})));

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
