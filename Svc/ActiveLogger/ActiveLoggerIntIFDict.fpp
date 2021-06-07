enum QueueLogSeverity {
  QUEUE_LOG_FATAL = 1 @< A fatal non-recoverable event
  QUEUE_LOG_WARNING_HI = 2 @< A serious but recoverable event
  QUEUE_LOG_WARNING_LO = 3 @< A less serious but recoverable event
  QUEUE_LOG_COMMAND = 4 @< An activity related to commanding
  QUEUE_LOG_ACTIVITY_HI = 5 @< Important informational events
  QUEUE_LOG_ACTIVITY_LO = 6 @< Less important informational events
  QUEUE_LOG_DIAGNOSTIC = 7 @< Software diagnostic events
}

@ internal interface to send log messages to component thread
internal port loqQueue(
                        $id: FwEventIdType @< Log ID
                        timeTag: Fw.Time @< Time Tag
                        $severity: QueueLogSeverity @< The severity argument
                        args: Fw.LogBuffer @< Buffer containing serialized log entry
                      ) \
  priority 1 \
  drop
