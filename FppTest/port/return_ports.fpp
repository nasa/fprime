@ A port with no arguments
port NoArgsReturn -> bool

@ A port returning a primitive type
port PrimitiveReturn(
  u32: U32,
  ref u32Ref: U32,
  f32: F32,
  ref f32Ref: F32,
  b: bool,
  ref bRef: bool,
) -> U32
