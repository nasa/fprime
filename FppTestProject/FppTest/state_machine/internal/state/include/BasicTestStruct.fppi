@ A basic state machine with TestStruct actions
state machine BasicTestStruct {

  @ Action a
  action a

  @ Action b
  action b: SmHarness.TestStruct

  @ Signal s
  signal s: SmHarness.TestStruct

  initial enter S

  @ State S
  state S {

    @ Exit
    exit do { a }

    @ State transition
    on s do { a, b } enter T

  }

  @ State T
  state T {

    @ Entry
    entry do { a, a, a }

  }

}
