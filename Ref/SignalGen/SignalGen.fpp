module Ref {

   enum SignalType {
    TRIANGLE = 0
    SQUARE = 1
    SINE = 2
    NOISE = 3
  }

  array SignalSet = [4] F32 default [0.0, 0.0, 0.0, 0.0] format "{f}"

  struct SignalPair {
    $time: F32 format "{f}"
    value: F32 format "{f}"
  }

}
