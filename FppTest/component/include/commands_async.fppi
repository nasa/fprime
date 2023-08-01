async command CMD_ASYNC_NO_ARGS

async command CMD_ASYNC_PRIMITIVE (
  u32_1: U32, @< A U32
  u32_2: U32, @< A U32
  f32_1: F32, @< An F32
  f32_2: F32, @< An F32
  b1: bool @< A boolean
  b2: bool @< A boolean
)

async command CMD_ASYNC_STRINGS(
  str1: string, @< A string
  str2: string size 100 @< Another string
) opcode 0x20 priority 10

async command CMD_ASYNC_ENUM (
  en: FormalParamEnum @< An enum
) priority 20

async command CMD_ASYNC_ARRAY(
  arr: FormalParamArray @< An array
) drop

async command CMD_ASYNC_STRUCT(
  str: FormalParamStruct @< A struct
) priority 30 drop
