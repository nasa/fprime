# Variant of topology.fpp selected by the build when FPRIME_ENABLE_TEXT_LOGGERS
# is off: CdhCore provides no textLogger instance in that configuration.
module Ref {

  deployment topology Ref {
    include "RefTopologyMembers.fppi"
  }

}
