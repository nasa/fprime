module FileHandling {

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance fileUplink: Svc.FileUplink base id FileHandlingConfig.BASE_ID + 0x00000 \
        queue size FileHandlingConfig.QueueSizes.fileUplink \
        stack size FileHandlingConfig.StackSizes.fileUplink \
        priority FileHandlingConfig.Priorities.fileUplink \
        cpu FileHandlingConfig.CpuAffinities.fileUplink \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        // Confine ground-uplinked file writes to the sandbox directory (see
        // FileHandlingConfig::FileSystem). "." = deployment working-directory subtree.
        FileHandling::fileUplink.configure(".");
        """
    }

    instance fileDownlink: Svc.FileDownlink base id FileHandlingConfig.BASE_ID + 0x01000 \
        queue size FileHandlingConfig.QueueSizes.fileDownlink \
        stack size FileHandlingConfig.StackSizes.fileDownlink \
        priority FileHandlingConfig.Priorities.fileDownlink \
        cpu FileHandlingConfig.CpuAffinities.fileDownlink \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        FileHandling::fileDownlink.configure(
            FileHandlingConfig::DownlinkConfig::cooldown,
            FileHandlingConfig::DownlinkConfig::cycleTime,
            FileHandlingConfig::DownlinkConfig::fileQueueDepth
        );
        // Confine ground-commanded file reads (SendFile / SendPartial) to the sandbox
        // directory (see FileHandlingConfig::FileSystem).
        FileHandling::fileDownlink.configure(".");
        """
    }

    instance fileManager: Svc.FileManager base id FileHandlingConfig.BASE_ID + 0x02000 \
        queue size FileHandlingConfig.QueueSizes.fileManager \
        stack size FileHandlingConfig.StackSizes.fileManager \
        priority FileHandlingConfig.Priorities.fileManager \
        cpu FileHandlingConfig.CpuAffinities.fileManager

    instance prmDb: Svc.PrmDb base id FileHandlingConfig.BASE_ID + 0x03000 \
        queue size FileHandlingConfig.QueueSizes.prmDb \
        stack size FileHandlingConfig.StackSizes.prmDb \
        priority FileHandlingConfig.Priorities.prmDb \
        cpu FileHandlingConfig.CpuAffinities.prmDb \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        // Confine ground-commanded PRM_LOAD_FILE reads to the sandbox directory (see
        // FileHandlingConfig::FileSystem). Runs before readParameters; only PRM_LOAD_FILE
        // (DB_STAGING) is sandboxed, so the boot-time readParamFile() of the configured
        // store file is unaffected.
        FileHandling::prmDb.configureLoadSandbox(".");
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

        # ----------------------------------------------------------------------
        # Topology ports
        # ----------------------------------------------------------------------

        @ Output port for sending file buffers to a downlink component
        port fileDownlinkBufferSendOut = fileDownlink.bufferSendOut

        @ Input port for returning ownership of buffers sent on fileDownlinkBufferSendOut
        port fileDownlinkBufferReturn  = fileDownlink.bufferReturn

        @ Mutex-locked input port for requesting a file downlink
        port fileDownlinkSendFile      = fileDownlink.SendFile

        @ Output port for notifying that a file downlink has completed
        port fileDownlinkFileComplete  = fileDownlink.FileComplete

        @ Input port for scheduling fileDownlink
        port fileDownlinkRun           = fileDownlink.Run

        @ Input port for receiving uplinked file packets
        port fileUplinkBufferSendIn  = fileUplink.bufferSendIn

        @ Output port for returning ownership of received uplink buffers
        port fileUplinkBufferSendOut = fileUplink.bufferSendOut

        @ Input port for scheduling fileManager operations
        port fileManagerSchedIn = fileManager.schedIn

    } # end topology
} # end FileHandling Subtopology
