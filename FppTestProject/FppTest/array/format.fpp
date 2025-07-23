array FormatBool = [3] bool format "a {} b"

array FormatU8 = [3] U8 format "a {} b"

array FormatU16Dec = [3] U16 format "a {d} b"

array FormatU32Oct = [3] U32 format "a {o} b"

array FormatU64Hex = [3] U64 format "a {x} b"

array FormatI8 = [3] I8 format "a {} b"

array FormatI16Dec = [3] I16 format "a {d} b"

array FormatI32Oct = [3] I32 format "a {o} b"

array FormatI64Hex = [3] I64 format "a {x} b"

array FormatF32e = [3] F32 format "a {.1e} b"

array FormatF32f = [3] F32 format "a {.2f} b"

array FormatF64g = [3] F64 format "a {.3g} b"

array FormatString = [3] string format "% {}"

array FormatChar = [3] U8 format "a {c} b"
