#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Logger/Logger.hpp>

namespace Svc {

    ConsoleTextLoggerImpl::ConsoleTextLoggerImpl(const char* compName) :
        PassiveTextLoggerComponentBase(compName) {
    }

    void ConsoleTextLoggerImpl::init(NATIVE_INT_TYPE instanceId) {
        PassiveTextLoggerComponentBase::init(instanceId);
    }

    ConsoleTextLoggerImpl::~ConsoleTextLoggerImpl() {}

    void ConsoleTextLoggerImpl::TextLogger_handler(NATIVE_INT_TYPE portNum, FwEventIdType id, Fw::Time &timeTag, const Fw::LogSeverity& severity, Fw::TextLogString &text) {
        const char *severityString = "UNKNOWN";
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
        Fw::Logger::logMsg("EVENT: (%d) (%d:%d,%d) %s: %s\n",
                id, timeTag.getTimeBase(), timeTag.getSeconds(), timeTag.getUSeconds(),
                reinterpret_cast<POINTER_CAST>(severityString), reinterpret_cast<POINTER_CAST>(text.toChar()));
    }
}
