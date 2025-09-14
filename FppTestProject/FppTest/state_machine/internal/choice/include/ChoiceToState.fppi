@ A state machine for testing choice-to-state transitions
@ with hierarchy
state machine ChoiceToState {

  @ Exit S1
  action exitS1

  @ Action a
  action a

  @ Enter S2
  action enterS2

  @ Enter S3
  action enterS3

  @ Signal s
  signal s

  @ Guard g
  guard g

  @ Initial transition
  initial enter S1

  @ State S1
  state S1 {

    @ Exit S1
    exit do { exitS1 }

    @ Choice C
    choice C {
      if g do { a } enter S2 else do { a } enter S2.S3
    }

    @ State transition to C
    on s enter C

  }

  @ State S2
  state S2 {

    @ Enter S2
    entry do { enterS2 }

    @ Initial transition
    initial do { a } enter S3

    @ State S3
    state S3 {

      @ Enter S3
      entry do { enterS3 }

    }

  }

}
