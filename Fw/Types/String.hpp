// ======================================================================
// @file   String.hpp
// @author F Prime
// @brief  A general purpose string backed by a fixed-size buffer
// ======================================================================

#ifndef FW_STRING_HPP
#define FW_STRING_HPP

#include "Fw/Types/StringTemplate.hpp"

namespace Fw {

using String = StringTemplate<FW_FIXED_LENGTH_STRING_SIZE>;

}  // namespace Fw

#endif
