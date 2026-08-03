/*
 * EventManagerCfg.hpp
 *
 *  Created on: Apr 16, 2015
 *      Author: tcanham
 */

#ifndef Config_EventManagerCfg_HPP_
#define Config_EventManagerCfg_HPP_

// Severity filter defaults
// true  = events pass through
// false = events are filtered out
enum {
    FILTER_WARNING_HI_DEFAULT = true,
    FILTER_WARNING_LO_DEFAULT = true,
    FILTER_COMMAND_DEFAULT = true,
    FILTER_ACTIVITY_HI_DEFAULT = true,
    FILTER_ACTIVITY_LO_DEFAULT = true,
    FILTER_DIAGNOSTIC_DEFAULT = false,
};

enum {
    TELEM_ID_FILTER_SIZE = 25,  //!< Size of telemetry ID filter
};

#endif /* Config_EventManagerCfg_HPP_ */
