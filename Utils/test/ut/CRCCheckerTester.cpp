// ======================================================================
// \title  CRCCheckerTester.cpp
// \brief  cpp file for CRCChecker unit tests
//
// \copyright
// Copyright 2009-2026, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <gtest/gtest.h>
#include <Fw/Types/FileNameString.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Os/ValidateFile.hpp>
#include <STest/STest/Pick/Pick.hpp>
#include <Utils/CRCChecker.hpp>
#include <Utils/Hash/Hash.hpp>
#include <string>
#include <vector>

namespace {

const char* const TEST_FILE = "crc_checker_test_file.bin";

Fw::FileNameString hashFileName(const char* const fname) {
    Fw::FileNameString name;
    EXPECT_EQ(name.format("%s%s", fname, HASH_EXTENSION_STRING), Fw::FormatStatus::SUCCESS);
    return name;
}

void removeTestFiles(const char* const fname) {
    (void)Os::FileSystem::removeFile(fname);
    (void)Os::FileSystem::removeFile(hashFileName(fname).toChar());
}

void writeFile(const char* const fname, const U8* data, FwSizeType size) {
    Os::File f;
    ASSERT_EQ(f.open(fname, Os::File::OPEN_CREATE, Os::File::OVERWRITE), Os::File::OP_OK);
    if (size > 0) {
        FwSizeType writeSize = size;
        ASSERT_EQ(f.write(data, writeSize), Os::File::OP_OK);
        ASSERT_EQ(writeSize, size);
    }
    f.close();
}

std::vector<U8> randomData(FwSizeType size) {
    std::vector<U8> data(size);
    for (FwSizeType i = 0; i < size; i++) {
        data[i] = static_cast<U8>(STest::Pick::lowerUpper(0, 255));
    }
    return data;
}

U32 expectedCrc(const U8* data, FwSizeType size) {
    Utils::Hash hash;
    U32 crc = 0;
    if (size > 0) {
        hash.update(data, size);
    }
    hash.finalize(crc);
    return crc;
}

class CRCCheckerTest : public ::testing::Test {
  protected:
    void SetUp() override { removeTestFiles(TEST_FILE); }
    void TearDown() override { removeTestFiles(TEST_FILE); }
};

TEST_F(CRCCheckerTest, NominalRoundTripSizes) {
    const FwSizeType block = static_cast<FwSizeType>(Utils::CRC_FILE_READ_BLOCK);
    const FwSizeType sizes[] = {0, 1, block - 1, block, block + 1, (3 * block) + 7};
    for (FwSizeType size : sizes) {
        std::vector<U8> data = randomData(size);
        writeFile(TEST_FILE, data.data(), size);

        ASSERT_EQ(Utils::create_checksum_file(TEST_FILE), Utils::PASSED_FILE_CRC_WRITE) << "size " << size;

        U32 expected = 0;
        U32 actual = 0;
        ASSERT_EQ(Utils::verify_checksum(TEST_FILE, expected, actual), Utils::PASSED_FILE_CRC_CHECK) << "size " << size;
        ASSERT_EQ(expected, actual);
        ASSERT_EQ(actual, expectedCrc(data.data(), size));
        removeTestFiles(TEST_FILE);
    }
}

TEST_F(CRCCheckerTest, GoldenValue) {
    // Standard CRC-32 (IEEE 802.3) check value for the ASCII string "123456789"
    const U8 data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    writeFile(TEST_FILE, data, sizeof(data));
    ASSERT_EQ(Utils::create_checksum_file(TEST_FILE), Utils::PASSED_FILE_CRC_WRITE);

    U32 fromFile = 0;
    ASSERT_EQ(Utils::read_crc32_from_file(TEST_FILE, fromFile), Utils::PASSED_FILE_CRC_CHECK);
    ASSERT_EQ(fromFile, 0xCBF43926u);

    // The static hash interface produces the same digest
    Utils::HashBuffer buffer;
    Utils::Hash::hash(data, sizeof(data), buffer);
    ASSERT_EQ(buffer.asBigEndianU32(), 0xCBF43926u);

    Utils::HashBuffer other;
    Utils::Hash::hash(data, sizeof(data), other);
    ASSERT_TRUE(buffer == other);

    // File extension helpers
    ASSERT_STREQ(Utils::Hash::getFileExtensionString(), HASH_EXTENSION_STRING);
    ASSERT_EQ(Utils::Hash::getFileExtensionLength(), static_cast<FwSizeType>(sizeof(HASH_EXTENSION_STRING) - 1));
    Fw::FileNameString extended;
    Utils::Hash::addFileExtension(Fw::FileNameString(TEST_FILE), extended);
    ASSERT_STREQ(extended.toChar(), hashFileName(TEST_FILE).toChar());
}

TEST_F(CRCCheckerTest, ChecksumFileMatchesFrameworkFormat) {
    // A .CRC32 file is not private to CRCChecker: Utils::Hash and Os::ValidateFile write and read
    // the same sidecar for the same data files. Its contents must therefore be the serialized
    // (big-endian) form of the CRC rather than a raw copy of the host's U32 bytes.
    const U8 data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    writeFile(TEST_FILE, data, sizeof(data));
    ASSERT_EQ(Utils::create_checksum_file(TEST_FILE), Utils::PASSED_FILE_CRC_WRITE);

    // Bytes on disk match what Utils::Hash produces for the same data
    Utils::HashBuffer expectedBuffer;
    Utils::Hash::hash(data, sizeof(data), expectedBuffer);
    U8 onDisk[sizeof(U32)] = {};
    Os::File crcFile;
    ASSERT_EQ(crcFile.open(hashFileName(TEST_FILE).toChar(), Os::File::OPEN_READ), Os::File::OP_OK);
    FwSizeType readSize = sizeof(onDisk);
    ASSERT_EQ(crcFile.read(onDisk, readSize), Os::File::OP_OK);
    ASSERT_EQ(readSize, static_cast<FwSizeType>(sizeof(onDisk)));
    crcFile.close();
    ASSERT_EQ(memcmp(onDisk, expectedBuffer.getBuffAddr(), sizeof(onDisk)), 0);

    // ...and the framework's standard validation entry point accepts the file
    ASSERT_EQ(Os::ValidateFile::validate(TEST_FILE, hashFileName(TEST_FILE).toChar()), Os::ValidateFile::VALIDATION_OK);

    // The reverse direction holds too: a checksum file written by Os::ValidateFile reads back
    // through CRCChecker with the correct value
    (void)Os::FileSystem::removeFile(hashFileName(TEST_FILE).toChar());
    ASSERT_EQ(Os::ValidateFile::createValidation(TEST_FILE, hashFileName(TEST_FILE).toChar()),
              Os::ValidateFile::VALIDATION_OK);
    U32 fromFile = 0;
    ASSERT_EQ(Utils::read_crc32_from_file(TEST_FILE, fromFile), Utils::PASSED_FILE_CRC_CHECK);
    ASSERT_EQ(fromFile, 0xCBF43926u);
    U32 expected = 0;
    U32 actual = 0;
    ASSERT_EQ(Utils::verify_checksum(TEST_FILE, expected, actual), Utils::PASSED_FILE_CRC_CHECK);
}

TEST_F(CRCCheckerTest, MissingDataFile) {
    ASSERT_EQ(Utils::create_checksum_file(TEST_FILE), Utils::FAILED_FILE_SIZE);
    U32 expected = 0;
    U32 actual = 0;
    ASSERT_EQ(Utils::verify_checksum(TEST_FILE, expected, actual), Utils::FAILED_FILE_SIZE);
}

TEST_F(CRCCheckerTest, MissingCrcFile) {
    std::vector<U8> data = randomData(64);
    writeFile(TEST_FILE, data.data(), data.size());

    U32 fromFile = 0;
    ASSERT_EQ(Utils::read_crc32_from_file(TEST_FILE, fromFile), Utils::FAILED_FILE_CRC_OPEN);
    U32 expected = 0;
    U32 actual = 0;
    ASSERT_EQ(Utils::verify_checksum(TEST_FILE, expected, actual), Utils::FAILED_FILE_CRC_OPEN);
}

TEST_F(CRCCheckerTest, TruncatedCrcFile) {
    std::vector<U8> data = randomData(64);
    writeFile(TEST_FILE, data.data(), data.size());
    // Write a checksum file shorter than a U32
    const U8 partial[] = {0xAB, 0xCD};
    writeFile(hashFileName(TEST_FILE).toChar(), partial, sizeof(partial));

    U32 fromFile = 0;
    ASSERT_EQ(Utils::read_crc32_from_file(TEST_FILE, fromFile), Utils::FAILED_FILE_CRC_READ);
    U32 expected = 0;
    U32 actual = 0;
    ASSERT_EQ(Utils::verify_checksum(TEST_FILE, expected, actual), Utils::FAILED_FILE_CRC_READ);
}

TEST_F(CRCCheckerTest, CorruptedDataFails) {
    std::vector<U8> data = randomData(256);
    writeFile(TEST_FILE, data.data(), data.size());
    ASSERT_EQ(Utils::create_checksum_file(TEST_FILE), Utils::PASSED_FILE_CRC_WRITE);

    // Corrupt one byte of the data file
    FwSizeType index = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, static_cast<U32>(data.size() - 1)));
    data[index] = static_cast<U8>(data[index] ^ 0xFF);
    writeFile(TEST_FILE, data.data(), data.size());

    U32 expected = 0;
    U32 actual = 0;
    ASSERT_EQ(Utils::verify_checksum(TEST_FILE, expected, actual), Utils::FAILED_FILE_CRC_CHECK);
    ASSERT_NE(expected, actual);
}

