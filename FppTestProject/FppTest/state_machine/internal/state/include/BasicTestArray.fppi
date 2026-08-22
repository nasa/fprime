@ A basic state machine with TestArray actions
state machine BasicTestArray {

  @ Action a
  action a

  @ Action b
  action b: SmHarness.TestArray

  @ Signal s
  signal s: SmHarness.TestArray

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
