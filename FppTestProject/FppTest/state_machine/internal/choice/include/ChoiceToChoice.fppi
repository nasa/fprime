@ A state machine for testing choice-to-choice transitions
@ with hierarchy
state machine ChoiceToChoice {

  @ Exit S1
  action exitS1

  @ Action a
  action a

  @ Enter S2
  action enterS2

  @ Signal s
  signal s

  @ Guard g1
  guard g1

  @ Guard g2
  guard g2

  @ Initial transition
  initial enter S1

  @ State S1
  state S1 {

    @ Exit S1
    exit do { exitS1 }

    @ Choice C1
    choice C1 {
      if g1 do { a } enter S2 else do { a } enter S2.C2
    }

    @ State transition to C1
    on s enter C1

  }

  @ State S2
  state S2 {

    @ Enter S2
    entry do { enterS2 }

    @ Initial transition
    initial enter S3

    @ Choice C2
    choice C2 {
      if g2 enter S3 else enter S4
    }

    @ State S3
    state S3

    @ State S4
    state S4

  }

}
