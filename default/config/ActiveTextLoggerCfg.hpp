#ifndef Config_ActiveTextLoggerCfg_HPP_
#define Config_ActiveTextLoggerCfg_HPP_

enum {
    ACTIVE_TEXT_LOGGER_ID_FILTER_SIZE = 25,  //!< Size of event ID filter
};

// Severity filter defaults for ActiveTextLogger
// true  = events pass through (are logged)
// false = events are filtered out (dropped)
enum {
    ACTIVE_TEXT_LOGGER_FILTER_WARNING_HI_DEFAULT = true,
    ACTIVE_TEXT_LOGGER_FILTER_WARNING_LO_DEFAULT = true,
    ACTIVE_TEXT_LOGGER_FILTER_COMMAND_DEFAULT = true,
    ACTIVE_TEXT_LOGGER_FILTER_ACTIVITY_HI_DEFAULT = true,
    ACTIVE_TEXT_LOGGER_FILTER_ACTIVITY_LO_DEFAULT = true,
    ACTIVE_TEXT_LOGGER_FILTER_DIAGNOSTIC_DEFAULT = false,  //!< DIAGNOSTIC events are filtered out by default
};

#endif /* Config_ActiveTextLoggerCfg_HPP_ */
