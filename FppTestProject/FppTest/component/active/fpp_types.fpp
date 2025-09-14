enum FormalParamEnum { X, Y, Z }

array FormalParamArray = [3] U32

array FormalStringArray = [3] string size 3

type FormalAliasString = string size 32

struct FormalParamStruct { x: U32, y: string, z: FormalAliasString }

array FormalAliasStringArray = [3] FormalAliasString
type FormalAliasEnum = FormalParamEnum
type FormalAliasArray = FormalAliasStringArray
