module Fw {

  type ParamBuffer

  @ Enum representing parameter validity
  enum ParamValid {
    UNINIT = 0
    VALID = 1
    INVALID = 2
    DEFAULT = 3
  }

  @ Port for getting a parameter
  port PrmGet(
               $id: FwPrmIdType @< Parameter ID
               @ Buffer containing serialized parameter value.
               @ Unmodified if param not found.
               ref val: ParamBuffer
             ) -> ParamValid

  @ Port for setting a parameter
  port PrmSet(
               $id: FwPrmIdType @< Parameter ID
               ref val: ParamBuffer @< Buffer containing serialized parameter value
             )

}
