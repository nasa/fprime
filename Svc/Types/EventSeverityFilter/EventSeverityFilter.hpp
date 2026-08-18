#ifndef SVC_EVENT_SEVERITY_FILTER_HPP
#define SVC_EVENT_SEVERITY_FILTER_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Log/LogSeverityEnumAc.hpp>
#include <Fw/Types/SuccessEnumAc.hpp>

namespace Svc {

//! \class EventSeverityFilter
//! \brief Common severity-based event filtering utility
//!
//! Provides per-severity enabled/disabled state for event filtering.
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

    //! Query the filter state for a severity level
    //! \param severity The severity level to query (FATAL always returns true)
    //! \return true if events pass through, false if events are dropped
    bool isEnabled(Fw::LogSeverity severity) const;

    //! Map a filter index (0-5) to the corresponding LogSeverity
    //! \param index The filter index
    //! \param severity Output: the corresponding LogSeverity
    //! \return SUCCESS if index is valid, FAILURE otherwise
    static Fw::Success fromIndex(FwSizeType index, Fw::LogSeverity& severity);

  private:
    //! Map a LogSeverity to a filter array index
    //! \param severity The severity to map
    //! \param index Output: the resulting index
    //! \return SUCCESS if severity is filterable, FAILURE for FATAL/unknown
    static Fw::Success toIndex(Fw::LogSeverity severity, FwSizeType& index);

    //! Severity values in filter index order
    static constexpr Fw::LogSeverity::t SEVERITY_ORDER[NUM_FILTER_LEVELS] = {
        Fw::LogSeverity::WARNING_HI,  Fw::LogSeverity::WARNING_LO,  Fw::LogSeverity::COMMAND,
        Fw::LogSeverity::ACTIVITY_HI, Fw::LogSeverity::ACTIVITY_LO, Fw::LogSeverity::DIAGNOSTIC,
    };

    // Verify each severity maps to its expected filter index
    static_assert(SEVERITY_ORDER[0] == Fw::LogSeverity::WARNING_HI, "WARNING_HI must map to index 0");
    static_assert(SEVERITY_ORDER[1] == Fw::LogSeverity::WARNING_LO, "WARNING_LO must map to index 1");
    static_assert(SEVERITY_ORDER[2] == Fw::LogSeverity::COMMAND, "COMMAND must map to index 2");
    static_assert(SEVERITY_ORDER[3] == Fw::LogSeverity::ACTIVITY_HI, "ACTIVITY_HI must map to index 3");
    static_assert(SEVERITY_ORDER[4] == Fw::LogSeverity::ACTIVITY_LO, "ACTIVITY_LO must map to index 4");
    static_assert(SEVERITY_ORDER[5] == Fw::LogSeverity::DIAGNOSTIC, "DIAGNOSTIC must map to index 5");

    //! Per-severity enabled state
    bool m_enabled[NUM_FILTER_LEVELS];
};

}  // namespace Svc

#endif  // SVC_EVENT_SEVERITY_FILTER_HPP
