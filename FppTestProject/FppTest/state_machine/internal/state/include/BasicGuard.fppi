@ A basic state machine with a guard
state machine BasicGuard { 

  @ Action a
  action a

  @ Guard g
  guard g

  @ Signal s
  signal s

  initial enter S

  @ State S
  state S {

    @ Exit
    exit do { a }

    @ State transition
    on s if g do { a, a } enter T

  }

  @ State T
  state T {

    @ Entry
    entry do { a, a, a }

  }

}
