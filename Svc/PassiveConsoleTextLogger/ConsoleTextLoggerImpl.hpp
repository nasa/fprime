#ifndef SVC_TEXT_LOGGER_IMPL_HPP
#define SVC_TEXT_LOGGER_IMPL_HPP

#include <Svc/PassiveConsoleTextLogger/PassiveTextLoggerComponentAc.hpp>
#include <config/PassiveTextLoggerCfg.hpp>

namespace Svc {

class ConsoleTextLoggerImpl final : public PassiveTextLoggerComponentBase {
  public:
    // Only called by derived class
    ConsoleTextLoggerImpl(const char* compName);
    ~ConsoleTextLoggerImpl();

    //! Configure component with event ID filters
    void configure(const FwEventIdType* filteredIds, FwSizeType count);

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
};

}  // namespace Svc

#endif
