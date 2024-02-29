// ======================================================================
// \title  CRCChecker.cpp
// \author ortega
// \brief  cpp file for a crc32 checker
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ======================================================================

#include <FpConfig.hpp>
#include <cstdio> // For snprintf
#include <Utils/CRCChecker.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/File.hpp>
#include <Os/FileSystem.hpp>
#include <Utils/Hash/Hash.hpp>

namespace Utils {

  crc_stat_t create_checksum_file(const char* const fname)
  {
    FW_ASSERT(fname != nullptr);

    FwSignedSizeType i;
    FwSignedSizeType blocks;
    FwSignedSizeType remaining_bytes;
    FwSignedSizeType filesize;
    Os::File f;
    Os::FileSystem::Status fs_stat;
    Os::File::Status stat;
    Utils::Hash hash;
    U32 checksum;
    I32 s_stat;
    FwSignedSizeType int_file_size;
    FwSignedSizeType bytes_to_read;
    FwSignedSizeType bytes_to_write;
    CHAR hashFilename[CRC_MAX_FILENAME_SIZE];
    U8 block_data[CRC_FILE_READ_BLOCK];

    fs_stat = Os::FileSystem::getFileSize(fname, filesize);
    if(fs_stat != Os::FileSystem::OP_OK)
    {
      return FAILED_FILE_SIZE;
    }

    int_file_size = filesize;

    // Open file
    stat = f.open(fname, Os::File::OPEN_READ);
    if(stat != Os::File::OP_OK)
    {
      return FAILED_FILE_OPEN;
    }

    // Read file
    bytes_to_read = CRC_FILE_READ_BLOCK;
    blocks = int_file_size / CRC_FILE_READ_BLOCK;
    for(i = 0; i < blocks; i++)
    {
      stat = f.read(block_data, bytes_to_read);
      if(stat != Os::File::OP_OK || bytes_to_read != CRC_FILE_READ_BLOCK)
      {
        f.close();
        return  FAILED_FILE_READ;
      }

      hash.update(block_data, bytes_to_read);
    }

    remaining_bytes = int_file_size % CRC_FILE_READ_BLOCK;
    bytes_to_read = remaining_bytes;
    if(remaining_bytes > 0)
    {
      stat = f.read(block_data, bytes_to_read);
      if(stat != Os::File::OP_OK || bytes_to_read != remaining_bytes)
      {
        f.close();
        return FAILED_FILE_READ;
      }

      hash.update(block_data, remaining_bytes);
    }

    // close file
    f.close();

    // generate checksum
    hash.final(checksum);

    // open checksum file
    s_stat = snprintf(hashFilename,  CRC_MAX_FILENAME_SIZE, "%s%s", fname, HASH_EXTENSION_STRING);
    FW_ASSERT(s_stat > 0);

    stat = f.open(hashFilename, Os::File::OPEN_WRITE);
    if(stat != Os::File::OP_OK)
    {
      return FAILED_FILE_CRC_OPEN;
    }

    // Write  checksum  file
    bytes_to_write = sizeof(checksum);
    stat = f.write(reinterpret_cast<U8*>(&checksum), bytes_to_write);
    if(stat != Os::File::OP_OK || sizeof(checksum) != bytes_to_write)
    {
      f.close();
      return FAILED_FILE_CRC_WRITE;
    }

    // close checksum file
    f.close();

    return PASSED_FILE_CRC_WRITE;
  }

  crc_stat_t read_crc32_from_file(const char* const fname, U32 &checksum_from_file) {
      Os::File f;
      Os::File::Status stat;
      char hashFilename[CRC_MAX_FILENAME_SIZE];
      FW_ASSERT(fname != nullptr);
      // open checksum file
      I32 s_stat = snprintf(hashFilename,  CRC_MAX_FILENAME_SIZE, "%s%s", fname, HASH_EXTENSION_STRING);
      FW_ASSERT(s_stat > 0);

      stat = f.open(hashFilename, Os::File::OPEN_READ);
      if(stat != Os::File::OP_OK)
      {
        return FAILED_FILE_CRC_OPEN;
      }

      // Read  checksum  file
      FwSignedSizeType checksum_from_file_size = static_cast<FwSignedSizeType>(sizeof(checksum_from_file));
      stat = f.read(reinterpret_cast<U8*>(&checksum_from_file), checksum_from_file_size);
      if(stat != Os::File::OP_OK || checksum_from_file_size != sizeof(checksum_from_file))
      {
        f.close();
        return FAILED_FILE_CRC_READ;
      }

      // close checksum file
      f.close();
      return PASSED_FILE_CRC_CHECK;
  }

  crc_stat_t verify_checksum(const char* const fname, U32 &expected, U32 &actual)
  {
    FW_ASSERT(fname != nullptr);

    FwSignedSizeType i;
    FwSignedSizeType blocks;
    PlatformIntType remaining_bytes;
    FwSignedSizeType filesize;
    Os::File f;
    Os::FileSystem::Status fs_stat;
    Os::File::Status stat;
    Utils::Hash hash;
    U32 checksum;
    U32 checksum_from_file;
    FwSignedSizeType int_file_size;
    FwSignedSizeType bytes_to_read;
    U8 block_data[CRC_FILE_READ_BLOCK];

    fs_stat = Os::FileSystem::getFileSize(fname, filesize);
    if(fs_stat != Os::FileSystem::OP_OK)
    {
      return FAILED_FILE_SIZE;
    }

    int_file_size = static_cast<NATIVE_INT_TYPE>(filesize);
    if(static_cast<FwSignedSizeType>(int_file_size) != filesize)
    {
      return FAILED_FILE_SIZE_CAST;
    }

    // Open file
    stat = f.open(fname, Os::File::OPEN_READ);
    if(stat != Os::File::OP_OK)
    {
      return FAILED_FILE_OPEN;
    }

    // Read file
    bytes_to_read = CRC_FILE_READ_BLOCK;
    blocks = filesize / CRC_FILE_READ_BLOCK;
    for(i = 0; i < blocks; i++)
    {
      stat = f.read(block_data, bytes_to_read);
      if(stat != Os::File::OP_OK || bytes_to_read != CRC_FILE_READ_BLOCK)
      {
        f.close();
        return  FAILED_FILE_READ;
      }

      hash.update(block_data, bytes_to_read);
    }

    remaining_bytes = int_file_size % CRC_FILE_READ_BLOCK;
    bytes_to_read = remaining_bytes;
    if(remaining_bytes > 0)
    {
      stat = f.read(block_data, bytes_to_read);
      if(stat != Os::File::OP_OK || bytes_to_read != remaining_bytes)
      {
        f.close();
        return FAILED_FILE_READ;
      }

      hash.update(block_data, remaining_bytes);
    }

    // close file
    f.close();
    // generate checksum
    hash.final(checksum);

    crc_stat_t crcstat = read_crc32_from_file(fname, checksum_from_file);
    if (crcstat != PASSED_FILE_CRC_CHECK) {
        return crcstat;
    }

    // compare checksums
    if(checksum != checksum_from_file)
    {
      expected = checksum_from_file;
      actual = checksum;
      return FAILED_FILE_CRC_CHECK;
    }

    expected = checksum_from_file;
    actual = checksum;
    return PASSED_FILE_CRC_CHECK;
  }

}
