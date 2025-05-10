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
               @ Buffer containing serialized telemetry value. 
               @ Size set to 0 if channel not found.
               ref val: Fw.TlmBuffer 
             )

}
