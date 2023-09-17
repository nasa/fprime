internal port internalNoArgs priority 10 drop

internal port internalPrimitive(
  u32_1: U32, @< A U32
  u32_2: U32, @< A U32
  f32_1: F32, @< An F32
  f32_2: F32, @< An F32
  b1: bool @< A boolean
  b2: bool @< A boolean
) priority 5

internal port internalString(
  str1: string, @< A string
  str2: string size 100 @< Another string
)

internal port internalEnum(
  en: FormalParamEnum @< An enum
)

internal port internalArray(
  arr: FormalParamArray @< An array
)

internal port internalStruct(
  str: FormalParamStruct @< A struct
) priority 20 drop
