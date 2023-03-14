#include <gtest/gtest.h>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Os/MicroFs/MicroFs.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <Fw/Test/UnitTest.hpp>
#include <Fw/Types/String.hpp>
#include "Tester.hpp"


TEST(Initialization, InitTest) {
    Os::Tester tester;
    tester.InitTest();
}

TEST(FileOps, OpenWriteReadTest) {
    Os::Tester tester;
    tester.OpenWriteReadTest();
}

TEST(FileOps, OpenWriteTwiceReadOnceTest) {
    Os::Tester tester;
    tester.OpenWriteTwiceReadOnceTest();
}

TEST(FileOps, OpenWriteOnceReadTwiceTest) {
    Os::Tester tester;
    tester.OpenWriteOnceReadTwiceTest();
}

TEST(FileSystemOps, OneFileReadDirectory) {
    Os::Tester tester;
    tester.OneFileReadDirectory();
}

TEST(FileSystemOps, OpenFreeSpaceTest) {
    Os::Tester tester;
    tester.OpenFreeSpaceTest();
}

TEST(FileOps, OpenStressTest) {
    Os::Tester tester;
    tester.OpenStressTest();
}

#if 0

TEST(FileOps, OddTest) {
    Os::Tester tester;
    tester.OddTest();
}
#endif

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
