/**
 * \file: FpConfig.hpp
 * \author T. Canham, mstarch
 * \brief C++-compatible configuration header for fprime configuration
 *
 * \copyright
 * Copyright 2009-2015, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 */
#include <Fw/Types/BasicTypes.hpp>
#include <FppConstantsAc.hpp>
extern "C" {
#include <FpConfig.h>

// Define max length of assert string
// Note: This constant truncates file names in assertion failure event reports
#ifndef FW_ASSERT_TEXT_SIZE
#define FW_ASSERT_TEXT_SIZE FwAssertTextSize  //!< Size of string used to store assert description
#endif

}