TEST_F(CRCCheckerTest, RandomizedSoak) {
    const FwSizeType block = static_cast<FwSizeType>(Utils::CRC_FILE_READ_BLOCK);
    const U32 iterations = 25;
    for (U32 i = 0; i < iterations; i++) {
        const FwSizeType size = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, static_cast<U32>(4 * block)));
        std::vector<U8> data = randomData(size);
        writeFile(TEST_FILE, data.data(), size);

        ASSERT_EQ(Utils::create_checksum_file(TEST_FILE), Utils::PASSED_FILE_CRC_WRITE);
        U32 expected = 0;
        U32 actual = 0;
        ASSERT_EQ(Utils::verify_checksum(TEST_FILE, expected, actual), Utils::PASSED_FILE_CRC_CHECK);

        if (size > 0) {
            // Mutate a random byte: verification must fail
            FwSizeType index = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, static_cast<U32>(size - 1)));
            data[index] = static_cast<U8>(data[index] ^ (1u << STest::Pick::lowerUpper(0, 7)));
            writeFile(TEST_FILE, data.data(), size);
            ASSERT_EQ(Utils::verify_checksum(TEST_FILE, expected, actual), Utils::FAILED_FILE_CRC_CHECK);
            ASSERT_NE(expected, actual);
        }
        removeTestFiles(TEST_FILE);
    }
}

}  // namespace

