@ A hierarchical state machine with an internal transition
state machine Internal {

  @ Action a
  action a

  @ Signal for internal transition in S1
  signal S1_internal

  @ Signal for transition from S2 to S3
  signal S2_to_S3

  @ Initial transition
  initial enter S1

  @ State S1
  state S1 {

    @ Initial transition
    initial enter S2

    @ State S2
    state S2 {

      @ Transition from S2 to S3
      on S2_to_S3 enter S3

    }

    @ State S3
    state S3

    @ Internal transition
    on S1_internal do { a }

  }

}
