// ======================================================================
// @file   SdlsKeyBuffer.hpp
// @brief  An on-stack buffer sized to hold an SDLS key
// ======================================================================

#ifndef SVC_CCSDS_SDLS_KEY_BUFFER_HPP
#define SVC_CCSDS_SDLS_KEY_BUFFER_HPP

#include "Fw/Types/LinearBufferTemplate.hpp"
#include "SdlsKeyConfig/FppConstantsAc.hpp"

namespace Svc {
namespace Ccsds {

using SdlsKeyBuffer = Fw::LinearBufferTemplate<SdlsCfg::MAX_SDLS_KEY_SIZE>;

}  // namespace Ccsds
}  // namespace Svc

#endif
