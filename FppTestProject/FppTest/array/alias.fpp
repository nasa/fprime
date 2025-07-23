type EA = E

array ArrayOfAlias = [3] EA default [ E.A, E.B, E.C ]

type AString = string size 32

array AliasString = [3] AString

type AliasOfArray = ArrayOfAlias
