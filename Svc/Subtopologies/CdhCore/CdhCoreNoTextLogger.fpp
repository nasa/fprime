# Variant of CdhCore.fpp selected by the build when FPRIME_ENABLE_TEXT_LOGGERS
# is off: Svc.PassiveTextLogger is excluded from the build, so the subtopology
# carries no textLogger instance.
module CdhCore {
    include "CdhCoreInstances.fppi"

    topology Subtopology {
        include "SubtopologyMembers.fppi"
    } # end topology
} # end CdhCore Subtopology
