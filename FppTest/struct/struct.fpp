enum StructEnum { A, B, C }
array StructArray = [3] U32

struct Primitive {
  mBool: bool
  mU32: U32
  mI16: I16
  mF64: F64
}

struct NonPrimitive {
  mString: string
  mEnum: StructEnum
  mArray: StructArray
  mStruct: Primitive
  mU32Arr: [3] U32
  mStructArr: [3] Primitive
} default {
  mEnum = StructEnum.C
  mStruct = { mBool = true, mF64 = 3.14 }
  mStructArr = { mBool = true, mF64 = 1.16 }
}
