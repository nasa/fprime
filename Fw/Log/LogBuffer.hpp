// ======================================================================
// @file   LogBuffer.hpp
// @author F Prime
// @brief  A buffer sized for log entries
// ======================================================================

#ifndef FW_LOG_BUFFER_HPP
#define FW_LOG_BUFFER_HPP

#include "Fw/Types/LinearBufferTemplate.hpp"

namespace Fw {

using LogBuffer = LinearBufferTemplate<FW_LOG_BUFFER_MAX_SIZE>;

}  // namespace Fw

#endif
