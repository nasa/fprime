// ======================================================================
// @file   TlmString.hpp
// @author F Prime
// @brief  A string sized for a telemetry channel
// ======================================================================

#ifndef FW_TLM_STRING_HPP
#define FW_TLM_STRING_HPP

#include "Fw/Types/StringTemplate.hpp"

namespace Fw {

using TlmString = StringTemplate<FW_TLM_STRING_MAX_SIZE>;

}  // namespace Fw

#endif
