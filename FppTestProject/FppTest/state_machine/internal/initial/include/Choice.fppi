@ A state machine with an initial choice
state machine Choice {

  @ Action a
  action a

  @ Guard g
  guard g

  initial do { a } enter C

  @ Choice C
  choice C { if g do { a } enter S else do { a, a } enter T }

  @ State S
  state S {
    entry do { a }
  }

  @ State T
  state T {
    entry do { a, a }
  }

}
