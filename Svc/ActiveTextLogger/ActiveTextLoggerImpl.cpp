// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.

#include <Svc/ActiveTextLogger/ActiveTextLoggerImpl.hpp>
#include <Fw/Types/Assert.hpp>
#include <Fw/Logger/Logger.hpp>
#include <time.h>

namespace Svc {

    // ----------------------------------------------------------------------
    // Initialization/Exiting
    // ----------------------------------------------------------------------

    ActiveTextLoggerComponentImpl::ActiveTextLoggerComponentImpl(const char* name) :
        ActiveTextLoggerComponentBase(name),
        m_log_file()
    {

    }

    ActiveTextLoggerComponentImpl::~ActiveTextLoggerComponentImpl()
    {

    }

    void ActiveTextLoggerComponentImpl::init(NATIVE_INT_TYPE queueDepth, NATIVE_INT_TYPE instance)
    {
        ActiveTextLoggerComponentBase::init(queueDepth,instance);
    }

    // ----------------------------------------------------------------------
    // Handlers to implement for typed input ports
    // ----------------------------------------------------------------------

    void ActiveTextLoggerComponentImpl::TextLogger_handler(NATIVE_INT_TYPE portNum,
                                                  FwEventIdType id,
                                                  Fw::Time &timeTag,
                                                  Fw::TextLogSeverity severity,
                                                  Fw::TextLogString &text)
    {

        // Currently not doing any input filtering
        // TKC - 5/3/2018 - remove diagnostic
        if (Fw::TEXT_LOG_DIAGNOSTIC == severity) {
            return;
        }

        // Format the string here, so that it is done in the task context
        // of the caller.  Format doc borrowed from PassiveTextLogger.
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

        // TODO: Add calling task id to format string
        char textStr[FW_INTERNAL_INTERFACE_STRING_MAX_SIZE];
        NATIVE_INT_TYPE stat;

        if (timeTag.getTimeBase() == TB_WORKSTATION_TIME) {

            time_t t = timeTag.getSeconds();
            // Using localtime_r prevents any other calls to localtime (from another thread for example) from
            // interfering with our time object before we use it. However, the null pointer check is still needed
            // to ensure a successful call
            tm tm;
            if (localtime_r(&t, &tm) == NULL) {
                return;
            }

            stat = snprintf(textStr,
                            FW_INTERNAL_INTERFACE_STRING_MAX_SIZE,
                            "EVENT: (%d) (%04d-%02d-%02dT%02d:%02d:%02d.%03u) %s: %s\n",
                            id, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour,
                            tm.tm_min,tm.tm_sec,timeTag.getUSeconds(),
                            severityString,text.toChar());
        }
        else {

            stat = snprintf(textStr,
                            FW_INTERNAL_INTERFACE_STRING_MAX_SIZE,
                            "EVENT: (%d) (%d:%d,%d) %s: %s\n",
                            id,timeTag.getTimeBase(),timeTag.getSeconds(),timeTag.getUSeconds(),severityString,text.toChar());
        }

        // If there was a error then just return:
        if (stat <= 0) {
            return;
        }
        // If there was string text truncation:
        else if (stat >= FW_INTERNAL_INTERFACE_STRING_MAX_SIZE) {
            // Do nothing
        }

        // Call internal interface so that everything else is done on component thread,
        // this helps ensure consistent ordering of the printed text:
        Fw::InternalInterfaceString intText(textStr);
        this->TextQueue_internalInterfaceInvoke(intText);
    }

    // ----------------------------------------------------------------------
    // Internal interface handlers
    // ----------------------------------------------------------------------

    void ActiveTextLoggerComponentImpl::TextQueue_internalInterfaceHandler(const Fw::InternalInterfaceString& text)
    {

        // Print to console:
        Fw::Logger::logMsg(text.toChar(),0,0,0,0,0,0,0,0,0);

        // Print to file if there is one:
        (void) this->m_log_file.write_to_log(text.toChar(), text.length());  // Ignoring return status

    }

    // ----------------------------------------------------------------------
    // Helper Methods
    // ----------------------------------------------------------------------

    bool ActiveTextLoggerComponentImpl::set_log_file(const char* fileName, const U32 maxSize, const U32 maxBackups)
    {
        FW_ASSERT(fileName != NULL);

        return this->m_log_file.set_log_file(fileName, maxSize, maxBackups);
    }


} // namespace Svc
