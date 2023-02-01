#include <gtest/gtest.h>
#include <Os/File.hpp>
#include <Os/MicroFs/MicroFs.hpp>
#include <Fw/Types/MallocAllocator.hpp>

TEST(Initialization, InitTest) {
    Fw::MallocAllocator alloc;
    Os::MicroFsConfig testCfg;
    testCfg.numBins = 1;
    testCfg.bins[0].fileSize = 100;
    testCfg.bins[0].numFiles = 1;
    Os::MicroFsInit(testCfg,0,alloc);

    Os::MicroFsCleanup(0,alloc);

}

TEST(FileOps, OpenWriteTest) {
    Fw::MallocAllocator alloc;
    Os::MicroFsConfig testCfg;
    testCfg.numBins = 1;
    testCfg.bins[0].fileSize = 100;
    testCfg.bins[0].numFiles = 1;
    Os::MicroFsInit(testCfg,0,alloc);

    Os::File f;
    f.open("/bin0/file0",Os::File::OPEN_READ);


    Os::MicroFsCleanup(0,alloc);
}

TEST(FileOps, OpenWriteReadTest) {
    Fw::MallocAllocator alloc;
    Os::MicroFsConfig testCfg;
    testCfg.numBins = 1;
    testCfg.bins[0].fileSize = 100;
    testCfg.bins[0].numFiles = 1;
    Os::MicroFsInit(testCfg,0,alloc);

    Os::File f;
    f.open("/bin0/file0",Os::File::OPEN_READ);

    Os::MicroFsCleanup(0,alloc);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
