module CDHCore {

    instance cmdDisp: Svc.CommandDispatcher base id CDHCoreConfig.CDHCore_BASE_ID + 0x0100 \
        queue size CDHCoreConfig.Defaults.cmdDisp_QUEUE_SIZE \
        stack size CDHCoreConfig.Defaults.cmdDisp_STACK_SIZE \
        priority CDHCoreConfig.Priorities.cmdDisp_PRIORITY

    instance eventLogger: Svc.ActiveLogger base id CDHCoreConfig.CDHCore_BASE_ID + 0x0200 \
        queue size CDHCoreConfig.Defaults.eventLogger_QUEUE_SIZE \
        stack size CDHCoreConfig.Defaults.eventLogger_STACK_SIZE \
        priority CDHCoreConfig.Priorities.eventLogger_PRIORITY

    instance tlmSend: Svc.TlmChan base id CDHCoreConfig.CDHCore_BASE_ID + 0x0300 \
        queue size CDHCoreConfig.Defaults.tlmSend_QUEUE_SIZE \
        stack size CDHCoreConfig.Defaults.tlmSend_STACK_SIZE \
        priority CDHCoreConfig.Priorities.tlmSend_PRIORITY

    instance $health: Svc.Health base id CDHCoreConfig.CDHCore_BASE_ID + 0x0400 \
        queue size 25
    
    topology Subtopology {
        instance cmdDisp 
        instance eventLogger
        instance tlmSend

        instance $health

        command connections instance cmdDisp
        event connections instance eventLogger
        telemetry connections instance tlmSend

    } # end topology
} # end CDHCore Subtopology