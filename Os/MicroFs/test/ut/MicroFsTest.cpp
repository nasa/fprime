#include <gtest/gtest.h>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Os/MicroFs/MicroFs.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <Fw/Test/UnitTest.hpp>
#include <Fw/Types/String.hpp>
#include "Tester.hpp"

#if 0
TEST(Trial, TempTest) {
    Os::Tester tester;
    tester.TempTest();
}
#endif

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

#if 0
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

TEST(FileOps, OpenWriteOnceReadTwiceTest) {

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
    ASSERT_EQ(Os::File::OP_OK,f.write(buffOut,size));
    ASSERT_EQ(sizeof(buffOut)/4,size);

    // write the rest of the file
    size = sizeof(buffOut) - sizeof(buffOut)/4;
    ASSERT_EQ(Os::File::OP_OK,f.write(&buffOut[sizeof(buffOut)/4],size));
    ASSERT_EQ(sizeof(buffOut)- sizeof(buffOut)/4,size);

    // seek back to beginning
    ASSERT_EQ(Os::File::OP_OK,f.seek(0));

    // read data back in

    BYTE buffIn[testCfg.bins[0].fileSize];
    memset(buffIn,0xA5,sizeof(buffIn));

    // read part of the data back in
    size = sizeof(buffIn)/2;

    ASSERT_EQ(Os::File::OP_OK,f.read(buffIn,size));
    ASSERT_EQ(sizeof(buffIn)/2,size);

    // read the rest of the data back in
    ASSERT_EQ(Os::File::OP_OK,f.read(&buffIn[sizeof(buffIn)/2],size));
    ASSERT_EQ(sizeof(buffIn)/2,size);

    // check for equality
    ASSERT_EQ(0,memcmp(buffIn,buffOut,sizeof(buffIn)));

    f.close();

    Os::MicroFsCleanup(0,alloc);
}

TEST(FileSystemOps, OneFileReadDirectory) {
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

    // close file
    f.close();

    Fw::String listDir;
    Fw::String expectedFile;
    Fw::String files[1];
    U32 numFiles = 10; // oversize to check return

    COMMENT("Listing /");
    listDir = "/";

    // get root directory listing
    ASSERT_EQ(Os::FileSystem::OP_OK,
        Os::FileSystem::readDirectory(listDir.toChar(),1, files, numFiles));
    ASSERT_EQ(1,numFiles);

    expectedFile.format("/%s0",MICROFS_BIN_STRING);
    ASSERT_EQ(0,strcmp(expectedFile.toChar(),files[0].toChar()));

    // get file listing
    listDir.format("/%s0",MICROFS_BIN_STRING);
    Fw::String msg;
    msg.format("Listing %s",listDir.toChar());
    COMMENT(msg.toChar());
    numFiles = 10;

    ASSERT_EQ(Os::FileSystem::OP_OK,
        Os::FileSystem::readDirectory(listDir.toChar(),1, files, numFiles));

    expectedFile.format("/%s0/%s0",MICROFS_BIN_STRING,MICROFS_FILE_STRING);

    ASSERT_EQ(0,strcmp(expectedFile.toChar(),files[0].toChar()));

    // check nonexistent bin

    listDir.format("/%s1",MICROFS_BIN_STRING);
    msg.format("Listing nonexistent %s",listDir.toChar());
    COMMENT(msg.toChar());
    numFiles = 10;

    ASSERT_EQ(Os::FileSystem::NOT_DIR,
        Os::FileSystem::readDirectory(listDir.toChar(),1, files, numFiles));

}
#endif

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
