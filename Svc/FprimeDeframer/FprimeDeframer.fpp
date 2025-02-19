module Svc {

  @ A component for deframing input received from the ground
  @ via a FrameAccumulator
  passive component FprimeDeframer {

    # ----------------------------------------------------------------------
    # Deframer interface
    # ----------------------------------------------------------------------

    include "../Interfaces/DeframerInterface.fppi"

  }

}
