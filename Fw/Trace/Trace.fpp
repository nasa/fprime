module Fw {

  type TraceBuffer

  @Enum representing trace Types
  @TODO: Talk with Tim to figure out what enums we'd want for trace types
  enum TraceType {
    FULL = 1 @< A full log file downliked
    ABRIDGED = 2 @< An abridged version of trace
  }

  @ Port for sending Traces
  port Trace(
            $id: FwTraceIdType @< Trace ID
            ref timeTag: Fw.Time @< Time Tag
            $type: TraceType @< The trace type argument
            ref args: TraceBuffer @< Buffer containing serialized trace entry
          )
}