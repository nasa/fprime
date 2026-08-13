enum FormalParamEnum { X, Y, Z }

array FormalParamArray = [3] U32

array FormalStringArray = [3] string size 3
array FormalStringZeroArray = [3] string size 0

type FormalAliasString = string size 32
type FormalAliasStringZero = string size 0

struct FormalParamStruct { x: U32, y: string, z: FormalAliasString, z0: FormalAliasStringZero }

array FormalAliasStringArray = [3] FormalAliasString
array FormalAliasStringZeroArray = [3] FormalAliasStringZero
type FormalAliasEnum = FormalParamEnum
type FormalAliasArray = FormalAliasStringArray
