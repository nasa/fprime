@ A basic state machine with a U32 choice sequence
state machine SequenceU32 {

  @ Action a
  action a: U32

  @ Action b
  action b

  @ Signal s
  signal s: U32

  @ Guard g1
  guard g1

  @ Guard g2
  guard g2: U32

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
