@ A port with no arguments
port NoArgsReturn -> bool

@ A port returning a primitive type
port PrimitiveReturn(
  u32: U32
  ref u32Ref: U32
  f32: F32
  ref f32Ref: F32
  b: bool
  ref bRef: bool
) -> U32

# Commented out because of bug in Python component autocoder
# Will be tested with the FPP component autocoder
# @ A port returning a string type
# port StringReturn(
#   str80: string @< A string of size 80
#   ref str80Ref: string
#   str100: string size 100 @< A string of size 100
#   ref str100Ref: string size 100
# ) -> string

@ A port returning an enum type
port EnumReturn(
  en: PortEnum @< An enum
  ref enRef: PortEnum @< An enum ref
) -> PortEnum

@ A port returning an array type
port ArrayReturn(
  a: PortArray @< An array
  ref aRef: PortArray @< An array ref
) -> PortArray

@ A port returning a struct type
port StructReturn(
  s: PortStruct @< A struct
  ref sRef: PortStruct @< A struct ref
) -> PortStruct
