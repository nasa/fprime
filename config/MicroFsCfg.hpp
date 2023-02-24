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

static const FwNativeUIntType MAX_MICROFS_BINS = 10;  //!< Maximum number of bin configurations
#define MICROFS_BIN_STRING "bin"        //!< path name for bin directory
#define MICROFS_FILE_STRING "file"     //!< name for file slot prefix
static const bool MICROFS_SKIP_NULL_CHECK = false; //!< if true, skip memory null check on init. Guards against case where a reset does not clear memory.
}  // namespace Os
#endif