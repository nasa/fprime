module Fw {

  type TlmBuffer

  @ Telemetry port
  port Tlm(
            $id: FwChanIdType @< Telemetry Channel ID
            ref timeTag: Fw.Time @< Time Tag
            ref val: TlmBuffer @< Buffer containing serialized telemetry value
          )

}
module Fw {

  @ Telemetry port
  port TlmGet(
               $id: FwChanIdType @< Telemetry Channel ID
               ref timeTag: Fw.Time @< Time Tag
               ref val: Fw.TlmBuffer @< Buffer containing serialized telemetry value
             )

}
