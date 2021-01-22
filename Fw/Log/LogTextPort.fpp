module Fw {

  type TextLogString

  @ Text Log port - meant to be used for development and debugging, turn off for flight
  port LogText(
                $id: FwEventIdType @< Log ID
                ref timeTag: Fw.Time @< Time Tag
                $severity: LogSeverity @< The severity argument
                ref $text: Fw.TextLogString @< Text of log message
              )

}
