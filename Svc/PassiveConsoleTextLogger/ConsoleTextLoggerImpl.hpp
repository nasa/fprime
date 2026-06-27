#ifndef SVC_TEXT_LOGGER_IMPL_HPP
#define SVC_TEXT_LOGGER_IMPL_HPP

#include <Svc/PassiveConsoleTextLogger/PassiveTextLoggerComponentAc.hpp>
#include <Svc/Types/EventSeverityFilter/EventSeverityFilter.hpp>
#include <config/PassiveTextLoggerCfg.hpp>

namespace Svc {

class ConsoleTextLoggerImpl final : public PassiveTextLoggerComponentBase {
  public:
    // Only called by derived class
    ConsoleTextLoggerImpl(const char* compName);
    ~ConsoleTextLoggerImpl();

    //! Configure component with event ID filters
    void configure(const FwEventIdType* filteredIds, FwSizeType count);

    //! Set the filter state for a severity level
    //! \param severity The severity level to configure (FATAL is ignored)
    //! \param enabled ENABLED = events pass through, DISABLED = events are dropped
    void setSeverityFilter(Fw::LogSeverity severity, Fw::Enabled enabled);

  private:
    // downcalls for input ports
    void TextLogger_handler(FwIndexType portNum,
                            FwEventIdType id,
                            Fw::Time& timeTag,
                            const Fw::LogSeverity& severity,
                            Fw::TextLogString& text);

    // Event ID filters
    FwSizeType m_numFilteredIDs;
    FwEventIdType m_filteredIDs[PASSIVE_TEXT_LOGGER_ID_FILTER_SIZE];

    // Severity filter
    EventSeverityFilter m_severityFilter;
};

}  // namespace Svc

#endif
