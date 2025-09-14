@ A basic state machine with a TestStruct guard
state machine BasicGuardTestStruct { 

  @ Action a
  action a: SmHarness.TestStruct

  @ Guard g
  guard g: SmHarness.TestStruct

  @ Signal s
  signal s: SmHarness.TestStruct

  initial enter S

  @ State S
  state S {

    @ State transition
    on s if g do { a } enter T

  }

  @ State T
  state T

}
