/*
 * EventManagerCfg.hpp
 *
 *  Created on: Apr 16, 2015
 *      Author: tcanham
 */

#ifndef Config_EventManagerCfg_HPP_
#define Config_EventManagerCfg_HPP_

#include <Fw/Types/EnabledEnumAc.hpp>

// Severity filter defaults
// Fw::Enabled::ENABLED  = events pass through
// Fw::Enabled::DISABLED = events are filtered out
namespace EventManagerCfg {
static const Fw::Enabled FILTER_WARNING_HI_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_WARNING_LO_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_COMMAND_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_ACTIVITY_HI_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_ACTIVITY_LO_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_DIAGNOSTIC_DEFAULT = Fw::Enabled::DISABLED;
}  // namespace EventManagerCfg

enum {
    TELEM_ID_FILTER_SIZE = 25,  //!< Size of telemetry ID filter
};

#endif /* Config_EventManagerCfg_HPP_ */
