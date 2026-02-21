module Svc {

  @ A component for printing text logs to the console and optionally a file
  active component ActiveTextLogger {

    @ Logging port
    sync input port TextLogger: Fw.LogText

    @ Internal interface to send log text messages to component thread
    internal port TextQueue(
                             $text: string size FW_LOG_STRING_MAX_SIZE @< The text string
                           ) \
      priority 1 \
      drop

  }

}
