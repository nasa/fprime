@ Instance initializers
module Ref {

  # ----------------------------------------------------------------------
  # blockDrv
  # ----------------------------------------------------------------------

  init blockDrv phase Fpp.ToCpp.Phases.instances """
  // Declared in RefTopologyDefs.cpp
  """

  # ----------------------------------------------------------------------
  # chanTlm 
  # ----------------------------------------------------------------------

  init chanTlm phase Fpp.ToCpp.Phases.instances """
  Svc::TlmChanImpl chanTlm(FW_OPTIONAL_NAME("chanTlm"));
  """

  # ----------------------------------------------------------------------
  # cmdSeq
  # ----------------------------------------------------------------------

  init cmdSeq phase Fpp.ToCpp.Phases.configConstants """
  enum {
    BUFFER_SIZE = 5*1024
  };
  """

  # ----------------------------------------------------------------------
  # downlink
  # ----------------------------------------------------------------------

  init downlink phase Fpp.ToCpp.Phases.configObjects """
  Svc::FprimeFraming framing;
  """

}
