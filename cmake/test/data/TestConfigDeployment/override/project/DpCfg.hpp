// ======================================================================
// \title  DpCfg.hpp
// \author mstarch
// \brief  hpp file for override testing
//
// \copyright
// Copyright 2025, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef DPCFG_HPP
#define DPCFG_HPP

// THE ONLY DIFFERENCE: set a new value for override testing
const bool OVERRIDE_TEST = true;
// END OF THE ONLY DIFFERENCE

#include <Fw/FPrimeBasicTypes.hpp>

// The format string for a file name
// The format arguments are base directory, container ID, time seconds, and time microseconds
#define DP_EXT ".fdp"
constexpr const char *DP_FILENAME_FORMAT = "%s/Dp_%08" PRI_FwDpIdType "_%08" PRIu32 "_%08" PRIu32 DP_EXT;

#endif
