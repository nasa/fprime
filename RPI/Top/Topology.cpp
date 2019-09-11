#include <Components.hpp>

#include <Fw/Types/Assert.hpp>
#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <Os/File.hpp>
#include <Fw/Types/MallocAllocator.hpp>
#include <RPI/Top/RpiSchedContexts.hpp>

enum {
    DOWNLINK_PACKET_SIZE = 500,
    DOWNLINK_BUFFER_STORE_SIZE = 2500,
    DOWNLINK_BUFFER_QUEUE_SIZE = 5,
    UPLINK_BUFFER_STORE_SIZE = 3000,
    UPLINK_BUFFER_QUEUE_SIZE = 30
};

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
Svc::SocketGndIfImpl sockGndIf("SGIF");

#if FW_ENABLE_TEXT_LOGGING
Svc::ConsoleTextLoggerImpl textLogger("TLOG");
#endif

Svc::ActiveLoggerImpl eventLogger("ELOG");

Svc::LinuxTimeImpl linuxTime("LTIME");

Svc::LinuxTimerComponentImpl linuxTimer("LTIMER");

Svc::TlmChanImpl chanTlm("TLM");

Svc::CommandDispatcherImpl cmdDisp("CMDDISP");

// This needs to be statically allocated
Fw::MallocAllocator seqMallocator;

Svc::CmdSequencerComponentImpl cmdSeq("CMDSEQ");

Svc::PrmDbImpl prmDb("PRM","PrmDb.dat");

Svc::FileUplink fileUplink("fileUplink");

Svc::FileDownlink fileDownlink ("fileDownlink", DOWNLINK_PACKET_SIZE);

Svc::BufferManager fileDownlinkBufferManager("fileDownlinkBufferManager", DOWNLINK_BUFFER_STORE_SIZE, DOWNLINK_BUFFER_QUEUE_SIZE);

Svc::BufferManager fileUplinkBufferManager("fileUplinkBufferManager", UPLINK_BUFFER_STORE_SIZE, UPLINK_BUFFER_QUEUE_SIZE);

Svc::HealthImpl health("health");

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

void constructApp(int port_number, char* hostname) {

    // Initialize rate group driver
    rateGroupDriverComp.init();

    // Initialize the rate groups
    rateGroup10HzComp.init(10,0);
    rateGroup1HzComp.init(10,1);
    
#if FW_ENABLE_TEXT_LOGGING
    textLogger.init();
#endif

    eventLogger.init(10,0);

    linuxTime.init(0);

    linuxTimer.init(0);

    chanTlm.init(10,0);

    cmdDisp.init(20,0);

    cmdSeq.init(10,0);
    cmdSeq.allocateBuffer(0,seqMallocator,5*1024);

    prmDb.init(10,0);

    sockGndIf.init(0);

    fileUplink.init(30, 0);
    fileDownlink.init(30, 0);
    fileUplinkBufferManager.init(0);
    fileDownlinkBufferManager.init(1);

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

    // Active component startup
    // start rate groups
    rateGroup10HzComp.start(0, 120,10 * 1024);
    rateGroup1HzComp.start(0, 119,10 * 1024);
    // start dispatcher
    cmdDisp.start(0,101,10*1024);
    // start sequencer
    cmdSeq.start(0,100,10*1024);
    // start telemetry
    eventLogger.start(0,98,10*1024);
    chanTlm.start(0,97,10*1024);
    prmDb.start(0,96,10*1024);

    fileDownlink.start(0, 100, 10*1024);
    fileUplink.start(0, 100, 10*1024);
    rpiDemo.start(0, 100, 10*1024);

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

    uartDrv.startReadThread(100,10*1024,-1);

    // Initialize socket server
    sockGndIf.startSocketTask(100, 10*1024, port_number, hostname, Svc::SocketGndIfImpl::SEND_UDP);

}

void exitTasks(void) {
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
}

