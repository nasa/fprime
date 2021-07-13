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

  
}
