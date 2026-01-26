module FileHandlingCfdp {

    # ----------------------------------------------------------------------
    # Active Components
    # ----------------------------------------------------------------------
    instance cfdpManager: Svc.Ccsds.CfdpManager base id FileHandlingCfdpConfig.BASE_ID + 0x00000 \
        queue size FileHandlingCfdpConfig.QueueSizes.cfdpManager \
        stack size FileHandlingCfdpConfig.StackSizes.cfdpManager \
        priority FileHandlingCfdpConfig.Priorities.cfdpManager \
    {
        phase Fpp.ToCpp.Phases.configComponents """
        FileHandlingCfdp::cfdpManager.configure();
        """
    }

    instance fileManager: Svc.FileManager base id FileHandlingCfdpConfig.BASE_ID + 0x01000 \
        queue size FileHandlingCfdpConfig.QueueSizes.fileManager \
        stack size FileHandlingCfdpConfig.StackSizes.fileManager \
        priority FileHandlingCfdpConfig.Priorities.fileManager

    instance prmDb: Svc.PrmDb base id FileHandlingCfdpConfig.BASE_ID + 0x02000 \
        queue size FileHandlingCfdpConfig.QueueSizes.prmDb \
        stack size FileHandlingCfdpConfig.StackSizes.prmDb \
        priority FileHandlingCfdpConfig.Priorities.prmDb \
    {
        phase Fpp.ToCpp.Phases.configComponents """
            FileHandlingCfdp::prmDb.configure("PrmDb.dat");
        """
        phase Fpp.ToCpp.Phases.readParameters """
            FileHandlingCfdp::prmDb.readParamFile();
        """
    }

    topology Subtopology {
        # Active Components
        instance cfdpManager
        instance fileManager
        instance prmDb

    } # end topology
} # end FileHandlingCfdp Subtopology
