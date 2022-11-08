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
    rateGroup10HzComp.configure(
        ConfigObjects::rateGroup10HzComp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup10HzComp::context)
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
    prmDb.configure("PrmDb.dat");
    prmDb.readParamFile();
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
    rateGroup1HzComp.configure(
        ConfigObjects::rateGroup1HzComp::context,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroup1HzComp::context)
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

  instance downlink: Svc.Framer base id 1220 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    Svc::FprimeFraming framing;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    downlink.setup(ConfigObjects::downlink::framing);
    """

  }

  instance uplink: Svc.Deframer base id 1240 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    Svc::FprimeDeframing deframing;
    """

    phase Fpp.ToCpp.Phases.configComponents """
    uplink.setup(ConfigObjects::uplink::deframing);
    """

  }

  instance comm: Drv.ByteStreamDriverModel base id 1260 \
    type "Drv::TcpClient" \
    at "../../Drv/TcpClient/TcpClient.hpp" \
  {

    phase Fpp.ToCpp.Phases.configConstants """
    enum {
      PRIORITY = 30,
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
    type "Svc::LinuxTime" \
    at "../../Svc/LinuxTime/LinuxTime.hpp"

  instance linuxTimer: Svc.LinuxTimer base id 1600 \
  {

    phase Fpp.ToCpp.Phases.stopTasks """
    linuxTimer.quit();
    """

  }

  instance rateGroupDriverComp: Svc.RateGroupDriver base id 1700 \
  {

    phase Fpp.ToCpp.Phases.configObjects """
    NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriver::DIVIDER_SIZE] = { 1, 10, 0 };
    """
    
    phase Fpp.ToCpp.Phases.configComponents """
    rateGroupDriverComp.configure(
        ConfigObjects::rateGroupDriverComp::rgDivs,
        FW_NUM_ARRAY_ELEMENTS(ConfigObjects::rateGroupDriverComp::rgDivs)
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
        Fw::Logger::logMsg("[ERROR] Could not open UART driver\\n");
        Init::status = false;
      }
    }
    """

    phase Fpp.ToCpp.Phases.startTasks """
    if (Init::status) {
      uartDrv.startReadThread();
    }
    else {
      Fw::Logger::logMsg("[ERROR] Initialization failed; not starting UART driver\\n");
    }
    """

    phase Fpp.ToCpp.Phases.stopTasks """
    uartDrv.quitReadThread();
    """

  }

  instance ledDrv: Drv.LinuxGpioDriver base id 2100 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const bool status = ledDrv.open(21, Drv::LinuxGpioDriverComponentImpl::GPIO_OUT);
      if (!status) {
        Fw::Logger::logMsg("[ERROR] Could not open LED driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance gpio23Drv: Drv.LinuxGpioDriver base id 2200 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const bool status = gpio23Drv.open(23, Drv::LinuxGpioDriverComponentImpl::GPIO_OUT);
      if (!status) {
        Fw::Logger::logMsg("[ERROR] Could not open GPIO 23 driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance gpio24Drv: Drv.LinuxGpioDriver base id 2300 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const bool status = gpio24Drv.open(24, Drv::LinuxGpioDriverComponentImpl::GPIO_OUT);
      if (!status) {
        Fw::Logger::logMsg("[ERROR] Could not open GPIO 24 driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance gpio25Drv: Drv.LinuxGpioDriver base id 2400 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const bool status = gpio25Drv.open(25, Drv::LinuxGpioDriverComponentImpl::GPIO_IN);
      if (!status) {
        Fw::Logger::logMsg("[ERROR] Could not open GPIO 25 driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance gpio17Drv: Drv.LinuxGpioDriver base id 2500 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const bool status = gpio17Drv.open(17, Drv::LinuxGpioDriverComponentImpl::GPIO_IN);
      if (!status) {
        Fw::Logger::logMsg("[ERROR] Could not open GPIO 17 driver\\n");
        Init::status = false;
      }
    }
    """

  }

  instance spiDrv: Drv.LinuxSpiDriver base id 2600 \
  {

    phase Fpp.ToCpp.Phases.configComponents """
    {
      const bool status = spiDrv.open(0, 0, Drv::SPI_FREQUENCY_1MHZ);
      if (!status) {
        Fw::Logger::logMsg("[ERROR] Could not open SPI driver\\n");
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
      using namespace ConfigConstants::uartBufferManager;
      bufferBins.bins[0].bufferSize = STORE_SIZE;
      bufferBins.bins[0].numBuffers = QUEUE_SIZE;
      uartBufferManager.setup(
          MGR_ID,
          0,
          Allocation::mallocator,
          // OK to supply a local object here: BufferManager makes a copy
          bufferBins
      );
    }
    """

    phase Fpp.ToCpp.Phases.tearDownComponents """
    uartBufferManager.cleanup();
    """
  }


}
