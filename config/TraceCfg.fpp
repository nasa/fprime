# ======================================================================
# FPP file for Version configuration
# ======================================================================
# From Design review, it was requested that we add more trace types such as
# interrupts, exceptions, context switch,queue add/remove, EVRs, 
# semaphore takes(?), stack trace, DMAs
# Also consider adding adding state machine transitions, timer acquisition 
# and release traces, and EVRS to the category of traces that are automated. 
# Also consider what a potential IO trace could look like. A ring buffer for IO 
# is always requested for 1553 and recently M-Bus on Psyche. Presumably a 
# limited IO trace of some could be packaged with the autopsy log and be useful.
    
#Note: Trace Types are configurable by the project and defined here. 
#      It is stored as a bitmask that will be used for log filtering.
#      Current design allows a max of 16 trace types

module Fw {

  module TraceCfg {

    @Enum representing trace Types (aka Trace Categories)
    enum TraceType {
        MESSAGE_QUEUE = 0 @< Trace type of message wait
        MESSAGE_DEQUEUE = 1 @< Trace type of message dequeue
        PORT_CALL = 2 @< Trace type of port invocations
        USER = 3 @< user invoked trace
        #=====Do Not Delete above======#
        INTERRUPTS = 4 @<Trace type for interrupts handled 
        CONTEXT_SWITCH = 5 @<Trace type for context switching
        EVENTS = 6 @<Trace type for Events generated
        SEMAPHORES = 7 @<trace type for semaphores taken
        STATE_MACHINE = 8 @<trace type for state machine transitions
        MAX_TRACE_TYPES = 9
    }
  }
}