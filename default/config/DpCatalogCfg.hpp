/*
 * FileDownlinkCfg.hpp:
 *
 * Configuration settings for file downlink component.
 */

#ifndef SVC_DPCATALOG_CONFIG_HPP_
#define SVC_DPCATALOG_CONFIG_HPP_
#include <Fw/FPrimeBasicTypes.hpp>

namespace Svc {
// Sets the maximum number of directories where
// data products can be stored. The array passed
// to the initializer for DpCatalog cannot exceed
// this size.
static const FwIndexType DP_MAX_DIRECTORIES = 2;
static const FwIndexType DP_MAX_FILES = 127;
}  // namespace Svc

#endif /* SVC_DPCATALOG_CONFIG_HPP_ */
