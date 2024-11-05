@ A state machine with an initial choice
state machine Junction {

  @ Action a
  action a

  @ Guard g
  guard g

  initial do { a } enter J

  @ Junction J
  choice J { if g do { a } enter S else do { a, a } enter T }

  @ State S
  state S {
    entry do { a }
  }

  @ State T
  state T {
    entry do { a, a }
  }

}
