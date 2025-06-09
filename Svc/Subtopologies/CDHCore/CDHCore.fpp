module CDHCore {
    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance cmdDisp: Svc.CommandDispatcher base id CDHCoreConfig.BASE_ID + 0x0100 \
        queue size CDHCoreConfig.QueueSizes.cmdDisp \
        stack size CDHCoreConfig.StackSizes.cmdDisp \
        priority CDHCoreConfig.Priorities.cmdDisp

    instance events: Svc.ActiveLogger base id CDHCoreConfig.BASE_ID + 0x0200 \
        queue size CDHCoreConfig.QueueSizes.events \
        stack size CDHCoreConfig.StackSizes.events \
        priority CDHCoreConfig.Priorities.events

    instance tlmSend: Svc.TlmChan base id CDHCoreConfig.BASE_ID + 0x0300 \
        queue size CDHCoreConfig.QueueSizes.tlmSend \
        stack size CDHCoreConfig.StackSizes.tlmSend \
        priority CDHCoreConfig.Priorities.tlmSend

    # ----------------------------------------------------------------------
    # Queued Components
    # ----------------------------------------------------------------------
    instance $health: Svc.Health base id CDHCoreConfig.BASE_ID + 0x0400 \
        queue size CDHCoreConfig.QueueSizes.$health \
    
    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance version: Svc.Version base id CDHCoreConfig.BASE_ID + 0x0500 \

    instance textLogger: Svc.PassiveTextLogger base id CDHCoreConfig.BASE_ID + 0x0600 \
  
    instance fatalAdapter: Svc.AssertFatalAdapter base id CDHCoreConfig.BASE_ID + 0x0700 \
    
    topology Subtopology {
        #Active Components
        instance cmdDisp 
        instance events
        instance tlmSend

        #Queued Components
        instance $health

        #Passive Components
        instance version
        instance textLogger
        instance fatalAdapter


    } # end topology
} # end CDHCore Subtopology