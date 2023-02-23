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

    // seek back to beginning
    ASSERT_EQ(Os::File::OP_OK,f.seek(0));

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
    Os::File::Status stat = f.open("/bin0/file0",Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::OP_OK,stat);

    // write data to file

    BYTE buffOut[testCfg.bins[0].fileSize];
    memset(buffOut,0xFF,sizeof(buffOut));

    // write 1/4 of the file
    NATIVE_INT_TYPE size = sizeof(buffOut)/4;
    printf("S1: %d\n",size);
    ASSERT_EQ(Os::File::OP_OK,f.write(buffOut,size));
    ASSERT_EQ(sizeof(buffOut)/4,size);
    printf("S2: %d\n",size);

    // write the rest of the file
    size = sizeof(buffOut) - sizeof(buffOut)/4;
    printf("S3: %d\n",size);
    ASSERT_EQ(Os::File::OP_OK,f.write(&buffOut[size],size));
    ASSERT_EQ(sizeof(buffOut)- sizeof(buffOut)/4,size);
    printf("S4: %d\n",size);

    // seek back to beginning
    ASSERT_EQ(Os::File::OP_OK,f.seek(0));

    // read data back in
    BYTE buffIn[testCfg.bins[0].fileSize];
    size = sizeof(buffIn);
    printf("S5: %d\n",size);
    memset(buffIn,0xA5,sizeof(buffIn));
    ASSERT_EQ(Os::File::OP_OK,f.read(buffIn,size));
    ASSERT_EQ(sizeof(buffIn),size);
    printf("S6: %d\n",size);

    for (int b = 0; b < size; b++) {
        printf("%d: 0x%02X\n",b,buffIn[b]);
    }


    // check for equality
    ASSERT_EQ(0,memcmp(buffIn,buffOut,sizeof(buffIn)));


    f.close();

    Os::MicroFsCleanup(0,alloc);
}

TEST(FileOps, OpenWriteOnceReadTwiceTest) {

    return;

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
