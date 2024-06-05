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

#include <FpConfig.hpp>
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
static_assert((FW_ENABLE_TEXT_LOGGING == 0) || (FW_SERIALIZABLE_TO_STRING == 1),
              "FW_SERIALIZABLE_TO_STRING must be enabled to enable FW_ENABLE_TEXT_LOGGING");

static_assert(std::numeric_limits<FwBuffSizeType>::max() == std::numeric_limits<FwSizeStoreType>::max() &&
                  std::numeric_limits<FwBuffSizeType>::min() == std::numeric_limits<FwSizeStoreType>::min(),
              "FwBuffSizeType must be equivalent to FwExternalSizeType");

static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<FwSizeStoreType>::max() &&
                  std::numeric_limits<FwSizeType>::min() <= std::numeric_limits<FwSizeStoreType>::min(),
              "FwSizeType cannot entirely store values of type FwExternalSizeType");
