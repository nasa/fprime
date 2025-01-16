@ A basic state machine with a TestAbsType guard
state machine BasicGuardTestAbsType { 

  @ Action a
  action a: SmHarness.TestAbsType

  @ Guard g
  guard g: SmHarness.TestAbsType

  @ Signal s
  signal s: SmHarness.TestAbsType

  initial enter S

  @ State S
  state S {

    @ State transition
    on s if g do { a } enter T

  }

  @ State T
  state T

}
