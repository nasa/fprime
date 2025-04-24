enum StructEnum { A, B, C }
array StructArray = [3] U32
type StructArrAlias = StructArray

struct Primitive {
  mBool: bool
  mU32: U32
  mI16: I16
  mF64: F64
}

type StructSAlias = Primitive

struct NonPrimitive {
  mString: string
  mEnum: StructEnum
  mArray: StructArray
  mAliasStructAlias: StructArrAlias
  mStruct: Primitive
  mAliasStruct: StructSAlias
  mU32Arr: [3] U32
  mStructArr: [3] Primitive
} default {
  mEnum = StructEnum.C
  mStruct = { mBool = true, mF64 = 3.14 }
  mStructArr = { mBool = true, mF64 = 1.16 }
}

type StructAliasString = string size 30

struct MultiString {
  mStr_1: string
  mStr_2: string
  mStr50_1: string size 50
  mStr50_2: string size 50
  mStrArr_1: [3] string size 60
  mStrArr_2: [3] string size 60
  mStrAlias: StructAliasString
  mStrAlias_2: [3] StructAliasString
}
