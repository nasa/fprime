module Fw {

  type TraceBuffer


  @ Port for sending Traces
  port Trace(
            $id: FwTraceIdType @< Trace ID
            ref timeTag: Fw.Time @< Time Tag
            $type: TraceCfg.TraceType @< The trace type argument
            ref args: TraceBuffer @< Buffer containing serialized trace entry
          )
}