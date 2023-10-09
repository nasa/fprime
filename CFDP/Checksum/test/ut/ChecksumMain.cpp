// ----------------------------------------------------------------------
// Main.cpp 
// ----------------------------------------------------------------------

#include "gtest/gtest.h"

#include "CFDP/Checksum/Checksum.hpp"

using namespace CFDP;

const U8 data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };

const U32 expectedValue = 
  (data[0] << 3*8) + (data[1] << 2*8) + (data[2] << 1*8) + data[3] +
  (data[4] << 3*8) + (data[5] << 2*8) + (data[6] << 1*8) + data[7];

TEST(Checksum, OnePacket) {
  Checksum checksum;
  checksum.update(data, 0, 8);
  ASSERT_EQ(expectedValue, checksum.getValue());
}

TEST(Checksum, TwoPacketsAligned) {
  Checksum checksum;
  checksum.update(&data[0], 0, 4);
  checksum.update(&data[4], 4, 4);
  ASSERT_EQ(expectedValue, checksum.getValue());
}

TEST(Checksum, TwoPacketsUnaligned1) {
  Checksum checksum;
  checksum.update(&data[0], 0, 3);
  checksum.update(&data[3], 3, 5);
  ASSERT_EQ(expectedValue, checksum.getValue());
}

TEST(Checksum, TwoPacketsUnaligned2) {
  Checksum checksum;
  checksum.update(&data[0], 0, 5);
  checksum.update(&data[5], 5, 3);
  ASSERT_EQ(expectedValue, checksum.getValue());
}

TEST(Checksum, ThreePackets) {
  Checksum checksum;
  checksum.update(&data[0], 0, 2);
  checksum.update(&data[2], 2, 3);
  checksum.update(&data[5], 5, 3);
  ASSERT_EQ(expectedValue, checksum.getValue());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

