#include <Components.hpp>

#include <Fw/Types/Assert.hpp>
#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <Os/File.hpp>
#include <Os/TaskString.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <RPI/Top/RpiSchedContexts.hpp>
#include <Svc/FramingProtocol/FprimeProtocol.hpp>

enum {
    UPLINK_BUFFER_STORE_SIZE = 3000,
    UPLINK_BUFFER_QUEUE_SIZE = 30,
    UPLINK_BUFFER_MGR_ID = 200
};

Svc::FprimeDeframing deframing;
Svc::FprimeFraming framing;

// Component instances

// Rate Group Dividers for 10Hz and 1Hz

static NATIVE_INT_TYPE rgDivs[] = {1,10,0};
Svc::RateGroupDriverImpl rateGroupDriverComp("RGDRV",rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

// Context array variables are passed to rate group members if needed to distinguish one call from another
// These context must match the rate group members connected in RPITopologyAi.xml
static NATIVE_UINT_TYPE rg10HzContext[] = {Rpi::CONTEXT_RPI_DEMO_10Hz,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup10HzComp("RG10Hz",rg10HzContext,FW_NUM_ARRAY_ELEMENTS(rg10HzContext));

static NATIVE_UINT_TYPE rg1HzContext[] = {0,0,Rpi::CONTEXT_RPI_DEMO_1Hz,0,0,0,0,0,0,0};

Svc::ActiveRateGroupImpl rateGroup1HzComp("RG1Hz",rg1HzContext,FW_NUM_ARRAY_ELEMENTS(rg1HzContext));

// Command Components
Drv::TcpClientComponentImpl comm(FW_OPTIONAL_NAME("Tcp"));

#if FW_ENABLE_TEXT_LOGGING
Svc::ConsoleTextLoggerImpl textLogger("TLOG");
#endif

Svc::ActiveLoggerImpl eventLogger("ELOG");

Svc::PosixTime posixTime("LTIME");

Svc::LinuxTimerComponentImpl linuxTimer("LTIMER");

Svc::TlmChanImpl chanTlm("TLM");

Svc::CommandDispatcherImpl cmdDisp("CMDDISP");

// This needs to be statically allocated
Fw::MallocAllocator mallocator;
Svc::CmdSequencerComponentImpl cmdSeq("CMDSEQ");

Svc::PrmDbImpl prmDb("PRM","PrmDb.dat");

Svc::FileUplink fileUplink("fileUplink");

Svc::FileDownlink fileDownlink ("fileDownlink");

Svc::BufferManagerComponentImpl fileUplinkBufferManager("fileUplinkBufferManager");

Svc::HealthImpl health("health");

Svc::StaticMemoryComponentImpl staticMemory(FW_OPTIONAL_NAME("staticMemory"));

Svc::FramerComponentImpl downlink(FW_OPTIONAL_NAME("downlink"));

Svc::DeframerComponentImpl uplink(FW_OPTIONAL_NAME("uplink"));

Svc::AssertFatalAdapterComponentImpl fatalAdapter("fatalAdapter");

Svc::FatalHandlerComponentImpl fatalHandler("fatalHandler");

Drv::LinuxSerialDriverComponentImpl uartDrv("uartDrv");

Drv::LinuxSpiDriverComponentImpl spiDrv("spiDrv");

Drv::LinuxGpioDriverComponentImpl ledDrv("ledDrv");
Drv::LinuxGpioDriverComponentImpl gpio23Drv("gpio23Drv");
Drv::LinuxGpioDriverComponentImpl gpio24Drv("gpio24Drv");
Drv::LinuxGpioDriverComponentImpl gpio25Drv("gpio25Drv");
Drv::LinuxGpioDriverComponentImpl gpio17Drv("gpio17Drv");

Rpi::RpiDemoComponentImpl rpiDemo("rpiDemo");

void constructApp(U32 port_number, char* hostname) {
    staticMemory.init(0);
    // Initialize rate group driver
    rateGroupDriverComp.init();

    // Initialize the rate groups
    rateGroup10HzComp.init(10,0);
    rateGroup1HzComp.init(10,1);

#if FW_ENABLE_TEXT_LOGGING
    textLogger.init();
#endif

    eventLogger.init(10,0);

    posixTime.init(0);

    linuxTimer.init(0);

    chanTlm.init(10,0);

    cmdDisp.init(20,0);

    cmdSeq.init(10,0);
    cmdSeq.allocateBuffer(0,mallocator,5*1024);

    prmDb.init(10,0);

    downlink.init(0);
    uplink.init(0);
    comm.init(0);

    fileUplink.init(30, 0);
    fileDownlink.configure(1000, 200, 100, 10);
    fileDownlink.init(30, 0);
    fileUplinkBufferManager.init(0);

    fatalAdapter.init(0);
    fatalHandler.init(0);
    health.init(25,0);

    uartDrv.init(0);

    spiDrv.init(0);

    ledDrv.init(0);
    gpio23Drv.init(0);
    gpio24Drv.init(0);
    gpio25Drv.init(0);
    gpio17Drv.init(0);

    rpiDemo.init(10,0);
    downlink.setup(framing);
    uplink.setup(deframing);

    constructRPIArchitecture();

    /* Register commands */
    cmdSeq.regCommands();
    cmdDisp.regCommands();
    eventLogger.regCommands();
    prmDb.regCommands();
    fileDownlink.regCommands();
    health.regCommands();
    rpiDemo.regCommands();

    // set sequencer timeout
    cmdSeq.setTimeout(30);

    // read parameters
    prmDb.readParamFile();

    // set health ping entries

    // This list has to match the connections in RPITopologyAppAi.xml

    Svc::HealthImpl::PingEntry pingEntries[] = {
        {3,5,rateGroup10HzComp.getObjName()}, // 0
        {3,5,rateGroup1HzComp.getObjName()}, // 1
        {3,5,cmdDisp.getObjName()}, // 2
        {3,5,cmdSeq.getObjName()}, // 3
        {3,5,chanTlm.getObjName()}, // 4
        {3,5,eventLogger.getObjName()}, // 5
        {3,5,prmDb.getObjName()}, // 6
        {3,5,fileDownlink.getObjName()}, // 7
        {3,5,fileUplink.getObjName()}, // 8
    };

    // register ping table
    health.setPingEntries(pingEntries,FW_NUM_ARRAY_ELEMENTS(pingEntries),0x123);

    // load parameters
    rpiDemo.loadParameters();

    // set up BufferManager instances
    Svc::BufferManagerComponentImpl::BufferBins upBuffMgrBins;
    memset(&upBuffMgrBins,0,sizeof(upBuffMgrBins));
    upBuffMgrBins.bins[0].bufferSize = UPLINK_BUFFER_STORE_SIZE;
    upBuffMgrBins.bins[0].numBuffers = UPLINK_BUFFER_QUEUE_SIZE;
    fileUplinkBufferManager.setup(UPLINK_BUFFER_MGR_ID,0,mallocator,upBuffMgrBins);

    // Active component startup
    // start rate groups
    rateGroup10HzComp.start();
    rateGroup1HzComp.start();
    // start dispatcher
    cmdDisp.start();
    // start sequencer
    cmdSeq.start();
    // start telemetry
    eventLogger.start();
    chanTlm.start();
    prmDb.start();

    fileDownlink.start();
    fileUplink.start();
    rpiDemo.start();

    // Use the mini-UART for our serial connection
    // https://www.raspberrypi.org/documentation/configuration/uart.md

    if (not uartDrv.open("/dev/serial0",
            Drv::LinuxSerialDriverComponentImpl::BAUD_19200,
            Drv::LinuxSerialDriverComponentImpl::NO_FLOW,
            Drv::LinuxSerialDriverComponentImpl::PARITY_NONE,
            true)) {
        return;
    }

    if (not spiDrv.open(0,0,Drv::SPI_FREQUENCY_1MHZ)) {
        return;
    }

    if (not ledDrv.open(21,Drv::LinuxGpioDriverComponentImpl::GPIO_OUT)) {
        return;
    }

    if (not gpio23Drv.open(23,Drv::LinuxGpioDriverComponentImpl::GPIO_OUT)) {
        return;
    }

    if (not gpio24Drv.open(24,Drv::LinuxGpioDriverComponentImpl::GPIO_OUT)) {
        return;
    }

    if (not gpio25Drv.open(25,Drv::LinuxGpioDriverComponentImpl::GPIO_IN)) {
        return;
    }

    if (not gpio17Drv.open(17,Drv::LinuxGpioDriverComponentImpl::GPIO_IN)) {
        return;
    }

    uartDrv.startReadThread();

    // Initialize socket server if and only if there is a valid specification
    if (hostname != nullptr && port_number != 0) {
        Os::TaskString name("ReceiveTask");
        // Uplink is configured for receive so a socket task is started
        comm.configure(hostname, port_number);
        comm.startSocketTask(name);
    }
}

void exitTasks() {
    uartDrv.quitReadThread();
    linuxTimer.quit();
    rateGroup1HzComp.exit();
    rateGroup10HzComp.exit();
    cmdDisp.exit();
    eventLogger.exit();
    chanTlm.exit();
    prmDb.exit();
    fileUplink.exit();
    fileDownlink.exit();
    cmdSeq.exit();
    rpiDemo.exit();
    comm.stopSocketTask();
    (void) comm.joinSocketTask(nullptr);
    cmdSeq.deallocateBuffer(mallocator);
    fileUplinkBufferManager.cleanup();
}

