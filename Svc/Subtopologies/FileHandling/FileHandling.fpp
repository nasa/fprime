module FileHandling {

    enum Ports_ComBufferQueue {
        FILE_DOWNLINK
    }

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance fileUplink: Svc.FileUplink base id FileHandlingConfig.BASE_ID + 0x0100 \
        queue size FileHandlingConfig.QueueSizes.fileUplink \
        stack size FileHandlingConfig.StackSizes.fileUplink \
        priority FileHandlingConfig.Priorities.fileUplink 

    instance fileDownlink: Svc.FileDownlink base id FileHandlingConfig.BASE_ID + 0x0200 \
        queue size FileHandlingConfig.QueueSizes.fileDownlink \
        stack size FileHandlingConfig.StackSizes.fileDownlink \
        priority FileHandlingConfig.Priorities.fileDownlink \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        FileHandling::fileDownlink.configure(
            FileHandlingConfig::DownlinkConfig::timeout,
            FileHandlingConfig::DownlinkConfig::cooldown,
            FileHandlingConfig::DownlinkConfig::cycleTime,
            FileHandlingConfig::DownlinkConfig::fileQueueDepth
        );
        """
    }

    instance fileManager: Svc.FileManager base id FileHandlingConfig.BASE_ID + 0x0300 \
        queue size FileHandlingConfig.QueueSizes.fileManager \
        stack size FileHandlingConfig.StackSizes.fileManager \
        priority FileHandlingConfig.Priorities.fileManager

    instance prmDb: Svc.PrmDb base id FileHandlingConfig.BASE_ID + 0x0400 \
        queue size FileHandlingConfig.QueueSizes.prmDb \
        stack size FileHandlingConfig.StackSizes.prmDb \
        priority FileHandlingConfig.Priorities.prmDb \
    {
        phase Fpp.ToCpp.Phases.configComponents """
            FileHandling::prmDb.configure("PrmDb.dat");
        """
        phase Fpp.ToCpp.Phases.readParameters """
            FileHandling::prmDb.readParamFile();
        """
    }

    topology Subtopology {
        #Active Components
        instance fileUplink
        instance fileDownlink
        instance fileManager
        instance prmDb

    } # end topology
} # end FileHandling Subtopology
