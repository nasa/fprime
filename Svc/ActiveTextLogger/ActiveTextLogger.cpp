// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/ActiveTextLogger/ActiveTextLogger.hpp>
#include <ctime>

namespace Svc {
static_assert(std::numeric_limits<FwSizeType>::max() >= ACTIVE_TEXT_LOGGER_ID_FILTER_SIZE,
              "ACTIVE_TEXT_LOGGER_ID_FILTER_SIZE must fit within range of FwSizeType");

// ----------------------------------------------------------------------
// Initialization/Exiting
// ----------------------------------------------------------------------

ActiveTextLogger::ActiveTextLogger(const char* name)
    : ActiveTextLoggerComponentBase(name), m_log_file(), m_numFilteredIDs(0) {}

ActiveTextLogger::~ActiveTextLogger() {}

void ActiveTextLogger::configure(const FwEventIdType* filteredIds, FwSizeType count) {
    FW_ASSERT(count < ACTIVE_TEXT_LOGGER_ID_FILTER_SIZE, static_cast<FwAssertArgType>(count),
              ACTIVE_TEXT_LOGGER_ID_FILTER_SIZE);

    this->m_numFilteredIDs = count;
    for (FwSizeType entry = 0; entry < count; entry++) {
        this->m_filteredIDs[entry] = filteredIds[entry];
    }
}

// ----------------------------------------------------------------------
// Handlers to implement for typed input ports
// ----------------------------------------------------------------------

void ActiveTextLogger::TextLogger_handler(FwIndexType portNum,
                                          FwEventIdType id,
                                          Fw::Time& timeTag,
                                          const Fw::LogSeverity& severity,
                                          Fw::TextLogString& text) {
    // Currently not doing any input filtering
    // TKC - 5/3/2018 - remove diagnostic
    if (Fw::LogSeverity::DIAGNOSTIC == severity.e) {
        return;
    }
    // Check event ID filters
    for (FwSizeType i = 0; i < this->m_numFilteredIDs; i++) {
        if (this->m_filteredIDs[i] == id) {
            return;
        }
    }

    // Format the string here, so that it is done in the task context
    // of the caller.  Format doc borrowed from PassiveTextLogger.
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
    // Overflow is allowed and truncation accepted
    Fw::InternalInterfaceString intText;
    (void)intText.format("EVENT: (%" PRI_FwEventIdType ") (%" PRI_FwTimeBaseStoreType ":%" PRIu32 ",%" PRIu32
                         ") %s: %s\n",
                         id, static_cast<FwTimeBaseStoreType>(timeTag.getTimeBase()), timeTag.getSeconds(),
                         timeTag.getUSeconds(), severityString, text.toChar());

    // Call internal interface so that everything else is done on component thread,
    // this helps ensure consistent ordering of the printed text:
    this->TextQueue_internalInterfaceInvoke(intText);
}

// ----------------------------------------------------------------------
// Internal interface handlers
// ----------------------------------------------------------------------

void ActiveTextLogger::TextQueue_internalInterfaceHandler(const Fw::InternalInterfaceString& text) {
    // Print to console:
    Fw::Logger::log(text);

    // Print to file if there is one:
    (void)this->m_log_file.write_to_log(text.toChar(), text.length());  // Ignoring return status
}

// ----------------------------------------------------------------------
// Helper Methods
// ----------------------------------------------------------------------

bool ActiveTextLogger::set_log_file(const char* fileName, const U32 maxSize, const U32 maxBackups) {
    FW_ASSERT(fileName != nullptr);

    return this->m_log_file.set_log_file(fileName, maxSize, maxBackups);
}

}  // namespace Svc
