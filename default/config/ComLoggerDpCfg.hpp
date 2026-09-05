// ======================================================================
// \title  ComLoggerDpCfg.hpp
// \author Auto-generated
// \brief  Configuration constants for ComLoggerDp component
// ======================================================================

#ifndef CONFIG_COMLOGGERDPCFG_HPP
#define CONFIG_COMLOGGERDPCFG_HPP

#include "Fw/Types/BasicTypes.hpp"

namespace Svc {

    //! Sentry value inserted before each ComBuffer in data products (0xDEADBEEF)
    //! Used to identify buffer boundaries when reconstructing telemetry from partial containers
    constexpr U32 ComLoggerDpSentry = 0xDEADBEEF;

}

#endif
