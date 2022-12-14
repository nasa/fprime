@ A port with no arguments
port NoArgs

@ A port with primitive arguments
port PrimitiveArgs(
  u32: U32
  ref u32Ref: U32
  f32: F32
  ref f32Ref: F32
  b: bool
  ref bRef: bool
)

@ A port with string arguments
port StringArgs(
  str80: string @< A string of size 80
  ref str80Ref: string
  str100: string size 100 @< A string of size 100
  ref str100Ref: string size 100
)

@ A port with enum arguments
port EnumArgs(
  en: PortEnum @< An enum
  ref enRef: PortEnum @< An enum ref
)

@ A port with array arguments
port ArrayArgs(
  a: PortArray @< An array
  ref aRef: PortArray @< An array ref
)

@ A port with struct arguments
port StructArgs(
  s: PortStruct @< A struct
  ref sRef: PortStruct @< A struct ref
)
