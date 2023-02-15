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

TEST(FileOps, OpenWriteReadTest) {
    Fw::MallocAllocator alloc;
    Os::MicroFsConfig testCfg;
    testCfg.numBins = 1;
    testCfg.bins[0].fileSize = 100;
    testCfg.bins[0].numFiles = 1;
    Os::MicroFsInit(testCfg,0,alloc);

    Os::File f;
    Os::File::Status stat = f.open("/bin0/file0",Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::OP_OK,stat);

    // write data to file

    BYTE buffOut[testCfg.bins[0].fileSize];
    memset(buffOut,0xFF,sizeof(buffOut));
    NATIVE_INT_TYPE size = sizeof(buffOut);
    ASSERT_EQ(Os::File::OP_OK,f.write(buffOut,size));
    ASSERT_EQ(sizeof(buffOut),size);

    // read data back in

    BYTE buffIn[testCfg.bins[0].fileSize];
    size = sizeof(buffIn);
    memset(buffIn,0xA5,sizeof(buffIn));
    ASSERT_EQ(Os::File::OP_OK,f.read(buffIn,size));
    ASSERT_EQ(sizeof(buffIn),size);

    // check for equality
    ASSERT_EQ(0,memcmp(buffIn,buffOut,sizeof(buffIn)));

    f.close();

    Os::MicroFsCleanup(0,alloc);
}

TEST(FileOps, OpenWriteTwiceReadOnceTest) {

    Fw::MallocAllocator alloc;
    Os::MicroFsConfig testCfg;
    testCfg.numBins = 1;
    testCfg.bins[0].fileSize = 100;
    testCfg.bins[0].numFiles = 1;
    Os::MicroFsInit(testCfg,0,alloc);

    Os::File f;
    f.open("/bin0/file0",Os::File::OPEN_CREATE);

    Os::MicroFsCleanup(0,alloc);
}

TEST(FileOps, OpenWriteOnceReadTwiceTest) {

    Fw::MallocAllocator alloc;
    Os::MicroFsConfig testCfg;
    testCfg.numBins = 1;
    testCfg.bins[0].fileSize = 100;
    testCfg.bins[0].numFiles = 1;
    Os::MicroFsInit(testCfg,0,alloc);

    Os::File f;
    f.open("/bin0/file0",Os::File::OPEN_CREATE);

    Os::MicroFsCleanup(0,alloc);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
