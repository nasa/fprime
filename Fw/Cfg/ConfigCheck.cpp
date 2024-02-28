/**
 * \file
 * \author T. Canham
 * \brief Configuration checks for ISF configuration macros
 *
 * \copyright
 * Copyright 2009-2016, by the California Institute of Technology.
 * ALL RIGHTS RESERVED.  United States Government Sponsorship
 * acknowledged.
 *
 */

#include <FpConfig.hpp>
#include <limits>

// Check that command/telemetry strings are not larger than an argument buffer

static_assert(FW_CMD_STRING_MAX_SIZE <= FW_CMD_ARG_BUFFER_MAX_SIZE, "FW_CMD_STRING_MAX_SIZE cannot be larger than FW_CMD_ARG_BUFFER_MAX_SIZE");
static_assert(FW_LOG_STRING_MAX_SIZE <= FW_LOG_BUFFER_MAX_SIZE, "FW_LOG_STRING_MAX_SIZE cannot be larger than FW_LOG_BUFFER_MAX_SIZE");
static_assert(FW_TLM_STRING_MAX_SIZE <= FW_TLM_BUFFER_MAX_SIZE, "FW_TLM_STRING_MAX_SIZE cannot be larger than FW_TLM_BUFFER_MAX_SIZE");
static_assert(FW_PARAM_STRING_MAX_SIZE <= FW_PARAM_BUFFER_MAX_SIZE, "FW_PARAM_STRING_MAX_SIZE cannot be larger than FW_PARAM_BUFFER_MAX_SIZE");

// Text logging needs the code generator for serializables to generate a stringified version of the
// value.
static_assert((FW_ENABLE_TEXT_LOGGING == 0) || ( FW_SERIALIZABLE_TO_STRING == 1), "FW_SERIALIZABLE_TO_STRING must be enabled to enable FW_ENABLE_TEXT_LOGGING");

static_assert(std::numeric_limits<FwBuffSizeType>::max() == std::numeric_limits<FwSizeStoreType>::max() &&
              std::numeric_limits<FwBuffSizeType>::min() == std::numeric_limits<FwSizeStoreType>::min(),
              "FwBuffSizeType must be equivalent to FwExternalSizeType");

static_assert(std::numeric_limits<FwSizeType>::max() >= std::numeric_limits<FwSizeStoreType>::max() &&
              std::numeric_limits<FwSizeType>::min() <= std::numeric_limits<FwSizeStoreType>::min(),
              "FwSizeType cannot entirely store values of type FwExternalSizeType");
