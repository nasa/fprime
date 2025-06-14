// ====================================================================== 
// \title  CRCChecker.hpp
// \author ortega
// \brief  hpp file for a crc32 checker
//
// \copyright
// Copyright 2009-2020, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
// ====================================================================== 

#ifndef CRC_CHECKER_HPP
#define CRC_CHECKER_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <config/CRCCheckerConfig.hpp>

namespace Utils {

  static const FwSignedSizeType CRC_FILE_READ_BLOCK = CONFIG_CRC_FILE_READ_BLOCK ;
 

  typedef enum
  {
    PASSED_FILE_CRC_CHECK = 0,
    PASSED_FILE_CRC_WRITE,
    FAILED_FILE_SIZE,
    FAILED_FILE_SIZE_CAST,
    FAILED_FILE_OPEN,
    FAILED_FILE_READ,
    FAILED_FILE_CRC_OPEN,
    FAILED_FILE_CRC_READ,
    FAILED_FILE_CRC_WRITE,
    FAILED_FILE_CRC_CHECK
  } crc_stat_t;

  crc_stat_t create_checksum_file(const char* const filename);
  crc_stat_t read_crc32_from_file(const char* const fname, U32 &checksum_from_file);
  crc_stat_t verify_checksum(const char* const filename, U32 &expected, U32 &actual);

}

#endif
