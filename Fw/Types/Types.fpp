module Fw {

  @ A string stored in a fixed-size buffer
  type String

  @ A value of polymorphic type
  type PolyType

  @ Serialization status
  enum SerialStatus {
    OK, @< Serialization operation succeeded
    FORMAT_ERROR @< Data was the wrong format (e.g. wrong packet type)
    NO_ROOM_LEFT  @< No room left in the buffer to serialize data
  }

  @ Deserialization status
  enum DeserialStatus {
    OK = 0
    BUFFER_EMPTY = 3 @< Deserialization buffer was empty when trying to read data
    FORMAT_ERROR = 4  @< Deserialization data had incorrect values (unexpected data types)
    SIZE_MISMATCH = 5 @< Data was left in in the buffer, but not enough to deserialize
    TYPE_MISMATCH = 6 @< Deserialized type ID didn't match
  }

  @ Enabled and disabled states
  enum Enabled {
    DISABLED @< Disabled state
    ENABLED @< Enabled state
  }

}
