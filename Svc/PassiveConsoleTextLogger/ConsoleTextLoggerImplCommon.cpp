#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Logger/Logger.hpp>

namespace Svc {

    ConsoleTextLoggerImpl::ConsoleTextLoggerImpl(const char* compName) :
        PassiveTextLoggerComponentBase(compName),m_displayLine(2),m_pointerLine(0) {
    }

    void ConsoleTextLoggerImpl::init(void) {
        PassiveTextLoggerComponentBase::init();
    }

    ConsoleTextLoggerImpl::~ConsoleTextLoggerImpl(void) {}

    void ConsoleTextLoggerImpl::TextLogger_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, Fw::TextLogSeverity severity, Fw::TextLogString &text) {
        const char *severityString = "UNKNOWN";
        switch (severity) {
            case Fw::TEXT_LOG_FATAL:
                severityString = "FATAL";
                break;
            case Fw::TEXT_LOG_WARNING_HI:
                severityString = "WARNING_HI";
                break;
            case Fw::TEXT_LOG_WARNING_LO:
                severityString = "WARNING_LO";
                break;
            case Fw::TEXT_LOG_COMMAND:
                severityString = "COMMAND";
                break;
            case Fw::TEXT_LOG_ACTIVITY_HI:
                severityString = "ACTIVITY_HI";
                break;
            case Fw::TEXT_LOG_ACTIVITY_LO:
                severityString = "ACTIVITY_LO";
                break;
            case Fw::TEXT_LOG_DIAGNOSTIC:
                severityString = "DIAGNOSTIC";
                break;
            default:
                severityString = "SEVERITY ERROR";
                break;
        }
        Fw::Logger::logMsg("EVENT: (%d) (%d:%d,%d) %s: %s\n",
                id, timeTag.getTimeBase(), timeTag.getSeconds(), timeTag.getUSeconds(),
                reinterpret_cast<POINTER_CAST>(severityString), reinterpret_cast<POINTER_CAST>(text.toChar()));
    }
}
