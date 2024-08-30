# ======================================================================
# FPP file for Version configuration
# ======================================================================

module Fw {

  module TraceCfg {

    @Enum representing trace Types
    @TODO: Talk with Tim to figure out what enums we'd want for trace types
    enum TraceType {
        MESSAGE_QUEUE = 1 @< Trace type of message wait
        MESSAGE_DEQUEUE = 2 @< Trace type of message dequeue
        PORT_CALL = 3 @< Trace type of port invocations
        USER = 4 @< user invoked trace
    }
  }

}