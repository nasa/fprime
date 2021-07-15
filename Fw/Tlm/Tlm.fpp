module Fw {

  type TlmBuffer

  @ Port for sending telemetry
  port Tlm(
            $id: FwChanIdType @< Telemetry Channel ID
            ref timeTag: Fw.Time @< Time Tag
            ref val: TlmBuffer @< Buffer containing serialized telemetry value
          )

  @ Port for getting telemetry
  port TlmGet(
               $id: FwChanIdType @< Telemetry Channel ID
               ref timeTag: Fw.Time @< Time Tag
               ref val: Fw.TlmBuffer @< Buffer containing serialized telemetry value
             )

}
