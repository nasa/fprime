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

  @ FPP from XML: could not translate array value [ Ref::SignalPair(), Ref::SignalPair(), Ref::SignalPair(), Ref::SignalPair() ]
  #how should I change this for the xml conversion since it uses a nonstandard type
  array SignalPairSet = [4] Ref.SignalPair

  array SignalSet = [4] F32 default [0.0, 0.0, 0.0, 0.0] format "{f}"

  


  struct SignalInfo {
    $type: Ref.SignalType
    history: Ref.SignalSet
    pairHistory: Ref.SignalPairSet
  }

}
