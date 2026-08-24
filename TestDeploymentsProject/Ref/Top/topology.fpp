module Ref {

  deployment topology Ref {
    include "RefTopologyMembers.fppi"

    # Kept out of RefTopologyMembers.fppi: CdhCore.textLogger only exists when
    # FPRIME_ENABLE_TEXT_LOGGERS is on. topology-no-text-logger.fpp is the
    # variant without this pattern.
    text event connections instance CdhCore.textLogger
  }

}
