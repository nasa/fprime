module Svc {

  @ A component for dispatching commands
  passive component PolyDb {

    @ Port to get values
    guarded input port getValue: Svc.Poly

    @ Port to set values
    guarded input port setValue: Svc.Poly

  }

}
