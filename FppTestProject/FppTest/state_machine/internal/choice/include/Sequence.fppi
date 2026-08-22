@ A basic state machine with a choice sequence
state machine Sequence {

  @ Action a
  action a

  @ Action b
  action b

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

    @ State transition
    on s enter C1

  }

  @ Choice C1
  choice C1 {
    if g1 enter S2 else enter C2
  }

  @ Choice C2
  choice C2 {
    if g2 do { a } enter S3 else do { b } enter S4
  }

  @ State S2
  state S2

  @ State S3
  state S3

  @ State S4
  state S4

}
