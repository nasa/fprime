@ A hierarchical state machine with behavioral polymorphism
state machine Polymorphism {

  @ Signal for polymorphic transition
  signal poly

  @ Signal for transition from S2 to S3
  signal S2_to_S3

  @ Initial transition
  initial enter S1

  @ State S1
  state S1 {

    @ Initial transition
    initial enter S2

    @ Polymorphic state transition
    on poly enter S4

    @ State S2
    state S2 {

      on S2_to_S3 enter S3

    }

    @ State S3
    state S3 {

      @ Polymorphic state transition
      @ This transition overrides the transition in S1
      on poly enter S5

    }

  }

  @ State S4
  state S4

  @ State S5
  state S5

}
