// ======================================================================
// @file   PrmString.hpp
// @author F Prime
// @brief  A string sized for a parameter
// ======================================================================

#ifndef FW_PARAM_STRING_HPP
#define FW_PARAM_STRING_HPP

#include "Fw/Types/StringTemplate.hpp"

namespace Fw {

using ParamString = StringTemplate<FW_PARAM_STRING_MAX_SIZE>;

}  // namespace Fw

#endif
