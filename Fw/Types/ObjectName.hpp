// ======================================================================
// @file   ObjectName.hpp
// @author F Prime
// @brief  A string sized to store an object name
// ======================================================================

#ifndef FW_OBJECT_NAME_HPP
#define FW_OBJECT_NAME_HPP

#include "Fw/Types/StringTemplate.hpp"

namespace Fw {

using ObjectName = StringTemplate<FW_OBJ_NAME_BUFFER_SIZE>;

}  // namespace Fw

#endif
