@ Instance initializers
module Ref {

  # ----------------------------------------------------------------------
  # blockDrv
  # ----------------------------------------------------------------------

  init blockDrv phase Fpp.ToCpp.Phases.instances """
  // Declared in RefTopologyDefs.cpp
  """

  # ----------------------------------------------------------------------
  # cmdSeq
  # ----------------------------------------------------------------------

  init cmdSeq phase Fpp.ToCpp.Phases.configConstants """
  enum {
    BUFFER_SIZE = 5*1024
  };
  """

  init cmdSeq phase Fpp.ToCpp.Phases.configComponents """
  cmdSeq.allocateBuffer(
      0,
      Allocation::mallocator,
      ConfigConstants::cmdSeq::BUFFER_SIZE
  );
  """

  init cmdSeq phase Fpp.ToCpp.Phases.tearDownComponents """
  cmdSeq.deallocateBuffer(Allocation::mallocator);
  """

  # ----------------------------------------------------------------------
  # comm
  # ----------------------------------------------------------------------

  init comm phase Fpp.ToCpp.Phases.instances """
  Drv::TcpClient comm(FW_OPTIONAL_NAME("comm"));
  """

  init comm phase Fpp.ToCpp.Phases.configConstants """
  enum {
    PRIORITY = 100,
    STACK_SIZE = Default::stackSize
  };
  """

  init comm phase Fpp.ToCpp.Phases.startTasks """
  // Initialize socket server if and only if there is a valid specification
  if (state.hostName != NULL && state.portNumber != 0) {
      Fw::EightyCharString name("ReceiveTask");
      // Uplink is configured for receive so a socket task is started
      comm.configure(state.hostName, state.portNumber);
      comm.startSocketTask(
          name,
          ConfigConstants::comm::PRIORITY,
          ConfigConstants::comm::STACK_SIZE
      );
  }
  """

  init comm phase Fpp.ToCpp.Phases.freeThreads """
  comm.stopSocketTask();
  (void) comm.joinSocketTask(NULL);
  """

  # ----------------------------------------------------------------------
  # downlink
  # ----------------------------------------------------------------------

  init downlink phase Fpp.ToCpp.Phases.configObjects """
  Svc::FprimeFraming framing;
  """

  init downlink phase Fpp.ToCpp.Phases.configComponents """
  downlink.setup(ConfigObjects::downlink::framing);
  """

  # ----------------------------------------------------------------------
  # fileDownlink
  # ----------------------------------------------------------------------

  init fileDownlink phase Fpp.ToCpp.Phases.configConstants """
  enum {
    TIMEOUT = 1000,
    COOLDOWN = 1000,
    CYCLE_TIME = 1000,
    FILE_QUEUE_DEPTH = 10
  };
  """

  init fileDownlink phase Fpp.ToCpp.Phases.configComponents """
  fileDownlink.configure(
      ConfigConstants::fileDownlink::TIMEOUT,
      ConfigConstants::fileDownlink::COOLDOWN,
      ConfigConstants::fileDownlink::CYCLE_TIME,
      ConfigConstants::fileDownlink::FILE_QUEUE_DEPTH
  );
  """

  # ----------------------------------------------------------------------
  # fileUplinkBufferManager
  # ----------------------------------------------------------------------

  init fileUplinkBufferManager phase Fpp.ToCpp.Phases.configConstants """
  enum {
    STORE_SIZE = 3000,
    QUEUE_SIZE = 30,
    MGR_ID = 200
  };
  """

  init fileUplinkBufferManager phase Fpp.ToCpp.Phases.configComponents """
  Svc::BufferManager::BufferBins upBuffMgrBins;
  memset(&upBuffMgrBins, 0, sizeof(upBuffMgrBins));
  {
    using namespace ConfigConstants::fileUplinkBufferManager;
    upBuffMgrBins.bins[0].bufferSize = STORE_SIZE;
    upBuffMgrBins.bins[0].numBuffers = QUEUE_SIZE;
    fileUplinkBufferManager.setup(
        MGR_ID,
        0,
        Allocation::mallocator,
        upBuffMgrBins
    );
  }
  """

  init fileUplinkBufferManager phase Fpp.ToCpp.Phases.tearDownComponents """
  fileUplinkBufferManager.cleanup();
  """

  # ----------------------------------------------------------------------
  # health
  # ----------------------------------------------------------------------

