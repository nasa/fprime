#include <Fw/FPrimeBasicTypes.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Types/Assert.hpp>
#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>

namespace Svc {

ConsoleTextLoggerImpl::ConsoleTextLoggerImpl(const char* compName) : PassiveTextLoggerComponentBase(compName) {}

ConsoleTextLoggerImpl::~ConsoleTextLoggerImpl() {}

void ConsoleTextLoggerImpl::TextLogger_handler(FwIndexType portNum,
                                               FwEventIdType id,
                                               Fw::Time& timeTag,
                                               const Fw::LogSeverity& severity,
                                               Fw::TextLogString& text) {
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
