module Fw {

  type TlmBuffer

  @ Telemetry port
  port Tlm(
            $id: FwChanIdType @< Telemetry Channel ID
            ref timeTag: Fw.Time @< Time Tag
            ref val: TlmBuffer @< Buffer containing serialized telemetry value
          )

}
