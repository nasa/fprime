@ A basic state machine with TestEnum actions
state machine BasicTestEnum {

  @ Action a
  action a

  @ Action b
  action b: SmHarness.TestEnum

  @ Signal s
  signal s: SmHarness.TestEnum

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
