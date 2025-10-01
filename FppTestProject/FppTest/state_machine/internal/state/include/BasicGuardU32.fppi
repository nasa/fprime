@ A basic state machine with a U32 guard
state machine BasicGuardU32 { 

  @ Action a
  action a: U32

  @ Guard g
  guard g: U32

  @ Signal s
  signal s: U32

  initial enter S

  @ State S
  state S {

    @ State transition
    on s if g do { a } enter T

  }

  @ State T
  state T

}
