sync command CMD_NO_ARGS

sync command CMD_PRIMITIVE(
  u32_1: U32, @< A U32
  u32_2: U32, @< A U32
  f32_1: F32, @< An F32
  f32_2: F32, @< An F32
  b1: bool @< A boolean
  b2: bool @< A boolean
) opcode 0x10

sync command CMD_STRINGS(
  str1: string, @< A string
  str2: string size 100 @< Another string
)

sync command CMD_ENUM(
  en: FormalParamEnum @< An enum
)

guarded command CMD_ARRAY(
  arr: FormalParamArray @< An array
)

guarded command CMD_STRUCT(
  str: FormalParamStruct @< A struct
)
