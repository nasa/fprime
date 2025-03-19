@ A state machine for testing state-to-child transitions
state machine StateToChild {

  @ Exit S2
  action exitS2

  @ Exit S3
  action exitS3

  @ Action a
  action a

  @ Enter S2
  action enterS2

  @ Enter S3
  action enterS3

  @ Signal for going from S1 to S2
  signal S1_to_S2

  @ Signal for going from S2 to S3
  signal S2_to_S3

  @ Initial transition
  initial enter S1

  @ State S1
  state S1 {

    @ Initial transition
    initial enter S2

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

    @ State transition to S2
    on S1_to_S2 do { a } enter S2

  }

}
