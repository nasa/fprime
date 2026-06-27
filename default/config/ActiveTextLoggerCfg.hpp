#ifndef Config_ActiveTextLoggerCfg_HPP_
#define Config_ActiveTextLoggerCfg_HPP_

#include <Fw/Types/EnabledEnumAc.hpp>

enum {
    ACTIVE_TEXT_LOGGER_ID_FILTER_SIZE = 25,  //!< Size of event ID filter
};

// Severity filter defaults for ActiveTextLogger
// Fw::Enabled::ENABLED  = events pass through (are logged)
// Fw::Enabled::DISABLED = events are filtered out (dropped)
namespace ActiveTextLoggerCfg {
static const Fw::Enabled FILTER_WARNING_HI_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_WARNING_LO_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_COMMAND_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_ACTIVITY_HI_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_ACTIVITY_LO_DEFAULT = Fw::Enabled::ENABLED;
static const Fw::Enabled FILTER_DIAGNOSTIC_DEFAULT = Fw::Enabled::DISABLED;  //!< DIAGNOSTIC events are filtered out
}  // namespace ActiveTextLoggerCfg

#endif /* Config_ActiveTextLoggerCfg_HPP_ */