  init $health phase Fpp.ToCpp.Phases.configConstants """
  enum {
    WATCHDOG_CODE = 0x123
  };
  """

  init $health phase Fpp.ToCpp.Phases.configComponents """
  health.setPingEntries(
      ConfigObjects::health::pingEntries,
      FW_NUM_ARRAY_ELEMENTS(ConfigObjects::health::pingEntries),
      ConfigConstants::health::WATCHDOG_CODE
  );
  """

  # ----------------------------------------------------------------------
  # linuxTime
  # ----------------------------------------------------------------------

  init linuxTime phase Fpp.ToCpp.Phases.instances """
  Svc::LinuxTime linuxTime(FW_OPTIONAL_NAME("linuxTime"));
  """

  # ----------------------------------------------------------------------
  # prmDb
  # ----------------------------------------------------------------------

  init prmDb phase Fpp.ToCpp.Phases.instances """
  Svc::PrmDb prmDb(FW_OPTIONAL_NAME("prmDb"), "PrmDb.dat");
  """

  init prmDb phase Fpp.ToCpp.Phases.readParameters """
  prmDb.readParamFile();
  """

  # ----------------------------------------------------------------------
  # rateGroup1Comp
  # ----------------------------------------------------------------------

  init rateGroup1Comp phase Fpp.ToCpp.Phases.configObjects """
  NATIVE_UINT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  """

  init rateGroup1Comp phase Fpp.ToCpp.Phases.instances """
  Svc::ActiveRateGroup rateGroup1Comp(
      FW_OPTIONAL_NAME("rateGroup1Comp"),
      ConfigObjects::rateGroup1Comp::context,
      FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup1Comp::context)
  );
  """

  # ----------------------------------------------------------------------
  # rateGroup2Comp
  # ----------------------------------------------------------------------

  init rateGroup2Comp phase Fpp.ToCpp.Phases.configObjects """
  NATIVE_UINT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  """

  init rateGroup2Comp phase Fpp.ToCpp.Phases.instances """
  Svc::ActiveRateGroup rateGroup2Comp(
      FW_OPTIONAL_NAME("rateGroup2Comp"),
      ConfigObjects::rateGroup2Comp::context,
      FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup2Comp::context)
  );
  """

  # ----------------------------------------------------------------------
  # rateGroup3Comp
  # ----------------------------------------------------------------------

  init rateGroup3Comp phase Fpp.ToCpp.Phases.configObjects """
  NATIVE_UINT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  """

  init rateGroup3Comp phase Fpp.ToCpp.Phases.instances """
  Svc::ActiveRateGroup rateGroup3Comp(
      FW_OPTIONAL_NAME("rateGroup3Comp"),
      ConfigObjects::rateGroup3Comp::context,
      FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup3Comp::context)
  );
  """

  # ----------------------------------------------------------------------
  # rateGroupDriverComp
  # ----------------------------------------------------------------------

  init rateGroupDriverComp phase Fpp.ToCpp.Phases.configObjects """
  NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriver::DIVIDER_SIZE] = { 1, 2, 4 };
  """

  init rateGroupDriverComp phase Fpp.ToCpp.Phases.instances """
  Svc::RateGroupDriver rateGroupDriverComp(
      FW_OPTIONAL_NAME("rateGroupDriverComp"),
      ConfigObjects::rateGroupDriverComp::rgDivs,
      FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroupDriverComp::rgDivs)
  );
  """

  # ----------------------------------------------------------------------
  # textLogger
  # ----------------------------------------------------------------------

  init textLogger phase Fpp.ToCpp.Phases.instances """
  Svc::ConsoleTextLoggerImpl textLogger(FW_OPTIONAL_NAME("textLogger"));
  """

  init textLogger phase Fpp.ToCpp.Phases.initComponents """
  textLogger.init();
  """

  # ----------------------------------------------------------------------
  # uplink
  # ----------------------------------------------------------------------

  init uplink phase Fpp.ToCpp.Phases.configObjects """
  Svc::FprimeDeframing deframing;
  """

  init uplink phase Fpp.ToCpp.Phases.instances """
  Svc::DeframerComponentImpl uplink(FW_OPTIONAL_NAME("uplink"));
  """

  init uplink phase Fpp.ToCpp.Phases.configComponents """
  uplink.setup(ConfigObjects::uplink::deframing);
  """

}
