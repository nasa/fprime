@ A basic state machine with a self transition
state machine BasicSelf {

  @ Action a
  action a

  @ Signal s
  signal s

  initial enter S

  @ State S
  state S {

    @ Entry
    entry do { a }

    @ Exit
    exit do { a, a }

    @ State transition
    on s do { a, a, a } enter S

  }

}
