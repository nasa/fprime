#ifndef SVC_EVENT_SEVERITY_FILTER_HPP
#define SVC_EVENT_SEVERITY_FILTER_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Log/LogSeverityEnumAc.hpp>

namespace Svc {

//! \class EventSeverityFilter
//! \brief Common severity-based event filtering utility
//!
//! Provides per-severity enabled/disabled state for event filtering.
//! Used by EventManager, ActiveTextLogger, and PassiveConsoleTextLogger
//! to share a single implementation of severity-based event filtering.
//! FATAL events are never filtered.
class EventSeverityFilter {
  public:
    //! Number of filterable severity levels (excludes FATAL)
    static constexpr FwSizeType NUM_FILTER_LEVELS = 6;

    //! Construct with all severity levels enabled (events pass through)
    EventSeverityFilter();

    //! Set the filter state for a severity level
    //! \param severity The severity level to configure (FATAL is ignored)
    //! \param enabled true = events pass through, false = events are dropped
    void setFilter(Fw::LogSeverity severity, bool enabled);

    //! Check if an event with the given severity should be filtered out
    //! \param severity The severity of the event
    //! \return true if the event should be dropped, false if it should pass through
    bool isFiltered(Fw::LogSeverity severity) const;

    //! Check if a severity level is enabled (events pass through)
    //! \param severity The severity level to query (FATAL always returns true)
    //! \return true if events of this severity pass through
    bool isEnabled(Fw::LogSeverity severity) const;

  private:
    //! Map a LogSeverity to a filter array index
    //! \param severity The severity to map
    //! \param index Output: the resulting index
    //! \return true if severity is filterable (not FATAL), false otherwise
    static bool toIndex(Fw::LogSeverity severity, FwSizeType& index);

    //! Per-severity enabled state. true = events pass through.
    bool m_enabled[NUM_FILTER_LEVELS];
};

}  // namespace Svc

#endif  // SVC_EVENT_SEVERITY_FILTER_HPP
