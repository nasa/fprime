// ======================================================================
// @file   PrmBuffer.hpp
// @author F Prime
// @brief  A buffer sized for parameters
// ======================================================================

#ifndef FW_PRM_BUFFER_HPP
#define FW_PRM_BUFFER_HPP

#include "Fw/Types/LinearBufferTemplate.hpp"
#include "Fw/Types/StringBase.hpp"

namespace Fw {

static_assert(FW_PARAM_BUFFER_MAX_SIZE >= StringBase::BUFFER_SIZE(FW_PARAM_STRING_MAX_SIZE),
              "param string must fit into param buffer");

using ParamBuffer = LinearBufferTemplate<FW_PARAM_BUFFER_MAX_SIZE>;

}  // namespace Fw

#endif
