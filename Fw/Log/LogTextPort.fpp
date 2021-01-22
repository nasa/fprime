module Fw {

  type TextLogString

  enum TextLogSeverity {
    FATAL = 1 @< A fatal non-recoverable event
    WARNING_HI = 2 @< A serious but recoverable event
    WARNING_LO = 3 @< A less serious but recoverable event
    COMMAND = 4 @< An activity related to commanding
    ACTIVITY_HI = 5 @< Important informational events
    ACTIVITY_LO = 6 @< Less important informational events
    DIAGNOSTIC = 7 @< Software diagnostic events
  }

  @ Text Log port - meant to be used for development and debugging, turn off for flight
  port LogText(
                $id: FwEventIdType @< Log ID
                ref timeTag: Fw.Time @< Time Tag
                $severity: TextLogSeverity @< The severity argument
                ref $text: Fw.TextLogString @< Text of log message
              )

}
