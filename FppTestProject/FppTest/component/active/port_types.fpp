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
  en: FormalParamEnum @< An enum
  ref enRef: FormalParamEnum @< An enum ref
  enA: FormalAliasEnum @< An enum alias
  ref enARef: FormalAliasEnum @< An enum alias ref
)

@ A port with array arguments
port ArrayArgs(
  a: FormalParamArray @< An array
  ref aRef: FormalParamArray @< An array ref
  b: FormalAliasArray @< alias of an array
  ref bRef: FormalAliasArray @< alias of array ref
  c: FormalAliasStringArray @< array of string aliases
  ref cRef: FormalAliasStringArray @< array of string aliases ref
)

@ A port with struct arguments
port StructArgs(
  s: FormalParamStruct @< A struct
  ref sRef: FormalParamStruct @< A struct ref
)

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

@ A port returning a string type
port StringReturn(
  str: string @< A string
  ref strRef: string @< A string ref
) -> string

@ A port returning a string type
port StringAliasReturn(
  str: string @< A string
  ref strRef: string @< A string ref
) -> FormalAliasString

@ A port returning an enum type
port EnumReturn(
  en: FormalParamEnum @< An enum
  ref enRef: FormalParamEnum @< An enum ref
) -> FormalParamEnum

@ A port returning an array type
port ArrayReturn(
  a: FormalParamArray @< An array
  ref aRef: FormalParamArray @< An array ref
) -> FormalParamArray

@ A port returning an array type of string aliases
port ArrayStringAliasReturn(
  a: FormalParamArray @< An array
  ref aRef: FormalParamArray @< An array ref
) -> FormalAliasStringArray

@ A port returning a struct type
port StructReturn(
  s: FormalParamStruct @< A struct
  ref sRef: FormalParamStruct @< A struct ref
) -> FormalParamStruct
