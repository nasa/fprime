// ======================================================================
// @file   TlmBuffer.hpp
// @author F Prime
// @brief  A buffer sized for telemetry
// ======================================================================

#ifndef FW_TLM_BUFFER_HPP
#define FW_TLM_BUFFER_HPP

#include "Fw/Types/LinearBufferTemplate.hpp"

namespace Fw {

using TlmBuffer = LinearBufferTemplate<FW_TLM_BUFFER_MAX_SIZE>;

}  // namespace Fw

#endif
