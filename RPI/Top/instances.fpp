module RPI {

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {

    constant queueSize = 10

    constant stackSize = 64 * 1024

  }

  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------

  instance rateGroup10HzComp: Svc.ActiveRateGroup base id 200 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 40 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE context[] = { RpiDemo::RG_CONTEXT_10Hz, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    RPI::rateGroup10HzComp.configure(
        ConfigObjects::RPI_rateGroup10HzComp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::RPI_rateGroup10HzComp::context)
    );
    """

  }

  instance chanTlm: Svc.TlmChan base id 400 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 25

  instance cmdDisp: Svc.CommandDispatcher base id 500 \
    queue size 20 \
    stack size Default.stackSize \
    priority 30

  instance prmDb: Svc.PrmDb base id 600 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 20 \
  {
    phase Fpp.ToCpp.Phases.readParameters """
    RPI::prmDb.configure("PrmDb.dat");
    RPI::prmDb.readParamFile();
    """

  }

  instance cmdSeq: Svc.CmdSequencer base id 700 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 30 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      BUFFER_SIZE = 5*1024,
      TIMEOUT = 30
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    {
      using namespace ConfigConstants::RPI_cmdSeq;
      RPI::cmdSeq.allocateBuffer(
          0,
          Allocation::mallocator,
          ConfigConstants::RPI_cmdSeq::BUFFER_SIZE
      );
      RPI::cmdSeq.setTimeout(TIMEOUT);
    }
    """

    phase Fpp.ToCpp.Phases.tearDownComponents """
    RPI::cmdSeq.deallocateBuffer(Allocation::mallocator);
    """

  }

  instance fileUplink: Svc.FileUplink base id 800 \
    queue size 30 \
    stack size Default.stackSize \
    priority 30

  instance rateGroup1HzComp: Svc.ActiveRateGroup base id 300 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 40 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE context[] = { 0, 0, RpiDemo::RG_CONTEXT_1Hz, 0, 0, 0, 0, 0, 0, 0 };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    RPI::rateGroup1HzComp.configure(
        ConfigObjects::RPI_rateGroup1HzComp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::RPI_rateGroup1HzComp::context)
    );
    """

  }

  instance eventLogger: Svc.ActiveLogger base id 1400 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 25

  instance fileDownlink: Svc.FileDownlink base id 1800 \
    queue size 30 \
    stack size Default.stackSize \
    priority 20 \
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
    RPI::fileDownlink.configure(
        ConfigConstants::RPI_fileDownlink::TIMEOUT,
        ConfigConstants::RPI_fileDownlink::COOLDOWN,
        ConfigConstants::RPI_fileDownlink::CYCLE_TIME,
        ConfigConstants::RPI_fileDownlink::FILE_QUEUE_DEPTH
    );
    """

  }

  instance rpiDemo: RPI.RpiDemo base id 2700 \
    queue size Default.queueSize \
    stack size Default.stackSize \
    priority 30

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
    RPI::health.setPingEntries(
        ConfigObjects::RPI_health::pingEntries,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::RPI_health::pingEntries),
        ConfigConstants::RPI_health::WATCHDOG_CODE
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
      using namespace ConfigConstants::RPI_fileUplinkBufferManager;
      bufferBins.bins[0].bufferSize = STORE_SIZE;
      bufferBins.bins[0].numBuffers = QUEUE_SIZE;
      RPI::fileUplinkBufferManager.setup(
          MGR_ID,
          0,
          Allocation::mallocator,
          // OK to supply a local object here: BufferManager makes a copy
          bufferBins
      );
    }
    """

    phase Fpp.ToCpp.Phases.tearDownComponents """
    RPI::fileUplinkBufferManager.cleanup();
    """

  }

  instance fatalAdapter: Svc.AssertFatalAdapter base id 1000

  instance staticMemory: Svc.StaticMemory base id 1200

  instance downlink: Svc.Framer base id 1220 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    Svc::FprimeFraming framing;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    RPI::downlink.setup(ConfigObjects::RPI_downlink::framing);
    """

  }

  instance uplink: Svc.Deframer base id 1240 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    Svc::FprimeDeframing deframing;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    RPI::uplink.setup(ConfigObjects::RPI_uplink::deframing);
    """

  }

  instance comm: Drv.TcpClient base id 1260 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      PRIORITY = 30,
      STACK_SIZE = Default::stackSize
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    // Configure socket server if and only if there is a valid specification
    if (state.hostName != nullptr && state.portNumber != 0) {
        RPI::comm.configure(state.hostName, state.portNumber);
    }
    """

    phase Fpp.ToCpp.Phases.startTasks """
    // Initialize socket server if and only if there is a valid specification
    if (state.hostName != nullptr && state.portNumber != 0) {
        // Uplink is configured for receive so a socket task is started
        Os::TaskString name("ReceiveTask");
        RPI::comm.start(
            name,
            ConfigConstants::RPI_comm::PRIORITY,
            ConfigConstants::RPI_comm::STACK_SIZE
        );
    }
    """

    phase Fpp.ToCpp.Phases.stopTasks """
    RPI::comm.stop();
    """

    phase Fpp.ToCpp.Phases.freeThreads """
    (void) RPI::comm.join();
    """

  }

  instance posixTime: Svc.PosixTime base id 1500

  instance linuxTimer: Svc.LinuxTimer base id 1600 \
  {

    phase Fpp.ToCpp.Phases.stopTasks """
    RPI::linuxTimer.quit();
    """

  }

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 1700 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    Svc::RateGroupDriver::DividerSet rgDivs{{{1, 0}, {10, 0}, {0, 0}}};
    """

    phase Fpp.ToCpp.Phases.configComponents """
    RPI::rateGroupDriverComp.configure(
        ConfigObjects::RPI_rateGroupDriverComp::rgDivs
    );
    """
  }

  instance textLogger: Svc.PassiveTextLogger base id 1900

  instance uartDrv: Drv.LinuxUartDriver base id 2000 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const bool status = uartDrv.open("/dev/serial0",
          Drv::LinuxUartDriver::BAUD_19200,
          Drv::LinuxUartDriver::NO_FLOW,
          Drv::LinuxUartDriver::PARITY_NONE,
          1024
      );
      if (!status) {
        Fw::Logger::log("[ERROR] Could not open UART driver\\n");
        Init::status = false;
      }
    }
    """

    phase Fpp.ToCpp.Phases.startTasks """
    if (Init::status) {
      RPI::uartDrv.start();
    }
    else {
      Fw::Logger::log("[ERROR] Initialization failed; not starting UART driver\\n");
    }
    """

    phase Fpp.ToCpp.Phases.stopTasks """
    RPI::uartDrv.quitReadThread();
    """

  }

  instance ledDrv: Drv.LinuxGpioDriver base id 2100 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const Os::File::Status status = RPI::ledDrv.open("/dev/gpiochip0", 21, Drv::LinuxGpioDriver::GpioConfiguration::GPIO_OUTPUT);
      if (status != Os::File::Status::OP_OK) {
        Fw::Logger::log("[ERROR] Could not open LED driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance gpio23Drv: Drv.LinuxGpioDriver base id 2200 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const Os::File::Status status = RPI::gpio23Drv.open("/dev/gpiochip0", 23, Drv::LinuxGpioDriver::GpioConfiguration::GPIO_OUTPUT);
      if (status != Os::File::Status::OP_OK) {
        Fw::Logger::log("[ERROR] Could not open GPIO 23 driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance gpio24Drv: Drv.LinuxGpioDriver base id 2300 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const Os::File::Status status = RPI::gpio24Drv.open("/dev/gpiochip0", 24, Drv::LinuxGpioDriver::GpioConfiguration::GPIO_OUTPUT);
      if (status != Os::File::Status::OP_OK) {
        Fw::Logger::log("[ERROR] Could not open GPIO 24 driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance gpio25Drv: Drv.LinuxGpioDriver base id 2400 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const Os::File::Status status = RPI::gpio25Drv.open("/dev/gpiochip0", 25, Drv::LinuxGpioDriver::GpioConfiguration::GPIO_INPUT);
      if (status != Os::File::Status::OP_OK) {
        Fw::Logger::log("[ERROR] Could not open GPIO 25 driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance gpio17Drv: Drv.LinuxGpioDriver base id 2500 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const Os::File::Status status = RPI::gpio17Drv.open("/dev/gpiochip0", 17, Drv::LinuxGpioDriver::GpioConfiguration::GPIO_INPUT);
      if (status != Os::File::Status::OP_OK) {
        Fw::Logger::log("[ERROR] Could not open GPIO 17 driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance spiDrv: Drv.LinuxSpiDriver base id 2600 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const bool status = RPI::spiDrv.open(0, 0, Drv::SPI_FREQUENCY_1MHZ);
      if (!status) {
        Fw::Logger::log("[ERROR] Could not open SPI driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance uartBufferManager: Svc.BufferManager base id 2800 \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      STORE_SIZE = 3000,
      QUEUE_SIZE = 30,
      MGR_ID = 300
    };
    """

    phase Fpp.ToCpp.Phases.configComponents """
    {
      Svc::BufferManager::BufferBins bufferBins;
      memset(&bufferBins, 0, sizeof(bufferBins));
      using namespace ConfigConstants::RPI_uartBufferManager;
      bufferBins.bins[0].bufferSize = STORE_SIZE;
      bufferBins.bins[0].numBuffers = QUEUE_SIZE;
      RPI::uartBufferManager.setup(
          MGR_ID,
          0,
          Allocation::mallocator,
          // OK to supply a local object here: BufferManager makes a copy
          bufferBins
      );
    }
    """

    phase Fpp.ToCpp.Phases.tearDownComponents """
    RPI::uartBufferManager.cleanup();
    """
  }


}
