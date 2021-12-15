module Svc {

  @ A component to implement log messages with a print to the console
  passive component PassiveTextLogger {

    @ Logging port
    sync input port TextLogger: Fw.LogText

  }

}
