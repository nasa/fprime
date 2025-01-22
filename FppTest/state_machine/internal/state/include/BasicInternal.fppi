@ A basic state machine with an internal transition
state machine BasicInternal {

  @ Action a
  action a

  @ Signal s
  signal s

  @ Initial transition
  initial enter S

  @ State S
  state S {

    @ Enter S
    entry do { a }

    @ Internal transition
    on s do { a }

  }

}
