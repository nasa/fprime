constant basicStringSize = 80

@ A basic state machine with string actions
state machine BasicString {

  @ Action a
  action a

  @ Action b
  action b: string

  @ Signal s
  signal s: string

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
