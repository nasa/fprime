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

struct MultiString {
  mStr_1: string
  mStr_2: string
  mStr50_1: string size 50
  mStr50_2: string size 50
  mStrArr_1: [3] string size 60
  mStrArr_2: [3] string size 60
}
