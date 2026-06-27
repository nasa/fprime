#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>

namespace Svc {
static_assert(std::numeric_limits<FwSizeType>::max() >= PASSIVE_TEXT_LOGGER_ID_FILTER_SIZE,
              "PASSIVE_TEXT_LOGGER_ID_FILTER_SIZE must fit within range of FwSizeType");

ConsoleTextLoggerImpl::ConsoleTextLoggerImpl(const char* compName)
    : PassiveTextLoggerComponentBase(compName), m_numFilteredIDs(0), m_severityFilter() {
    // Set severity filter defaults from config
    this->m_severityFilter.setFilter(Fw::LogSeverity::WARNING_HI, PASSIVE_TEXT_LOGGER_FILTER_WARNING_HI_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::WARNING_LO, PASSIVE_TEXT_LOGGER_FILTER_WARNING_LO_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::COMMAND, PASSIVE_TEXT_LOGGER_FILTER_COMMAND_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::ACTIVITY_HI, PASSIVE_TEXT_LOGGER_FILTER_ACTIVITY_HI_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::ACTIVITY_LO, PASSIVE_TEXT_LOGGER_FILTER_ACTIVITY_LO_DEFAULT);
    this->m_severityFilter.setFilter(Fw::LogSeverity::DIAGNOSTIC, PASSIVE_TEXT_LOGGER_FILTER_DIAGNOSTIC_DEFAULT);
}

ConsoleTextLoggerImpl::~ConsoleTextLoggerImpl() {}

void ConsoleTextLoggerImpl::configure(const FwEventIdType* filteredIds, FwSizeType count) {
    FW_ASSERT(count < PASSIVE_TEXT_LOGGER_ID_FILTER_SIZE, static_cast<FwAssertArgType>(count),
              PASSIVE_TEXT_LOGGER_ID_FILTER_SIZE);

    this->m_numFilteredIDs = count;
    for (FwSizeType entry = 0; entry < count; entry++) {
        this->m_filteredIDs[entry] = filteredIds[entry];
    }
}

void ConsoleTextLoggerImpl::setSeverityFilter(Fw::LogSeverity severity, bool enabled) {
    this->m_severityFilter.setFilter(severity, enabled);
}

void ConsoleTextLoggerImpl::TextLogger_handler(FwIndexType portNum,
                                               FwEventIdType id,
                                               Fw::Time& timeTag,
                                               const Fw::LogSeverity& severity,
                                               Fw::TextLogString& text) {
    // Check severity filter
    if (this->m_severityFilter.isFiltered(severity)) {
        return;
    }

    // Check event ID filters
    for (FwSizeType i = 0; i < this->m_numFilteredIDs; i++) {
        if (this->m_filteredIDs[i] == id) {
            return;
        }
    }

    const char* severityString = nullptr;
    switch (severity.e) {
        case Fw::LogSeverity::FATAL:
            severityString = "FATAL";
            break;
        case Fw::LogSeverity::WARNING_HI:
            severityString = "WARNING_HI";
            break;
        case Fw::LogSeverity::WARNING_LO:
            severityString = "WARNING_LO";
            break;
        case Fw::LogSeverity::COMMAND:
            severityString = "COMMAND";
            break;
        case Fw::LogSeverity::ACTIVITY_HI:
            severityString = "ACTIVITY_HI";
            break;
        case Fw::LogSeverity::ACTIVITY_LO:
            severityString = "ACTIVITY_LO";
            break;
        case Fw::LogSeverity::DIAGNOSTIC:
            severityString = "DIAGNOSTIC";
            break;
        default:
            severityString = "SEVERITY ERROR";
            break;
    }
    Fw::Logger::log("EVENT: (%" PRI_FwEventIdType ") (%" PRI_FwTimeBaseStoreType ":%" PRIu32 ",%" PRIu32 ") %s: %s\n",
                    id, static_cast<FwTimeBaseStoreType>(timeTag.getTimeBase()), timeTag.getSeconds(),
                    timeTag.getUSeconds(), severityString, text.toChar());
}
}  // namespace Svc
