@ A basic state machine
state machine Basic {

  @ Action a
  action a

  @ Signal s
  signal s

  initial enter S

  @ State S
  state S {

    @ Exit
    exit do { a }

    @ State transition
    on s do { a, a } enter T

  }

  @ State T
  state T {

    @ Entry
    entry do { a, a, a }

  }

}
