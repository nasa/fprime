#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>
#include <Fw/Types/BasicTypes.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Fw/Cfg/Config.hpp>

enum {
    NUM_LOG_BUFFERS = 20
};

static char logBuffers[NUM_LOG_BUFFERS][FW_LOG_TEXT_BUFFER_SIZE];
static NATIVE_INT_TYPE logEntry = 0;

namespace Svc {

    void ConsoleTextLoggerImpl::init(void) {
        PassiveTextLoggerComponentBase::init();
        memset(logBuffers,0,sizeof(logBuffers));
    }
    
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

        // copy the test to the static buffers for the VxWorks log task.
        // Otherwise the string can go out of scope and the log task can print garbage

        strncpy(logBuffers[logEntry],text.toChar(),FW_LOG_TEXT_BUFFER_SIZE);
        // null terminate
        logBuffers[logEntry][FW_LOG_TEXT_BUFFER_SIZE - 1] = 0;

        Fw::Logger::logMsg("EVENT: (%d) (%d:%d,%d) %s: %s\n",
//          printf("EVENT: (%d) (%d:%d,%d) %s: %s\n",
            (POINTER_CAST)id,
            (POINTER_CAST)timeTag.getTimeBase(),
            (POINTER_CAST)timeTag.getSeconds(),
            (POINTER_CAST)timeTag.getUSeconds(),
            (POINTER_CAST)severityString,
//            (POINTER_CAST)text.toChar());
            (POINTER_CAST)logBuffers[logEntry]);

        logEntry = (logEntry + 1)%NUM_LOG_BUFFERS;
    }

}
