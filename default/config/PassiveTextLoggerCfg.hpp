#ifndef Config_PassiveTextLoggerCfg_HPP_
#define Config_PassiveTextLoggerCfg_HPP_

enum {
    PASSIVE_TEXT_LOGGER_ID_FILTER_SIZE = 25,  //!< Size of event ID filter
};

// Severity filter defaults for PassiveTextLogger
// true  = events pass through (are logged)
// false = events are filtered out (dropped)
enum {
    PASSIVE_TEXT_LOGGER_FILTER_WARNING_HI_DEFAULT = true,
    PASSIVE_TEXT_LOGGER_FILTER_WARNING_LO_DEFAULT = true,
    PASSIVE_TEXT_LOGGER_FILTER_COMMAND_DEFAULT = true,
    PASSIVE_TEXT_LOGGER_FILTER_ACTIVITY_HI_DEFAULT = true,
    PASSIVE_TEXT_LOGGER_FILTER_ACTIVITY_LO_DEFAULT = true,
    PASSIVE_TEXT_LOGGER_FILTER_DIAGNOSTIC_DEFAULT = false,
};

#endif /* Config_PassiveTextLoggerCfg_HPP_ */
