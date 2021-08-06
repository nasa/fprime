module Svc {

  @ A polymorphic database component
  passive component PolyDb {

    @ Mutexed Port to get values
    guarded input port getValue: Svc.Poly

    @ Mutexed Port to set values
    guarded input port setValue: Svc.Poly

  }

}
