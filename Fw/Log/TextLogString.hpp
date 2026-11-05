// ======================================================================
// @file   TextLogString.hpp
// @author F Prime
// @brief  A string sized for an event log entry
// ======================================================================

#ifndef FW_TEXT_LOG_STRING_HPP
#define FW_TEXT_LOG_STRING_HPP

#include "Fw/Types/StringTemplate.hpp"

namespace Fw {

using TextLogString = StringTemplate<FW_LOG_TEXT_BUFFER_SIZE>;

}  // namespace Fw

#endif
