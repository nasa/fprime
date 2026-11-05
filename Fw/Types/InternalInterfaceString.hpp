// ======================================================================
// @file   InternalInterfaceString.hpp
// @author F Prime
// @brief  A string sized for an internal port argument
// ======================================================================

#ifndef FW_INTERNAL_INTERFACE_STRING_HPP
#define FW_INTERNAL_INTERFACE_STRING_HPP

#include "Fw/Types/StringTemplate.hpp"
#include "config/FppConstantsAc.hpp"

namespace Fw {

using InternalInterfaceString = StringTemplate<FW_INTERNAL_INTERFACE_STRING_MAX_SIZE>;

}  // namespace Fw

#endif
