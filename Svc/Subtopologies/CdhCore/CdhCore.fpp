module CdhCore {
    include "CdhCoreInstances.fppi"

    instance textLogger: Svc.PassiveTextLogger base id CdhCoreConfig.BASE_ID + 0x004000

    topology Subtopology {
        include "SubtopologyMembers.fppi"

        instance textLogger
    } # end topology
} # end CdhCore Subtopology
