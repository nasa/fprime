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
    INPUT_WARNING_HI_DEFAULT = true, //!< WARNING HI events are filtered at input
    INPUT_WARNING_LO_DEFAULT = true, //!< WARNING LO events are filtered at input
    INPUT_COMMAND_DEFAULT = true, //!< COMMAND events are filtered at input
    INPUT_ACTIVITY_HI_DEFAULT = true, //!< ACTIVITY HI events are filtered at input
    INPUT_ACTIVITY_LO_DEFAULT = true, //!< ACTIVITY LO  events are filtered at input
    INPUT_DIAGNOSTIC_DEFAULT = false, //!< DIAGNOSTIC events are filtered at input

    SEND_WARNING_HI_DEFAULT = true, //!< WARNING HI events are filtered at output
    SEND_WARNING_LO_DEFAULT = true, //!< WARNING LO events are filtered at output
    SEND_COMMAND_DEFAULT = true, //!< COMMAND events are filtered at output
    SEND_ACTIVITY_HI_DEFAULT = true, //!< ACTIVITY HO events are filtered at output
    SEND_ACTIVITY_LO_DEFAULT = true, //!< ACTIVITY LO events are filtered at output
    SEND_DIAGNOSTIC_DEFAULT = false //!< DIAGNOSTIC events are filtered at output
};

// set event history circular buffer sizes

enum {
    FATAL_EVENT_CB_DEPTH = 5, //!< FATAL event circular buffer depth
    WARNING_HI_EVENT_CB_DEPTH = 10, //!< WARNING HI event circular buffer depth
    WARNING_LO_EVENT_CB_DEPTH = 20, //!< WARNING LO event circular buffer depth
    COMMAND_EVENT_CB_DEPTH = 20, //!< COMMAND event circular buffer depth
    ACTIVITY_HI_EVENT_CB_DEPTH = 20, //!< ACTIVITY HI event circular buffer depth
    ACTIVITY_LO_EVENT_CB_DEPTH = 20, //!< ACTIVITY LO circular buffer depth
    DIAGNOSTIC_EVENT_CB_DEPTH = 50, //!< DIAGNOSTIC event circular buffer depth
    TELEM_ID_FILTER_SIZE = 25, //!< Size of telemetry ID filter
};

#endif /* ACTIVELOGGER_ACTIVELOGGERIMPLCFG_HPP_ */
