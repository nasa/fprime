// ======================================================================
// @file   CmdArgBuffer.hpp
// @author F Prime
// @brief  A buffer sized for command arguments
// ======================================================================

#ifndef FW_CMD_ARG_BUFFER_HPP
#define FW_CMD_ARG_BUFFER_HPP

#include "Fw/Types/LinearBufferTemplate.hpp"

namespace Fw {

using CmdArgBuffer = LinearBufferTemplate<FW_CMD_ARG_BUFFER_MAX_SIZE>;

}  // namespace Fw

#endif
