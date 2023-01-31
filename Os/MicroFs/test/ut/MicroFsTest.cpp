#include <gtest/gtest.h>
#include <Os/MicroFs/MicroFs.hpp>

TEST(Initialization, InitTest) {
    Os::MicroFsConfig testCfg;
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
