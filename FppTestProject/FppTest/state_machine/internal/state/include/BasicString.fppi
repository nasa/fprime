@ A basic state machine with string actions
state machine BasicString {

  constant stringSize = 80

  @ Action a
  action a

  @ Action b
  action b: string

  @ Signal s
  signal s: string size 200

  @ Signal s1
  signal s1: string size 100

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
