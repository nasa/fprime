@ An enum
enum E { X, Y, Z }

@ An array
array A = [3] U32

@ A struct
struct S { x: U32, y: string }

@ A typed port with no arguments
port NoArgs

@ A type port with no arguments and a return type
port NoArgsReturn -> U32

@ A typed port
port Typed(
  u32: U32, @< A U32
  f32: F32, @< An F32
  b: bool, @< A boolean
  str1: string, @< A string
  e: E, @< An enum
  a: A, @< An array
  s: S @< A struct
)

@ A typed port with a return type
port TypedReturn(
  u32: U32, @< A U32
  f32: F32, @< An F32
  b: bool, @< A boolean
  str2: string, @< A string
  e: E, @< An enum
  a: A, @< An array
  s: S @< A struct
) -> F32
