/*
 * ActiveLoggerImplCfg.hpp
 *
 *  Created on: Apr 16, 2015
 *      Author: tcanham
 */

#ifndef ACTIVELOGGER_ACTIVELOGGERIMPLCFG_HPP_
#define ACTIVELOGGER_ACTIVELOGGERIMPLCFG_HPP_

#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>

// set default filters

enum {
    FILTER_WARNING_HI_DEFAULT = true, //!< WARNING HI events are filtered at input
    FILTER_WARNING_LO_DEFAULT = true, //!< WARNING LO events are filtered at input
    FILTER_COMMAND_DEFAULT = true, //!< COMMAND events are filtered at input
    FILTER_ACTIVITY_HI_DEFAULT = true, //!< ACTIVITY HI events are filtered at input
    FILTER_ACTIVITY_LO_DEFAULT = true, //!< ACTIVITY LO  events are filtered at input
    FILTER_DIAGNOSTIC_DEFAULT = false, //!< DIAGNOSTIC events are filtered at input
};


enum {
    TELEM_ID_FILTER_SIZE = 25, //!< Size of telemetry ID filter
};

#endif /* ACTIVELOGGER_ACTIVELOGGERIMPLCFG_HPP_ */
