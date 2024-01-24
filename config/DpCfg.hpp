/*
 * FileDownlinkCfg.hpp:
 *
 * Configuration settings for file downlink component.
 */

#ifndef SVC_DP_CONFIG_HPP_
#define SVC_DP_CONFIG_HPP_
#include <FpConfig.hpp>

namespace Svc {
    // This format string is a filename template for DpWriter to write files
    // and DpCatalog to read. The directories where the data products reside
    // is part of the configuration for the DpWriter and DpCatalog.
    constexpr const char *DP_FILENAME_FORMAT = "Dp_%08" PRIu32 "_%08" PRIu32 "_%08" PRIu32 ".fdp";

}

#endif /* SVC_DP_CONFIG_HPP_ */
