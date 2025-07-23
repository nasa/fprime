event EventNoArgs \
  severity activity high \
  format "Event Activity High occurred"

event EventPrimitive(
  u32_1: U32, @< A U32
  u32_2: U32, @< A U32
  f32_1: F32, @< An F32
  f32_2: F32, @< An F32
  b1: bool @< A boolean
  b2: bool @< A boolean
) \
  severity activity low \
  id 0x10 \
  format "Event Activity Low occurred with arguments: {}, {}, {}, {}, {}, {}" \
  throttle 5

event EventString (
  str1: string, @< A string
  str2: string size 100 @< Another string
) \
  severity command \
  format "Event Command occurred with arguments: {}, {}"

event EventEnum(
  en: FormalParamEnum @< An enum
) \
  severity diagnostic \
  format "Event Diagnostic occurred with argument: {}"

event EventArray (
  arr: FormalParamArray @< An array
) \
  severity fatal \
  format "Event Fatal occurred with argument: {}" \
  throttle 10

event EventStruct (
  str: FormalParamStruct @< A struct
) \
  severity warning high \
  id 0x20 \
  format "Event Warning High occurred with argument: {}"

event EventBool (
  b: bool @< A boolean
) \
  severity warning low \
  format "Event Warning Low occurred {}" \
  throttle 10
