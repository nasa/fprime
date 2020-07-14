// ====================================================================== 
// \title  CRCChecker.hpp
// \author ortega
// \brief  hpp file for a crc32 checker
//
// \copyright
// Copyright 2009-2018, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged. Any commercial use must be negotiated with the Office
// of Technology Transfer at the California Institute of Technology.
// 
// This software may be subject to U.S. export control laws and
// regulations.  By accepting this document, the user agrees to comply
// with all U.S. export laws and regulations.  User has the
// responsibility to obtain export licenses, or other export authority
// as may be required before exporting such information to foreign
// countries or providing access to foreign persons.
// ====================================================================== 

#ifndef CRC_CHECKER_HPP
#define CRC_CHECKER_HPP

#include <Fw/Types/BasicTypes.hpp>

namespace Utils {

  static const NATIVE_INT_TYPE CRC_FILE_READ_BLOCK = 2048;
  static const U32 CRC_MAX_FILENAME_SIZE = 128; // TODO use a config variable

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
