// ======================================================================
// \title  Fw/FPrimeBasicTypes.hpp
// \author mstarch
// \brief  hpp file for basic types used in F Prime
//
// \copyright
// Copyright 2025, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// FPrime defines a number of basic types, platform configurable types,
// and project configurable types. This file provides a single header
// users can import for using these types. It wraps the underlying C
// header in C-linkage to allow C++ users to include this header.
//
// ======================================================================
#ifndef FPRIME_BASIC_TYPES_HPP
#define FPRIME_BASIC_TYPES_HPP
#include <limits>
extern "C" {
    #include <Fw/FPrimeBasicTypes.h>
}
#include <Fw/Types/BasicTypes.hpp>
// Needed for FwAssertTextSize
#include <config/FppConstantsAc.hpp>

// Define max length of assert string
// Note: This constant truncates file names in assertion failure event reports
#ifndef FW_ASSERT_TEXT_SIZE
#define FW_ASSERT_TEXT_SIZE FwAssertTextSize  //!< Size of string used to store assert description
#endif
#endif // FW_TYPES_HPP
