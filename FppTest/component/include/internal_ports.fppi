# ----------------------------------------------------------------------
# Internal ports
# ----------------------------------------------------------------------

@ An internal port with primitive params and priority
internal port internalPrimitive(
  u32: U32, @< A U32
  f32: F32, @< An F32
  b: bool @< A boolean
) priority 5

@ An internal port with string params
internal port internalString(
  str1: string, @< A string
  str2: string size 100 @< Another string
)

@ An internal port with enum params
internal port internalEnum(
  e: E @< An enum
)

@ An internal port with array params
internal port internalArray(
  a: A @< An array
)

@ An internal port with struct params with priority and queue full behavior
internal port internalStruct(
  s: S @< A struct
) priority 20 drop

@ An internal port with priority and queue full behavior
internal port internalPriorityDrop priority 10 drop