TEST_F(CRCCheckerTest, FileNameTooLong) {
    // A filename that leaves no room for the hash extension in Fw::FileNameString must be reported
    // as FAILED_FILE_NAME_TOO_LONG by every helper, never asserted on.
    const FwSizeType ext = Utils::Hash::getFileExtensionLength();
    const FwSizeType longestOk = FileNameStringSize - ext;  // Fw::FileNameString holds FileNameStringSize chars
    const U8 data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

    for (FwSizeType len : {longestOk + 1, static_cast<FwSizeType>(FileNameStringSize)}) {
        std::string name(static_cast<size_t>(len), 'x');
        writeFile(name.c_str(), data, sizeof(data));

        ASSERT_EQ(Utils::create_checksum_file(name.c_str()), Utils::FAILED_FILE_NAME_TOO_LONG) << "len " << len;
        U32 fromFile = 0;
        ASSERT_EQ(Utils::read_crc32_from_file(name.c_str(), fromFile), Utils::FAILED_FILE_NAME_TOO_LONG)
            << "len " << len;
        U32 expected = 0;
        U32 actual = 0;
        ASSERT_EQ(Utils::verify_checksum(name.c_str(), expected, actual), Utils::FAILED_FILE_NAME_TOO_LONG)
            << "len " << len;

        (void)Os::FileSystem::removeFile(name.c_str());
    }

    // The longest name that fits round-trips normally
    std::string okName(static_cast<size_t>(longestOk), 'y');
    writeFile(okName.c_str(), data, sizeof(data));
    ASSERT_EQ(Utils::create_checksum_file(okName.c_str()), Utils::PASSED_FILE_CRC_WRITE);
    U32 expected = 0;
    U32 actual = 0;
    ASSERT_EQ(Utils::verify_checksum(okName.c_str(), expected, actual), Utils::PASSED_FILE_CRC_CHECK);
    ASSERT_EQ(expected, 0xCBF43926u);
    removeTestFiles(okName.c_str());
}
