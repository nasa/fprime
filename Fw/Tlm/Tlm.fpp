module Fw {

  type TlmBuffer

  enum TlmValid {
    VALID = 0
    INVALID = 1
  }

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
               @ Size set to 0 if channel not found, or if no value
               @ has been received for this channel yet.
               ref val: Fw.TlmBuffer 
             ) -> Fw.TlmValid

}
