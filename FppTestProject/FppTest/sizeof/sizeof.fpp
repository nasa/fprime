constant SizeofU8 = sizeof(U8)
constant SizeofU16 = sizeof(U16)
constant SizeofU32 = sizeof(U32)
constant SizeofU64 = sizeof(U64)
constant SizeofI8 = sizeof(I8)
constant SizeofI16 = sizeof(I16)
constant SizeofI32 = sizeof(I32)
constant SizeofI64 = sizeof(I64)
constant SizeofF32 = sizeof(F32)
constant SizeofF64 = sizeof(F64)
constant SizeofBool = sizeof(bool)

type StringDefaultAlias = string
constant SizeofStringDefaultAlias = sizeof(StringDefaultAlias)
type StringAlias = string size 100
constant SizeofStringAlias = sizeof(StringAlias)

type U64Alias = U64
constant SizeofU64Alias = sizeof(U64Alias)
type EnumAlias = TestEnum1
constant SizeofEnumAlias = sizeof(EnumAlias)

array TestArray1 = [3] I64
constant SizeofArray = sizeof(TestArray1)

array TestArray2 = [2] TestArray1
constant SizeofArray2 = sizeof(TestArray2)

enum TestEnum1: U16 {
    X,
    Y,
    Z
}
constant SizeofEnum = sizeof(TestEnum1)

struct TestStruct1 {
    m1: [2] TestArray1,
    m2: F64
    m3: [1] EnumAlias
    m4: [3] string size 10
}
constant SizeofStruct = sizeof(TestStruct1)

struct TestStruct2 {
    m1: TestArray2
    m2: StringAlias
    m3: [2] TestStruct1
}
constant SizeofStruct2 = sizeof(TestStruct2)
