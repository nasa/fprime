@ A basic state machine with U32 actions
state machine BasicU32 {

  @ Action a
  action a

  @ Action b
  action b: U32

  @ Signal s
  signal s: U32

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
