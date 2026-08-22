@ A basic state machine with TestAbsType actions
state machine BasicTestAbsType {

  @ Action a
  action a

  @ Action b
  action b: SmHarness.TestAbsType

  @ Signal s
  signal s: SmHarness.TestAbsType

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
