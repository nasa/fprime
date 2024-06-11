#include <gtest/gtest.h>
#include <Os/FileSystem.hpp>
#include <Os/File.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Types/String.hpp>

#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>

void testTestFileSystem() {

    Os::FileSystem::Status file_sys_status;
    Os::File::Status file_status;
    Os::File test_file = Os::File();
    const char test_file_name1[] = "test_file1";
    const char test_file_name2[] = "test_file2";
    const char test_dir1[] = "./test_dir1";
    const char test_dir2[] = "./test_dir2";
    const char up_dir[] = "../";
    const char cur_dir[] = ".";
    struct stat info;
    FwSignedSizeType file_size = 42;
    U32 file_count = 0;
    const U8 test_string[] = "This is a test file.";
    FwSignedSizeType test_string_len = 0;


    printf("Creating %s directory\n", test_dir1);
    if ((file_sys_status = Os::FileSystem::createDirectory(test_dir1)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to create directory: %s\n", test_dir1);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(stat(test_dir1, &info),0);

    // Check directory file count
    printf("Checking directory file count of %s is 0.\n", test_dir1);
    if ((file_sys_status = Os::FileSystem::getFileCount(test_dir1, file_count)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to get file count of %s\n", test_dir1);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(file_count,0);

    printf("Moving directory (%s) to (%s).\n", test_dir1, test_dir2);
    if ((file_sys_status = Os::FileSystem::moveFile(test_dir1, test_dir2)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to move directory (%s) to (%s).\n", test_dir1, test_dir2);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(stat(test_dir1, &info),-1);
    ASSERT_EQ(stat(test_dir2, &info),0);

    printf("Trying to copy directory (%s) to (%s).\n", test_dir2, test_dir1);
    if ((file_sys_status = Os::FileSystem::copyFile(test_dir2, test_dir1)) == Os::FileSystem::OP_OK) {
        printf("\tShould not have been able to copy a directory %s to %s.\n", test_dir2, test_dir1);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(stat(test_dir1, &info),-1);
    ASSERT_EQ(stat(test_dir2, &info),0);

    printf("Removing directory %s\n", test_dir2);
    if ((file_sys_status = Os::FileSystem::removeDirectory(test_dir2)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to remove directory: %s\n", test_dir2);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(stat(test_dir2, &info),-1);

    //Create a file
    printf("Creating test file (%s)\n", test_file_name1);
    if ((file_status = test_file.open(test_file_name1, Os::File::OPEN_WRITE)) != Os::File::OP_OK) {
        printf("\tFailed to create file: %s\n", test_file_name1);
        printf("\tReturn status: %d\n", file_status);
        ASSERT_TRUE(0);
    }

    test_string_len = sizeof(test_string);

    printf("Writing to test file (%s) for testing.\n", test_file_name1);
    if((file_status = test_file.write(test_string, test_string_len, Os::File::WaitType::WAIT)) != Os::File::OP_OK) {
        printf("\tFailed to write to file: %s\n.", test_file_name1);
        printf("\tReturn status: %d\n", file_status);
        ASSERT_TRUE(0);
    }

    //Close test file
    test_file.close();

    file_size = 42;
    //Get file size
    printf("Checking file size of %s.\n", test_file_name1);
    if ((file_sys_status = Os::FileSystem::getFileSize(test_file_name1, file_size)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to get file size of %s\n", test_file_name1);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(file_size,sizeof(test_string));

    printf("Copying file (%s) to (%s).\n", test_file_name1, test_file_name2);
    if ((file_sys_status = Os::FileSystem::copyFile(test_file_name1, test_file_name2)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to copy file (%s) to (%s)\n", test_file_name1, test_file_name2);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(stat(test_file_name1, &info),0);
    ASSERT_EQ(stat(test_file_name2, &info),0);

    unsigned char file_buf1[64];
    unsigned char file_buf2[64];
    // Read the two files and make sure they are the same
    test_string_len = sizeof(test_string);
    test_file.open(test_file_name1, Os::File::OPEN_READ);
    test_file.read(file_buf1, test_string_len, Os::File::WaitType::NO_WAIT);
    test_file.close();

    test_string_len = sizeof(test_string);
    test_file.open(test_file_name2, Os::File::OPEN_READ);
    test_file.read(file_buf2, test_string_len,  Os::File::WaitType::NO_WAIT);
    test_file.close();

    ASSERT_EQ(::strcmp(reinterpret_cast<const char*>(file_buf1), reinterpret_cast<const char*>(file_buf2)),0);

    printf("Removing test file 1 (%s)\n", test_file_name1);
    if ((file_sys_status = Os::FileSystem::removeFile(test_file_name1)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to remove file (%s)\n", test_file_name1);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(stat(test_file_name1, &info),-1);

    printf("Removing test file 2 (%s)\n", test_file_name2);
    if ((file_sys_status = Os::FileSystem::removeFile(test_file_name2)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to remove file (%s)\n", test_file_name2);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(stat(test_file_name2, &info),-1);

    printf("Getting the number of files in (%s)\n", cur_dir);
    if ((file_sys_status = Os::FileSystem::getFileCount(cur_dir, file_count)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to get number of files in (%s)\n", cur_dir);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_TRUE(file_count > 0);

    printf("Reading the files in (%s)\n", cur_dir);
    const int num_str = 5;
        U32 num_2 = num_str;
    Fw::String str_array[num_str];
    if ((file_sys_status = Os::FileSystem::readDirectory(cur_dir, num_str, str_array, num_2)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to read files in (%s)\n", cur_dir);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    else {
        for (int i = 0; i < num_str; ++i) {
            printf("%s\n",str_array[i].toChar());
        }
    }

    printf("Changing working directory to (%s)\n", up_dir);
    if ((file_sys_status = Os::FileSystem::changeWorkingDirectory(up_dir)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to change working directory to: %s\n", up_dir);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }

    printf("Checking current working directory is (%s)\n", up_dir);
        char dir_buff[256];
        getcwd(dir_buff, 256);
    printf("Current dir: %s\n", dir_buff);

    //Create a file in OPEN_CREATE mode
    printf("Creating test file (%s)\n", test_file_name1);
    if ((file_status = test_file.open(test_file_name1, Os::File::OPEN_CREATE)) != Os::File::OP_OK) {
        printf("\tFailed to OPEN_CREATE file: %s\n", test_file_name1);
        printf("\tReturn status: %d\n", file_status);
        ASSERT_TRUE(0);
    }

    //Close test file
    test_file.close();

    //Should not be able to OPEN_CREATE file since it already exist and we have
    //include_excl enabled
    printf("Creating test file (%s)\n", test_file_name1);
    if ((file_status = test_file.open(test_file_name1, Os::File::OPEN_CREATE)) != Os::File::FILE_EXISTS) {
        printf("\tFailed to not to overwrite existing file: %s\n", test_file_name1);
        printf("\tReturn status: %d\n", file_status);
        ASSERT_TRUE(0);
    }

    printf("Removing test file 1 (%s)\n", test_file_name1);
    if ((file_sys_status = Os::FileSystem::removeFile(test_file_name1)) != Os::FileSystem::OP_OK) {
        printf("\tFailed to remove file (%s)\n", test_file_name1);
        printf("\tReturn status: %d\n", file_sys_status);
        ASSERT_TRUE(0);
    }
    ASSERT_EQ(stat(test_file_name1, &info),-1);

}

extern "C" {
    void fileSystemTest();
}

void fileSystemTest() {
    testTestFileSystem();
}
