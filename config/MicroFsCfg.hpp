/**
 * \file
 * \author T. Canham
 * \brief MicroFs Configuration file
 *
 * \copyright
 * Copyright 2023, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 * <br /><br />
 */

#ifndef _MICROFSCFG_HPP_
#define _MICROFSCFG_HPP_

#include <Fw/Types/BasicTypes.hpp>

#define MICROFS_INIT_FILE_DATA 1  //!< initialize file data to zero. Requires more CPU cycles.

namespace Os {

static const PlatformUIntType MAX_MICROFS_BINS = 10;  //!< Maximum number of bin configurations
static const char* MICROFS_BIN_STRING = "bin";        //!< path name for bin directory
static const char* MICROFS_FILE_STRING = "file";      //!< name for file slot prefix
static const PlatformUIntType MICROFS_MAX_STRING =
    4;  //!< limit number of characters to bound search for the bin/file number. Needs to match length of
        //!< MICROFS_BIN_STRING/MICROFS_FILE_STRING
static const PlatformUIntType MICROFS_MAX_DIGITS =
    3;  //!< limit number of digits to bound search for the bin/file number. Needs to match range of MAX_MICROFS_BINS

}  // namespace Os
#endif