module FppTest {

  module Array {

    type EA = E

    array ArrayOfAlias = [3] EA default [ E.A, E.B, E.C ]

    type AString = string size 32
    type AStringZero = string size 0

    array AliasString = [3] AString
    array AliasStringZero = [3] AStringZero

    type AliasOfArray = ArrayOfAlias

  }

}
