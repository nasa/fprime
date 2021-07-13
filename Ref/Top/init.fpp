@ Instance initializers
module Ref {

  # ----------------------------------------------------------------------
  # blockDrv
  # ----------------------------------------------------------------------

  init blockDrv phase Fpp.ToCpp.Phases.instances """
  // Declared in RefTopologyDefs.cpp
  """

  # ----------------------------------------------------------------------
  # chanTlm
  # ----------------------------------------------------------------------

  init chanTlm phase Fpp.ToCpp.Phases.instances """
  Svc::TlmChanImpl chanTlm(FW_OPTIONAL_NAME("chanTlm"));
  """

  # ----------------------------------------------------------------------
  # cmdDisp
  # ----------------------------------------------------------------------
  init cmdDisp phase Fpp.ToCpp.Phases.instances """
  Svc::CommandDispatcherImpl cmdDisp(FW_OPTIONAL_NAME("cmdDisp"));
  """

  # ----------------------------------------------------------------------
  # cmdSeq
  # ----------------------------------------------------------------------

  init cmdSeq phase Fpp.ToCpp.Phases.configConstants """
  enum {
    BUFFER_SIZE = 5*1024
  };

  """
  init cmdSeq phase Fpp.ToCpp.Phases.instances """
  Svc::CmdSequencerComponentImpl cmdSeq(FW_OPTIONAL_NAME("cmdSeq"));
  """

  # ----------------------------------------------------------------------
  # comm
  # ----------------------------------------------------------------------
  init comm phase Fpp.ToCpp.Phases.instances """
  Drv::TcpClientComponentImpl comm(FW_OPTIONAL_NAME("comm"));
  """

  # ----------------------------------------------------------------------
  # downlink
  # ----------------------------------------------------------------------

  init downlink phase Fpp.ToCpp.Phases.configObjects """
  Svc::FprimeFraming framing;
  """

  init downlink phase Fpp.ToCpp.Phases.instances """
  Svc::FramerComponentImpl downlink(FW_OPTIONAL_NAME("downlink"));
  """

  # ----------------------------------------------------------------------
  # eventLogger
  # ----------------------------------------------------------------------
  
  init eventLogger phase Fpp.ToCpp.Phases.instances """
  Svc::ActiveLoggerImpl eventLogger(FW_OPTIONAL_NAME("eventLogger"));
  """

  # ----------------------------------------------------------------------
  # fatalAdapter
  # ----------------------------------------------------------------------

  init fatalAdapter phase Fpp.ToCpp.Phases.instances """
  Svc::AssertFatalAdapterComponentImpl fatalAdapter(FW_OPTIONAL_NAME("fatalAdapter"));
  """

  # ----------------------------------------------------------------------
  # fatalHandler
  # ----------------------------------------------------------------------

  init fatalHandler phase Fpp.ToCpp.Phases.instances """
  Svc::FatalHandlerComponentImpl fatalHandler(FW_OPTIONAL_NAME("fatalHandler"));
  """

  # ----------------------------------------------------------------------
  # fileUplinkBufferManager
  # ----------------------------------------------------------------------

  init fileUplinkBufferManager phase Fpp.ToCpp.Phases.instances """
  Svc::BufferManagerComponentImpl fileUplinkBufferManager(FW_OPTIONAL_NAME("fileUplinkBufferManager"));
  """

  # ----------------------------------------------------------------------
  # health
  # ----------------------------------------------------------------------

  init $health phase Fpp.ToCpp.Phases.instances """
  Svc::HealthImpl health(FW_OPTIONAL_NAME("health"));
  """

  # ----------------------------------------------------------------------
  # linuxTime
  # ----------------------------------------------------------------------

  init linuxTime phase Fpp.ToCpp.Phases.instances """
  Svc::LinuxTimeImpl linuxTime(FW_OPTIONAL_NAME("linuxTime"));
  """

  # ----------------------------------------------------------------------
  # pingRcvr
  # ----------------------------------------------------------------------

  init pingRcvr phase Fpp.ToCpp.Phases.instances """
  PingReceiverComponentImpl pingRcvr(FW_OPTIONAL_NAME("pingRcvr"));
  """

  # ----------------------------------------------------------------------
  # prmDb
  # ----------------------------------------------------------------------

  init prmDb phase Fpp.ToCpp.Phases.instances """
  Svc::PrmDbImpl prmDb(FW_OPTIONAL_NAME("prmDb"), "PrmDb.dat");
  """

  # ----------------------------------------------------------------------
  # rateGroup1Comp
  # ----------------------------------------------------------------------
  
  init rateGroup1Comp phase Fpp.ToCpp.Phases.configObjects """
  NATIVE_UINT_TYPE context[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  """

  init rateGroup1Comp phase Fpp.ToCpp.Phases.instances """
  Svc::ActiveRateGroupImpl rateGroup1Comp(
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
  Svc::ActiveRateGroupImpl rateGroup2Comp(
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
  Svc::ActiveRateGroupImpl rateGroup3Comp(
      FW_OPTIONAL_NAME("rateGroup3Comp"),
      ConfigObjects::rateGroup3Comp::context,
      FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup3Comp::context)
  );
  """

  # ----------------------------------------------------------------------
  # rateGroupDriverComp
  # ----------------------------------------------------------------------
  
  init rateGroupDriverComp phase Fpp.ToCpp.Phases.configObjects """
  NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriverImpl::DIVIDER_SIZE] = { 1, 2, 4 };
  """

  init rateGroupDriverComp phase Fpp.ToCpp.Phases.instances """
  Svc::RateGroupDriverImpl rateGroupDriverComp(
      FW_OPTIONAL_NAME("rateGroupDriverComp"),
      ConfigObjects::rateGroupDriverComp::rgDivs,
      FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroupDriverComp::rgDivs)
  );
  """

  # ----------------------------------------------------------------------
  # recvBuffComp
  # ----------------------------------------------------------------------

  init recvBuffComp phase Fpp.ToCpp.Phases.instances """
  RecvBuffImpl recvBuffComp(FW_OPTIONAL_NAME("recvBuffComp"));
  """

  # ----------------------------------------------------------------------
  # sendBuffComp
  # ----------------------------------------------------------------------

  init sendBuffComp phase Fpp.ToCpp.Phases.instances """
  SendBuffImpl sendBuffComp(FW_OPTIONAL_NAME("sendBuffComp"));
  """

  # ----------------------------------------------------------------------
  # staticMemory
  # ----------------------------------------------------------------------

  init staticMemory phase Fpp.ToCpp.Phases.instances """
  Svc::StaticMemoryComponentImpl staticMemory(FW_OPTIONAL_NAME("staticMemory"));
  """

  # ----------------------------------------------------------------------
  # textLogger
  # ----------------------------------------------------------------------

  init textLogger phase Fpp.ToCpp.Phases.instances """
  Svc::ConsoleTextLoggerImpl textLogger(FW_OPTIONAL_NAME("textLogger"));
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

}
