    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance cmdDisp: Svc.CommandDispatcher base id CdhCoreConfig.BASE_ID + 0x00000 \
        queue size CdhCoreConfig.QueueSizes.cmdDisp \
        stack size CdhCoreConfig.StackSizes.cmdDisp \
        priority CdhCoreConfig.Priorities.cmdDisp \
        cpu CdhCoreConfig.CpuAffinities.cmdDisp

    instance events: Svc.EventManager base id CdhCoreConfig.BASE_ID + 0x001000 \
        queue size CdhCoreConfig.QueueSizes.events \
        stack size CdhCoreConfig.StackSizes.events \
        priority CdhCoreConfig.Priorities.events \
        cpu CdhCoreConfig.CpuAffinities.events

    # ----------------------------------------------------------------------
    # Queued Components
    # ----------------------------------------------------------------------
    instance $health: Svc.Health base id CdhCoreConfig.BASE_ID + 0x002000 \
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
    instance version: Svc.Version base id CdhCoreConfig.BASE_ID + 0x003000 \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        // Startup TLM and Config verbosity for Versions
        CdhCore::version.config(true);
        """
        phase Fpp.ToCpp.Phases.startTasks """
        // Startup TLM and EVENTS
        CdhCore::version.start();
        """
    }

    instance fatalAdapter: Svc.AssertFatalAdapter base id CdhCoreConfig.BASE_ID + 0x005000
