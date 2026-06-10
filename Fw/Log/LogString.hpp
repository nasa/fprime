// ======================================================================
// @file   LogString.hpp
// @author F Prime
// @brief  A string sized for an event log entry
// ======================================================================

#ifndef FW_LOG_STRING_HPP
#define FW_LOG_STRING_HPP

#include "Fw/Types/StringTemplate.hpp"

namespace Fw {

using LogStringArg = StringTemplate<FW_LOG_STRING_MAX_SIZE>;

}  // namespace Fw

#endif
