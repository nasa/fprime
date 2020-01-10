#include <Components.hpp>


#include <Fw/Types/Assert.hpp>
#include <Os/Task.hpp>
#include <Os/Log.hpp>
#include <Fw/Types/MallocAllocator.hpp>

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <stdlib.h>
#include <ctype.h>
#endif


Os::Log osLogger;

// List of context IDs

// GPS Application:
//  For GPS application specific items, look for GPS-- comments below
enum {
    DOWNLINK_PACKET_SIZE = 500,
    DOWNLINK_BUFFER_STORE_SIZE = 2500,
    DOWNLINK_BUFFER_QUEUE_SIZE = 5,
    UPLINK_BUFFER_STORE_SIZE = 3000,
    UPLINK_BUFFER_QUEUE_SIZE = 30
};

enum {
        ACTIVE_COMP_1HZ_RG,
        ACTIVE_COMP_P5HZ_RG,
        ACTIVE_COMP_P25HZ_RG,
        ACTIVE_COMP_CMD_DISP,
        ACTIVE_COMP_CMD_SEQ,
        ACTIVE_COMP_LOGGER,
        ACTIVE_COMP_TLM,
        ACTIVE_COMP_PRMDB,
        ACTIVE_COMP_FILE_DOWNLINK,
        ACTIVE_COMP_FILE_UPLINK,

        ACTIVE_COMP_BLKDRV,
        ACTIVE_COMP_PING_RECEIVER,
        // GPS-- our component is an active component, thus it needs a thread-id. Thread IDs come from this
        // enumeration to keep them unique.
        ACTIVE_COMP_GPS,

        CYCLER_TASK,
        NUM_ACTIVE_COMPS
};

// Registry
#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

