module FppTest {

  module Struct {

    enum StructEnum { A, B, C }
    array StructArray = [3] U32
    type StructArrAlias = StructArray

    include "primitive.fppi"

    type StructSAlias = Primitive

    struct Empty { }

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
    type StructAliasStringZero = string size 1

    struct MultiString {
      mStr_1: string
      mStr_2: string
      mStr50_1: string size 50
      mStr50_2: string size 50
      mStr0: string size 1
      mStrArr_1: [3] string size 60
      mStrArr_2: [3] string size 60
      mStrArr0: [3] string size 1
      mStrAlias: StructAliasString
      mStrAlias_2: [3] StructAliasString
      mStrAlias0: StructAliasStringZero
      mStrAliasArr0: [3] StructAliasStringZero
    }

    passive component C {
      include "primitive.fppi"
    }

    state machine SM {
      state S
      initial enter S
      type SMPrimitive = Primitive
    }

  }

}
