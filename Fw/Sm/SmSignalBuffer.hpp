// ======================================================================
// @file   SmSignalBuffer.hpp
// @author F Prime
// @brief  A buffer sized for state machine signals
// ======================================================================

#ifndef FW_SM_SIGNAL_BUFFER_HPP
#define FW_SM_SIGNAL_BUFFER_HPP

#include "Fw/Types/LinearBufferTemplate.hpp"

namespace Fw {

using SmSignalBuffer = LinearBufferTemplate<FW_SM_SIGNAL_BUFFER_MAX_SIZE>;

}  // namespace Fw

#endif
