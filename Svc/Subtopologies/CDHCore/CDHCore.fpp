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

    # ----------------------------------------------------------------------
    # Queued Components
    # ----------------------------------------------------------------------
    instance $health: Svc.Health base id CDHCoreConfig.BASE_ID + 0x0300 \
        queue size CDHCoreConfig.QueueSizes.$health \
    {
        phase Fpp.ToCpp.Phases.configConstants """
        enum {
            HEALTH_WATCHDOG_CODE = 0x123
        };
        """
        phase Fpp.ToCpp.Phases.configComponents """
        // Health is supplied a set of ping entires.
        CDHCore::health.setPingEntries(
            ConfigObjects::CDHCore_health::pingEntries,
            FW_NUM_ARRAY_ELEMENTS(ConfigObjects::CDHCore_health::pingEntries),
            ConfigConstants::CDHCore_health::HEALTH_WATCHDOG_CODE
        );
        """
    }

    # ----------------------------------------------------------------------
    # Passive Components
    # ----------------------------------------------------------------------
    instance version: Svc.Version base id CDHCoreConfig.BASE_ID + 0x0400 \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        // Startup TLM and Config verbosity for Versions
        CDHCore::version.config(true);
        """
    }

    instance textLogger: Svc.PassiveTextLogger base id CDHCoreConfig.BASE_ID + 0x0500

    instance fatalAdapter: Svc.AssertFatalAdapter base id CDHCoreConfig.BASE_ID + 0x0600

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