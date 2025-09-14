// ======================================================================
// \title  FPrimeNumericalConfig.hpp
// \author mstarch
// \brief  hpp file for FPrime integer configuration
//
// \copyright
// Copyright 2024, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// FPrime projects are allowed to turn on or off the use of 16, 32, and 64
// bit integers. This is done to allow projects to disable support for types
// that are not supported by the target platform, or perform inefficient
// computations to synthesize these types.
//
// Users should set the properties in this file to 1 or 0 to enable or disable
// the feature.
//
// This header is intended to be C-compatible.
//
// ======================================================================

#ifndef FPRIME_INTEGER_CONFIG_H
#define FPRIME_INTEGER_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif

#define FW_HAS_64_BIT 1                   //!< Architecture supports 64 bit integers
#define FW_HAS_32_BIT 1                   //!< Architecture supports 32 bit integers
#define FW_HAS_16_BIT 1                   //!< Architecture supports 16 bit integers
#define SKIP_FLOAT_IEEE_754_COMPLIANCE 0  //!<  Check IEEE 754 compliance of floating point arithmetic

#ifdef __cplusplus
}
#endif
#endif  // FPRIME_INTEGER_CONFIG_H
