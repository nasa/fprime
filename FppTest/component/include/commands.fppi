# ----------------------------------------------------------------------
# Commands
# ----------------------------------------------------------------------

@ A sync command with no params
sync command CMD_SYNC

@ An async command with primitive params
sync command CMD_SYNC_PRIMITIVE(
  u32: U32, @< A U32
  f32: F32, @< An F32
  b: bool @< A boolean
) opcode 0x10

@ A sync command with string params
sync command CMD_SYNC_STRING(
  str1: string, @< A string
  str2: string size 100 @< Another string
)

@ A sync command with enum params
sync command CMD_SYNC_ENUM(
  e: E @< An enum
)

@ A sync command with array params
sync command CMD_SYNC_ARRAY(
  a: A @< An array
)

@ A sync command with struct params
sync command CMD_SYNC_STRUCT(
  s: S @< A struct
)

@ A guarded command with no params
guarded command CMD_GUARDED

@ An aguarded command with primitive params
guarded command CMD_GUARDED_PRIMITIVE(
  u32: U32, @< A U32
  f32: F32, @< An F32
  b: bool @< A boolean
)

@ A guarded command with string params
guarded command CMD_GUARDED_STRING(
  str1: string, @< A string
  str2: string size 100 @< Another string
)

@ A guarded command with enum params
guarded command CMD_GUARDED_ENUM(
  e: E @< An enum
)

@ A guarded command with array params
guarded command CMD_GUARDED_ARRAY(
  a: A @< An array
)

@ A guarded command with struct params
guarded command CMD_GUARDED_STRUCT(
  s: S @< A struct
)
