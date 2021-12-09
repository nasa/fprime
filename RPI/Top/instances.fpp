module RPI {

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {

    constant queueSize = 10

    constant stackSize = 16 * 1024

  }

  # ----------------------------------------------------------------------
  # Active component instances 
  # ----------------------------------------------------------------------

  instance rateGroup10HzComp: Svc.ActiveRateGroup base id 200 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 120 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_UINT_TYPE context[] = { RGContext::CONTEXT_10Hz, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.instances """
    Svc::ActiveRateGroup rateGroup10HzComp(
        FW_OPTIONAL_NAME("rateGroup10HzComp"),
        ConfigObjects::rateGroup10HzComp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup10HzComp::context)
    );
    """

  }

  instance chanTlm: Svc.TlmChan base id 400 \
    queue size Default.queueSize \ 
    stack size Default.stackSize \
    priority 95

  instance cmdDisp: Svc.CommandDispatcher base id 500 \
    queue size 20 \
    stack size Default.stackSize \
    priority 100

  instance prmDb: Svc.PrmDb base id 600 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 90 \
  {

    phase Fpp.ToCpp.Phases.instances """
    Svc::PrmDb prmDb(FW_OPTIONAL_NAME("prmDb"), "PrmDb.dat");
    """

    phase Fpp.ToCpp.Phases.readParameters """
    prmDb.readParamFile();
    """

  }

  instance cmdSeq: Svc.CmdSequencer base id 700 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 100 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      BUFFER_SIZE = 5*1024,
      TIMEOUT = 30
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    {
      using namespace ConfigConstants::cmdSeq;
      cmdSeq.allocateBuffer(
          0,
          Allocation::mallocator,
          ConfigConstants::cmdSeq::BUFFER_SIZE
      );
      cmdSeq.setTimeout(TIMEOUT);
    }
    """

    phase Fpp.ToCpp.Phases.tearDownComponents """
    cmdSeq.deallocateBuffer(Allocation::mallocator);
    """

  }

  instance fileUplink: Svc.FileUplink base id 800 \
    queue size 30 \
    stack size Default.stackSize \
    priority 100

  instance rateGroup1HzComp: Svc.ActiveRateGroup base id 300 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 120 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_UINT_TYPE context[] = { RGContext::CONTEXT_1Hz, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.instances """
    Svc::ActiveRateGroup rateGroup1HzComp(
        FW_OPTIONAL_NAME("rateGroup1HzComp"),
        ConfigObjects::rateGroup1HzComp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup1HzComp::context)
    );
    """

  }

  instance eventLogger: Svc.ActiveLogger base id 1400 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 95

  instance fileDownlink: Svc.FileDownlink base id 1800 \
    queue size 30 \
    stack size Default.stackSize \
    priority 90 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      TIMEOUT = 1000,
      COOLDOWN = 200,
      CYCLE_TIME = 100,
      FILE_QUEUE_DEPTH = 10
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    fileDownlink.configure(
        ConfigConstants::fileDownlink::TIMEOUT,
        ConfigConstants::fileDownlink::COOLDOWN,
        ConfigConstants::fileDownlink::CYCLE_TIME,
        ConfigConstants::fileDownlink::FILE_QUEUE_DEPTH
    );
    """

  }

  instance rpiDemo: RPI.RpiDemo base id 2700 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 80

  # ----------------------------------------------------------------------
  # Queued component instances 
  # ----------------------------------------------------------------------

  instance $health: Svc.Health base id 1100 \
    queue size 25 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      WATCHDOG_CODE = 0x123
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    health.setPingEntries(
        ConfigObjects::health::pingEntries,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::health::pingEntries),
        ConfigConstants::health::WATCHDOG_CODE
    );
    """

  }

  # ----------------------------------------------------------------------
  # Passive component instances 
  # ----------------------------------------------------------------------

  instance fatalHandler: Svc.FatalHandler base id 100

  instance fileUplinkBufferManager: Svc.BufferManager base id 900 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      STORE_SIZE = 3000,
      QUEUE_SIZE = 30,
      MGR_ID = 200
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    {
      Svc::BufferManager::BufferBins bufferBins;
      memset(&bufferBins, 0, sizeof(bufferBins));
      using namespace ConfigConstants::fileUplinkBufferManager;
      bufferBins.bins[0].bufferSize = STORE_SIZE;
      bufferBins.bins[0].numBuffers = QUEUE_SIZE;
      fileUplinkBufferManager.setup(
          MGR_ID,
          0,
          Allocation::mallocator,
          // OK to supply a local object here: BufferManager makes a copy
          bufferBins
      );
    }
    """

    phase Fpp.ToCpp.Phases.tearDownComponents """
    fileUplinkBufferManager.cleanup();
    """

  }

  instance fatalAdapter: Svc.AssertFatalAdapter base id 1000

  instance staticMemory: Svc.StaticMemory base id 1200

  instance downlink: Svc.Framer base id 1220

  instance uplink: Svc.Deframer base id 1240

  instance comm: Drv.ByteStreamDriverModel base id 1260 \
    at "../../Drv/TcpClient/TcpClient.hpp" \
  {

    phase Fpp.ToCpp.Phases.instances """
    Drv::TcpClient comm(FW_OPTIONAL_NAME("comm"));
    """

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      PRIORITY = 100,
      STACK_SIZE = Default::stackSize
    };
    """

    phase Fpp.ToCpp.Phases.startTasks """
    // Initialize socket server if and only if there is a valid specification
    if (state.hostName != nullptr && state.portNumber != 0) {
        Os::TaskString name("ReceiveTask");
        // Uplink is configured for receive so a socket task is started
        comm.configure(state.hostName, state.portNumber);
        comm.startSocketTask(
            name,
            ConfigConstants::comm::PRIORITY,
            ConfigConstants::comm::STACK_SIZE
        );
    }
    """

    phase Fpp.ToCpp.Phases.stopTasks """
    comm.stopSocketTask();
    """

    phase Fpp.ToCpp.Phases.freeThreads """
    (void) comm.joinSocketTask(nullptr);
    """

  }

  instance linuxTime: Svc.Time base id 1500 \
    at "../../Svc/LinuxTime/LinuxTime.hpp" \
  {

    phase Fpp.ToCpp.Phases.instances """
    Svc::LinuxTime linuxTime(FW_OPTIONAL_NAME("linuxTime"));
    """

  }

  instance linuxTimer: Svc.LinuxTimer base id 1600 \
  {

    phase Fpp.ToCpp.Phases.instances """
    // Declared in RPITopologyDefs.cpp
    """

    phase Fpp.ToCpp.Phases.stopTasks """
    linuxTimer.quit();
    """

  }

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 1700 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriver::DIVIDER_SIZE] = { 1, 10, 0 };
    """

    phase Fpp.ToCpp.Phases.instances """
    Svc::RateGroupDriver rateGroupDriverComp(
        FW_OPTIONAL_NAME("rateGroupDriverComp"),
        ConfigObjects::rateGroupDriverComp::rgDivs,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroupDriverComp::rgDivs)
    );
    """

  }

  instance textLogger: Svc.PassiveTextLogger base id 1900

  instance uartDrv: Drv.LinuxSerialDriver base id 2000 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    auto status = uartDrv.open("/dev/serial0",
        Drv::LinuxSerialDriverComponentImpl::BAUD_19200,
        Drv::LinuxSerialDriverComponentImpl::NO_FLOW,
        Drv::LinuxSerialDriverComponentImpl::PARITY_NONE,
        true
    );
    FW_ASSERT(status);
    """

    phase Fpp.ToCpp.Phases.startTasks """
    uartDrv.startReadThread();
    """

    phase Fpp.ToCpp.Phases.stopTasks """
    uartDrv.quitReadThread();
    """

  }

  instance ledDrv: Drv.LinuxGpioDriver base id 2100 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    FW_ASSERT(ledDrv.open(21, Drv::LinuxGpioDriverComponentImpl::GPIO_OUT));
    """

  }

  instance gpio23Drv: Drv.LinuxGpioDriver base id 2200 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    FW_ASSERT(gpio23Drv.open(23, Drv::LinuxGpioDriverComponentImpl::GPIO_OUT));
    """

  }

  instance gpio24Drv: Drv.LinuxGpioDriver base id 2300 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    FW_ASSERT(gpio24Drv.open(24, Drv::LinuxGpioDriverComponentImpl::GPIO_OUT));
    """

  }

  instance gpio25Drv: Drv.LinuxGpioDriver base id 2400 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    FW_ASSERT(gpio25Drv.open(25, Drv::LinuxGpioDriverComponentImpl::GPIO_IN));
    """

  }

  instance gpio17Drv: Drv.LinuxGpioDriver base id 2500 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    FW_ASSERT(gpio17Drv.open(17, Drv::LinuxGpioDriverComponentImpl::GPIO_IN));
    """

  }

  instance spiDrv: Drv.LinuxSpiDriver base id 2600 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    FW_ASSERT(spiDrv.open(0, 0, Drv::SPI_FREQUENCY_1MHZ));
    """

  }

} 
