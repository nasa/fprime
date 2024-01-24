/*
 * FileDownlinkCfg.hpp:
 *
 * Configuration settings for file downlink component.
 */

#ifndef SVC_DPCATALOG_CONFIG_HPP_
#define SVC_DPCATALOG_CONFIG_HPP_
#include <FpConfig.hpp>

namespace Svc {
    // Sets the maximum number of directories where
    // data products can be stored. The array passed
    // to the initializer for DpCatalog cannot exceed
    // this size.
    static const FwNativeUIntType DP_MAX_DIRECTORIES = 2;
}

#endif /* SVC_DPCATALOG_CONFIG_HPP_ */
