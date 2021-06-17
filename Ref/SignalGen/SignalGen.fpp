module Ref {

  struct SignalPair {
    $time: F32 format "{f}"
    value: F32 format "{f}"
  }

  enum SignalType {
    TRIANGLE = 0
    SQUARE = 1
    SINE = 2
    NOISE = 3
  }

  array SignalPairSet = [4] Ref.SignalPair

  array SignalSet = [4] F32 format "{f}"

  struct SignalInfo {
    $type: Ref.SignalType
    history: Ref.SignalSet
    pairHistory: Ref.SignalPairSet
  }

}
