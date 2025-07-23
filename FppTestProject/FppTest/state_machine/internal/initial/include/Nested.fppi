@ A state machine with nested initial transitions
state machine Nested {

  @ Action a
  action a

  initial do { a } enter S

  @ State S
  state S {

    entry do { a, a }

    initial enter T

    @ State S.T
    state T {
      entry do { a, a, a }
    }

  }

}
