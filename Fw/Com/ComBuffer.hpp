// ======================================================================
// @file   ComBuffer.hpp
// @author F Prime
// @brief  A buffer sized for communication packets
// ======================================================================

#ifndef FW_COM_BUFFER_HPP
#define FW_COM_BUFFER_HPP

#include "Fw/Types/LinearBufferTemplate.hpp"

namespace Fw {

using ComBuffer = LinearBufferTemplate<FW_COM_BUFFER_MAX_SIZE>;

}  // namespace Fw

#endif
