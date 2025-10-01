@ A basic state machine with a TestArray guard
state machine BasicGuardTestArray { 

  @ Action a
  action a: SmHarness.TestArray

  @ Guard g
  guard g: SmHarness.TestArray

  @ Signal s
  signal s: SmHarness.TestArray

  initial enter S

  @ State S
  state S {

    @ State transition
    on s if g do { a } enter T

  }

  @ State T
  state T

}
