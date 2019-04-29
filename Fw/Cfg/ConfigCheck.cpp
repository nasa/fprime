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

#include <Fw/Cfg/Config.hpp>
#include <Fw/Types/BasicTypes.hpp>

// Check that command/telemetry strings are not larger than an argument buffer

FW_CONFIG_ERROR(FW_CMD_STRING_MAX_SIZE <= FW_CMD_ARG_BUFFER_MAX_SIZE,CMD_STRING_TOO_BIG);
FW_CONFIG_ERROR(FW_LOG_STRING_MAX_SIZE <= FW_LOG_BUFFER_MAX_SIZE,LOG_STRING_TOO_BIG);
FW_CONFIG_ERROR(FW_TLM_STRING_MAX_SIZE <= FW_TLM_BUFFER_MAX_SIZE,TLM_STRING_TOO_BIG);
FW_CONFIG_ERROR(FW_PARAM_STRING_MAX_SIZE <= FW_PARAM_BUFFER_MAX_SIZE,PRM_STRING_TOO_BIG);

// Text logging needs the code generator for serializables to generate a stringified version of the
// value.
FW_CONFIG_ERROR((FW_ENABLE_TEXT_LOGGING == 1) && (FW_SERIALIZABLE_TO_STRING == 1),FW_SERIALIZABLE_TO_STRING_not_enabled_for_FW_ENABLE_TEXT_LOGGING);

