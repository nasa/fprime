    @ A basic state machine with a TestEnum guard
state machine BasicGuardTestEnum { 

  @ Action a
  action a: SmHarness.TestEnum

  @ Guard g
  guard g: SmHarness.TestEnum

  @ Signal s
  signal s: SmHarness.TestEnum

  initial enter S

  @ State S
  state S {

    @ State transition
    on s if g do { a } enter T

  }

  @ State T
  state T

}
