// ======================================================================
// @file   CmdString.hpp
// @author F Prime
// @brief  A string sized for a command argument
// ======================================================================

#ifndef FW_CMD_STRING_HPP
#define FW_CMD_STRING_HPP

#include "Fw/Types/StringTemplate.hpp"

namespace Fw {

using CmdStringArg = StringTemplate<FW_CMD_STRING_MAX_SIZE>;

}  // namespace Fw

#endif
