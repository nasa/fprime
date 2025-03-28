/**
 * \file ConfigCheck.cpp
 * \author F Prime
 * \brief Static assertions for F Prime configuration macros
 *
 * \copyright
 * Copyright 2009-2024, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#include <Fw/FPrimeBasicTypes.hpp>
#include <limits>

// Check that Fw::String is big enough to hold any string that can be given a
// default value in FPP.

static_assert(FW_FIXED_LENGTH_STRING_SIZE >= FW_CMD_STRING_MAX_SIZE,
              "A generic string should be able to hold a command string");
static_assert(FW_FIXED_LENGTH_STRING_SIZE >= FW_LOG_STRING_MAX_SIZE,
              "A generic string should be able to hold an event string");
static_assert(FW_FIXED_LENGTH_STRING_SIZE >= FW_TLM_STRING_MAX_SIZE,
              "A generic string should be able to hold a telemetry string");
static_assert(FW_FIXED_LENGTH_STRING_SIZE >= FW_PARAM_STRING_MAX_SIZE,
              "A generic string should be able to hold a parameter string");

// Check that command/telemetry strings are not larger than an argument buffer

static_assert(FW_CMD_STRING_MAX_SIZE <= FW_CMD_ARG_BUFFER_MAX_SIZE,
              "FW_CMD_STRING_MAX_SIZE cannot be larger than FW_CMD_ARG_BUFFER_MAX_SIZE");
static_assert(FW_LOG_STRING_MAX_SIZE <= FW_LOG_BUFFER_MAX_SIZE,
              "FW_LOG_STRING_MAX_SIZE cannot be larger than FW_LOG_BUFFER_MAX_SIZE");
static_assert(FW_TLM_STRING_MAX_SIZE <= FW_TLM_BUFFER_MAX_SIZE,
              "FW_TLM_STRING_MAX_SIZE cannot be larger than FW_TLM_BUFFER_MAX_SIZE");
static_assert(FW_PARAM_STRING_MAX_SIZE <= FW_PARAM_BUFFER_MAX_SIZE,
              "FW_PARAM_STRING_MAX_SIZE cannot be larger than FW_PARAM_BUFFER_MAX_SIZE");

// Text logging needs the code generator for serializables to generate a stringified version of the
// value.
static_assert((FW_ENABLE_TEXT_LOGGING == 0) || (FW_SERIALIZABLE_TO_STRING != 0),
              "FW_SERIALIZABLE_TO_STRING must be enabled to enable FW_ENABLE_TEXT_LOGGING");

// Unit testing needs serializable strings
#ifdef BUILD_UT
static_assert(FW_SERIALIZABLE_TO_STRING != 0,
              "FW_SERIALIZABLE_TO_STRING must be enabled for unit testing");
#endif

static_assert(std::numeric_limits<FwBuffSizeType>::max() == std::numeric_limits<FwSizeStoreType>::max() &&
                  std::numeric_limits<FwBuffSizeType>::min() == std::numeric_limits<FwSizeStoreType>::min(),
              "FwBuffSizeType must be equivalent to FwExternalSizeType");

static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<FwSizeStoreType>::max() &&
                  std::numeric_limits<FwSizeType>::min() <= std::numeric_limits<FwSizeStoreType>::min(),
              "FwSizeType cannot entirely store values of type FwExternalSizeType");


// Check platform defined types for required invariants
static_assert(sizeof(PlatformPointerCastType) == sizeof(void*), "PlatformPointerCastType must be the same size as pointers");
static_assert(std::numeric_limits<PlatformSizeType>::max() >= std::numeric_limits<unsigned int>::max(),
                "PlatformSizeType must be at least as large as unsigned int");
static_assert(std::numeric_limits<PlatformSignedSizeType>::max() >= std::numeric_limits<int>::max(),
                "PlatformSignedSizeType must be at least as large as int"); 
static_assert(std::numeric_limits<PlatformSignedSizeType>::min() <= std::numeric_limits<int>::min(),
                "PlatformSignedSizeType must be at least as small as int");
static_assert(std::numeric_limits<PlatformIndexType>::is_signed, "PlatformIndexType must be signed");
static_assert(not std::numeric_limits<PlatformSizeType>::is_signed, "PlatformSizeType must be unsigned");
static_assert(std::numeric_limits<PlatformSignedSizeType>::is_signed, "PlatformSignedSizeType must be signed");
static_assert(std::numeric_limits<PlatformSizeType>::max() >= std::numeric_limits<U32>::max(), "PlatformSizeType must be at least as large as U32");
static_assert(sizeof(PlatformSizeType) == sizeof(PlatformSignedSizeType), "PlatformSizeType must be the same size as PlatformSignedSizeType");

// Check framework configured types for required invariants
static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<unsigned int>::max(),
                "FwSizeType must be at least as large as unsigned int");
static_assert(std::numeric_limits<FwSignedSizeType>::max() >= std::numeric_limits<int>::max(),
                "FwSignedSizeType must be at least as large as int"); 
static_assert(std::numeric_limits<FwSignedSizeType>::min() <= std::numeric_limits<int>::min(),
                "FwSignedSizeType must be at least as large as int");
static_assert(std::numeric_limits<FwIndexType>::is_signed, "FwIndexType must be signed");
static_assert(not std::numeric_limits<FwSizeType>::is_signed, "FwSizeType must be unsigned");
static_assert(std::numeric_limits<FwSignedSizeType>::is_signed, "FwSignedSizeType must be signed");
static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<U32>::max(), "FwSizeType must be at least as large as U32");
static_assert(sizeof(FwSizeType) == sizeof(FwSignedSizeType), "FwSizeType must be the same size as FwSignedSizeType");

