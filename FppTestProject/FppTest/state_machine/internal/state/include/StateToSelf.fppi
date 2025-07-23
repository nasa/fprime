@ A state machine for testing state-to-self transitions
@ with hierarchy
state machine StateToSelf {

  @ Exit S1
  action exitS1

  @ Exit S2
  action exitS2

  @ Exit S3
  action exitS3

  @ Action a
  action a

  @ Enter S1
  action enterS1

  @ Enter S2
  action enterS2

  @ Enter S3
  action enterS3

  @ Signal for going from S1 to S1
  signal S1_to_S1

  @ Signal for going from S2 to S3
  signal S2_to_S3

  @ Initial transition
  initial enter S1

  @ State S1
  state S1 {

    @ Initial transition
    initial enter S2

    @ Enter S1
    entry do { enterS1 }

    @ Exit S1
    exit do { exitS1 }

    @ State S2
    state S2 {

      @ Enter S2
      entry do { enterS2 }

      @ Exit S2
      exit do { exitS2 }

      @ State transition to S3
      on S2_to_S3 enter S3

    }

    @ State S3
    state S3 {

      @ Enter S3
      entry do { enterS3 }

      @ Exit S3
      exit do { exitS3 }

    }

    @ State transition to S1
    on S1_to_S1 do { a } enter S1

  }

}
