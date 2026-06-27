#ifndef SVC_EVENT_SEVERITY_FILTER_HPP
#define SVC_EVENT_SEVERITY_FILTER_HPP

#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Log/LogSeverityEnumAc.hpp>
#include <Fw/Types/EnabledEnumAc.hpp>
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
    //! \param enabled ENABLED = events pass through, DISABLED = events are dropped
    void setFilter(Fw::LogSeverity severity, Fw::Enabled enabled);

    //! Check if an event with the given severity should be filtered out
    //! \param severity The severity of the event
    //! \return true if the event should be dropped, false if it should pass through
    bool isFiltered(Fw::LogSeverity severity) const;

    //! Query the filter state for a severity level
    //! \param severity The severity level to query (FATAL always returns ENABLED)
    //! \return ENABLED if events pass through, DISABLED if events are dropped
    Fw::Enabled isEnabled(Fw::LogSeverity severity) const;

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
    static const Fw::LogSeverity::t SEVERITY_ORDER[NUM_FILTER_LEVELS];

    //! Per-severity enabled state
    Fw::Enabled m_enabled[NUM_FILTER_LEVELS];
};

}  // namespace Svc

#endif  // SVC_EVENT_SEVERITY_FILTER_HPP
