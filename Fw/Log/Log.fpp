module Fw {

  type LogBuffer
  type TextLogString

  @ Enum representing event severity
  enum LogSeverity {
    FATAL = 1 @< A fatal non-recoverable event
    WARNING_HI = 2 @< A serious but recoverable event
    WARNING_LO = 3 @< A less serious but recoverable event
    COMMAND = 4 @< An activity related to commanding
    ACTIVITY_HI = 5 @< Important informational events
    ACTIVITY_LO = 6 @< Less important informational events
    DIAGNOSTIC = 7 @< Software diagnostic events
  }

  @ Event log port
  port Log(
            $id: FwEventIdType @< Log ID
            ref timeTag: Fw.Time @< Time Tag
            $severity: LogSeverity @< The severity argument
            ref args: LogBuffer @< Buffer containing serialized log entry
          )


  @ Text event log port
  @ Use for development and debugging, turn off for flight
  port LogText(
                $id: FwEventIdType @< Log ID
                ref timeTag: Fw.Time @< Time Tag
                $severity: LogSeverity @< The severity argument
                ref $text: Fw.TextLogString @< Text of log message
              )

}
