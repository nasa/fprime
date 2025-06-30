module CdhCore {
    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance cmdDisp: Svc.CommandDispatcher base id CdhCoreConfig.BASE_ID + 0x0100 \
        queue size CdhCoreConfig.QueueSizes.cmdDisp \
        stack size CdhCoreConfig.StackSizes.cmdDisp \
        priority CdhCoreConfig.Priorities.cmdDisp

    instance events: Svc.ActiveLogger base id CdhCoreConfig.BASE_ID + 0x0200 \
        queue size CdhCoreConfig.QueueSizes.events \
        stack size CdhCoreConfig.StackSizes.events \
        priority CdhCoreConfig.Priorities.events

    # ----------------------------------------------------------------------
    # Queued Components
    # ----------------------------------------------------------------------
    instance $health: Svc.Health base id CdhCoreConfig.BASE_ID + 0x0300 \
        queue size CdhCoreConfig.QueueSizes.$health \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
            HEALTH_WATCHDOG_CODE = 0x123
        };
        """
        phase Fpp.ToCpp.Phases.configComponents """
        // Health is supplied a set of ping entires.
        CdhCore::health.setPingEntries(
            ConfigObjects::CdhCore_health::pingEntries,
            FW_NUM_ARRAY_ELEMENTS(ConfigObjects::CdhCore_health::pingEntries),
            ConfigConstants::CdhCore_health::HEALTH_WATCHDOG_CODE
        );
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance version: Svc.Version base id CdhCoreConfig.BASE_ID + 0x0400 \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        // Startup TLM and Config verbosity for Versions
        CdhCore::version.config(true);
        """
    }

    instance textLogger: Svc.PassiveTextLogger base id CdhCoreConfig.BASE_ID + 0x0500

    instance fatalAdapter: Svc.AssertFatalAdapter base id CdhCoreConfig.BASE_ID + 0x0600

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
        instance fatalHandler

        connections FaultProtection {
            events.FatalAnnounce -> fatalHandler.FatalReceive
        }
        
    } # end topology
} # end CdhCore Subtopology