// Component instance pointers
static NATIVE_INT_TYPE rgDivs[] = {1,2,4};
Svc::RateGroupDriverImpl rateGroupDriverComp(
#if FW_OBJECT_NAMES == 1
                    "RGDvr",
#endif
                    rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

static NATIVE_UINT_TYPE rg1Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup1Comp
#if FW_OBJECT_NAMES == 1
                    ("RG1",rg1Context,FW_NUM_ARRAY_ELEMENTS(rg1Context));
#else
                    (rg1Context,FW_NUM_ARRAY_ELEMENTS(rg1Context));
#endif
;

static NATIVE_UINT_TYPE rg2Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup2Comp
#if FW_OBJECT_NAMES == 1
                    ("RG2",rg2Context,FW_NUM_ARRAY_ELEMENTS(rg2Context));
#else
                    (rg2Context,FW_NUM_ARRAY_ELEMENTS(rg2Context));
#endif
;

static NATIVE_UINT_TYPE rg3Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup3Comp
#if FW_OBJECT_NAMES == 1
                    ("RG3",rg3Context,FW_NUM_ARRAY_ELEMENTS(rg3Context));
#else
                    (rg3Context,FW_NUM_ARRAY_ELEMENTS(rg3Context));
#endif
;

// Command Components
Svc::GroundInterfaceComponentImpl groundIf
#if FW_OBJECT_NAMES == 1
                    ("GNDIF")
#endif
;

//GPS-- GPS Component construction, notice if compiled with names, a name should be given.
GpsApp::GpsComponentImpl gpsImpl
#if FW_OBJECT_NAMES == 1
                    ("GPS")
#endif
;

Drv::LinuxSerialDriverComponentImpl gpsSerial
#if FW_OBJECT_NAMES == 1
                    ("GPSSERIAL")
#endif
;



#if FW_ENABLE_TEXT_LOGGING
Svc::ConsoleTextLoggerImpl textLogger
#if FW_OBJECT_NAMES == 1
                    ("TLOG")
#endif
;
#endif

Svc::ActiveLoggerImpl eventLogger
#if FW_OBJECT_NAMES == 1
                    ("ELOG")
#endif
;

Svc::LinuxTimeImpl linuxTime
#if FW_OBJECT_NAMES == 1
                    ("LTIME")
#endif
;

Svc::TlmChanImpl chanTlm
#if FW_OBJECT_NAMES == 1
                    ("TLM")
#endif
;

Svc::CommandDispatcherImpl cmdDisp
#if FW_OBJECT_NAMES == 1
                    ("CMDDISP")
#endif
;

Fw::MallocAllocator seqMallocator;
Svc::CmdSequencerComponentImpl cmdSeq
#if FW_OBJECT_NAMES == 1
                    ("CMDSEQ")
#endif
;

Svc::PrmDbImpl prmDb
#if FW_OBJECT_NAMES == 1
                    ("PRM","PrmDb.dat")
#else
                    ("PrmDb.dat")
#endif
;


Svc::FileUplink fileUplink ("fileUplink");
Svc::FileDownlink fileDownlink ("fileDownlink", DOWNLINK_PACKET_SIZE);
Svc::BufferManager fileDownlinkBufferManager("fileDownlinkBufferManager", DOWNLINK_BUFFER_STORE_SIZE, DOWNLINK_BUFFER_QUEUE_SIZE);
Svc::BufferManager fileUplinkBufferManager("fileUplinkBufferManager", UPLINK_BUFFER_STORE_SIZE, UPLINK_BUFFER_QUEUE_SIZE);
Svc::HealthImpl health("health");

Drv::SocketIpDriverComponentImpl socketIpDriver("SocketIpDriver");

Svc::AssertFatalAdapterComponentImpl fatalAdapter
#if FW_OBJECT_NAMES == 1
("fatalAdapter")
#endif
;

Svc::FatalHandlerComponentImpl fatalHandler
#if FW_OBJECT_NAMES == 1
("fatalHandler")
#endif
;


#if FW_OBJECT_REGISTRATION == 1

void dumparch(void) {
    simpleReg.dump();
}

#if FW_OBJECT_NAMES == 1
void dumpobj(const char* objName) {
    simpleReg.dump(objName);
}
#endif

#endif

void constructApp(int port_number, char* hostname, char* device) {
    bool uart_connected = false;
#if FW_PORT_TRACING
    Fw::PortBase::setTrace(false);
#endif    

    // Initialize rate group driver
    rateGroupDriverComp.init();

    // Initialize the rate groups
    rateGroup1Comp.init(10,0);
    
    rateGroup2Comp.init(10,1);
    
    rateGroup3Comp.init(10,2);
    //GPS-- Here we initialize the component with a queue size, and instance number. The queue size governs how
    //      many waiting port calls can queue up before the system asserts, and the instance number is a unique
    //      number given to every instance of a given type.
    gpsSerial.init(1);
    gpsImpl.init(10, 1);
#if FW_ENABLE_TEXT_LOGGING
    textLogger.init();
#endif

    eventLogger.init(10,0);

    linuxTime.init(0);

    chanTlm.init(10,0);

    cmdDisp.init(20,0);

    cmdSeq.init(10,0);
    cmdSeq.allocateBuffer(0,seqMallocator,5*1024);

    prmDb.init(10,0);

    groundIf.init(0);
    socketIpDriver.init(0);

    fileUplink.init(30, 0);
    fileDownlink.init(30, 0);
    fileUplinkBufferManager.init(0);
    fileDownlinkBufferManager.init(1);
    fatalAdapter.init(0);
    fatalHandler.init(0);
    health.init(25,0);
    // Connect rate groups to rate group driver
    constructGpsAppArchitecture();

    /* Register commands */
    cmdSeq.regCommands();
    cmdDisp.regCommands();
    eventLogger.regCommands();
    prmDb.regCommands();
    fileDownlink.regCommands();
    health.regCommands();
    gpsImpl.regCommands();

    // read parameters
    prmDb.readParamFile();

    // set health ping entries

    Svc::HealthImpl::PingEntry pingEntries[] = {
        {3,5,rateGroup1Comp.getObjName()}, // 0
        {3,5,rateGroup2Comp.getObjName()}, // 1
        {3,5,rateGroup3Comp.getObjName()}, // 2
        {3,5,cmdDisp.getObjName()}, // 3
        {3,5,eventLogger.getObjName()}, // 4
        {3,5,cmdSeq.getObjName()}, // 5
        {3,5,chanTlm.getObjName()}, // 6
        {3,5,fileUplink.getObjName()}, // 7
        {3,5,fileDownlink.getObjName()}, // 8
    };
    if (!gpsSerial.open(device,
                   Drv::LinuxSerialDriverComponentImpl::BAUD_9600,
                   Drv::LinuxSerialDriverComponentImpl::NO_FLOW, 
                   Drv::LinuxSerialDriverComponentImpl::PARITY_NONE,
                   true))
    {
        Fw::Logger::logMsg("[ERROR] Failed to open GPS UART: %s\n", reinterpret_cast<POINTER_CAST>(device));
    } else {
        Fw::Logger::logMsg("[INFO] Opened GPS UART: %s\n", reinterpret_cast<POINTER_CAST>(device));
        uart_connected = true;
    }
    // register ping table
    health.setPingEntries(pingEntries,FW_NUM_ARRAY_ELEMENTS(pingEntries),0x123);

    // Active component startup
    // start rate groups
    rateGroup1Comp.start(ACTIVE_COMP_1HZ_RG, 120,10 * 1024);
    rateGroup2Comp.start(ACTIVE_COMP_P5HZ_RG, 119,10 * 1024);
    rateGroup3Comp.start(ACTIVE_COMP_P25HZ_RG, 118,10 * 1024);
    // start dispatcher
    cmdDisp.start(ACTIVE_COMP_CMD_DISP,101,10*1024);
    // start sequencer
    cmdSeq.start(ACTIVE_COMP_CMD_SEQ,100,10*1024);
    // start telemetry
    eventLogger.start(ACTIVE_COMP_LOGGER,98,10*1024);
    chanTlm.start(ACTIVE_COMP_TLM,97,10*1024);
    prmDb.start(ACTIVE_COMP_PRMDB,96,10*1024);
    //GPS-- GPS thread starting. The GPS component is active, so its governing thread must be started
    //      with the unique id, defined above, a priority 256 (highest) - 0 (lowest) set here to 99, and
    //      a stack size for the thread, here 10KB is used.
    gpsImpl.start(ACTIVE_COMP_GPS, 95, 10*1024);
    if (uart_connected) {
        gpsSerial.startReadThread(94, 20 * 1024);
    }
    fileDownlink.start(ACTIVE_COMP_FILE_DOWNLINK, 100, 10*1024);
    fileUplink.start(ACTIVE_COMP_FILE_UPLINK, 100, 10*1024);

    // Initialize socket server if and only if there is a valid specification
    if (hostname != NULL && port_number != 0) {
        socketIpDriver.startSocketTask(100, 10 * 1024, hostname, port_number);
    }
}
//GPS-- Given the application's lack of a specific timing element, we
//      force a call to the rate group driver every second here.
//      More complex applications may drive this from a system oscillator.
void run1cycle(void) {
    // get timer to call rate group driver
    Svc::TimerVal timer;
    timer.take();
    rateGroupDriverComp.get_CycleIn_InputPort(0)->invoke(timer);
    Os::Task::TaskStatus delayStat = Os::Task::delay(1000);
    FW_ASSERT(Os::Task::TASK_OK == delayStat,delayStat);
}



void runcycles(NATIVE_INT_TYPE cycles) {
    if (cycles == -1) {
        while (true) {
            run1cycle();
        }
    }

    for (NATIVE_INT_TYPE cycle = 0; cycle < cycles; cycle++) {
        run1cycle();
    }

}

void exitTasks(void) {
    rateGroup1Comp.exit();
    rateGroup2Comp.exit();
    rateGroup3Comp.exit();
    cmdDisp.exit();
    eventLogger.exit();
    chanTlm.exit();
    prmDb.exit();
    fileUplink.exit();
    fileDownlink.exit();
    cmdSeq.exit();
}

