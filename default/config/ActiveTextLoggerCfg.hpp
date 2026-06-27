#ifndef Config_ActiveTextLoggerCfg_HPP_
#define Config_ActiveTextLoggerCfg_HPP_

enum {
    ACTIVE_TEXT_LOGGER_ID_FILTER_SIZE = 25,  //!< Size of event ID filter
};

// Severity filter defaults for ActiveTextLogger
// true = events of this severity pass through (are logged)
// false = events of this severity are filtered out (dropped)
enum {
    ACTIVE_TEXT_LOGGER_FILTER_WARNING_HI_DEFAULT = true,   //!< WARNING HI events pass through
    ACTIVE_TEXT_LOGGER_FILTER_WARNING_LO_DEFAULT = true,   //!< WARNING LO events pass through
    ACTIVE_TEXT_LOGGER_FILTER_COMMAND_DEFAULT = true,      //!< COMMAND events pass through
    ACTIVE_TEXT_LOGGER_FILTER_ACTIVITY_HI_DEFAULT = true,  //!< ACTIVITY HI events pass through
    ACTIVE_TEXT_LOGGER_FILTER_ACTIVITY_LO_DEFAULT = true,  //!< ACTIVITY LO events pass through
    ACTIVE_TEXT_LOGGER_FILTER_DIAGNOSTIC_DEFAULT = false,  //!< DIAGNOSTIC events are filtered out
};

#endif /* Config_ActiveTextLoggerCfg_HPP_ */
