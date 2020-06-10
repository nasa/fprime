#include <Components.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Task.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Os/Log.hpp>
#include <Fw/Types/MallocAllocator.hpp>

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#endif

// List of context IDs
enum {
    DOWNLINK_PACKET_SIZE = 500,
    DOWNLINK_BUFFER_STORE_SIZE = 2500,
    DOWNLINK_BUFFER_QUEUE_SIZE = 5,
    UPLINK_BUFFER_STORE_SIZE = 3000,
    UPLINK_BUFFER_QUEUE_SIZE = 30
};

Os::Log osLogger;


// Registry
#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

// Component instance pointers
static NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriverImpl::DIVIDER_SIZE] = {1,2,4};
Svc::RateGroupDriverImpl rateGroupDriverComp("RGDvr",rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

static NATIVE_UINT_TYPE rg1Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup1Comp("RG1",rg1Context,FW_NUM_ARRAY_ELEMENTS(rg1Context));

static NATIVE_UINT_TYPE rg2Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup2Comp("RG2",rg2Context,FW_NUM_ARRAY_ELEMENTS(rg2Context));

static NATIVE_UINT_TYPE rg3Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup3Comp("RG3",rg3Context,FW_NUM_ARRAY_ELEMENTS(rg3Context));

// Command Components
Svc::GroundInterfaceComponentImpl groundIf("GNDIF");

// Driver Component
Drv::BlockDriverImpl blockDrv("BDRV");

// Reference Implementation Components

Ref::RecvBuffImpl recvBuffComp("RBC");

Ref::SendBuffImpl sendBuffComp("SBC");

#if FW_ENABLE_TEXT_LOGGING
Svc::ConsoleTextLoggerImpl textLogger("TLOG");
#endif

Svc::ActiveLoggerImpl eventLogger("ELOG");

Svc::LinuxTimeImpl linuxTime("LTIME");

Svc::TlmChanImpl chanTlm("TLM");

Svc::CommandDispatcherImpl cmdDisp("CMDDISP");

Fw::MallocAllocator seqMallocator;
Svc::CmdSequencerComponentImpl cmdSeq("CMDSEQ");

Svc::PrmDbImpl prmDb("PRM","PrmDb.dat");

Ref::PingReceiverComponentImpl pingRcvr("PngRecv");

Drv::SocketIpDriverComponentImpl socketIpDriver("SocketIpDriver");

Svc::FileUplink fileUplink ("fileUplink");

Svc::FileDownlink fileDownlink ("fileDownlink", DOWNLINK_PACKET_SIZE);

Svc::BufferManager fileDownlinkBufferManager("fileDownlinkBufferManager", DOWNLINK_BUFFER_STORE_SIZE, DOWNLINK_BUFFER_QUEUE_SIZE);

Svc::BufferManager fileUplinkBufferManager("fileUplinkBufferManager", UPLINK_BUFFER_STORE_SIZE, UPLINK_BUFFER_QUEUE_SIZE);

Svc::HealthImpl health("health");

Ref::SignalGen SG1("signalGen1");

Ref::SignalGen SG2("signalGen2");

Ref::SignalGen SG3("signalGen3");

Ref::SignalGen SG4("signalGen4");

Ref::SignalGen SG5("signalGen5");

Svc::AssertFatalAdapterComponentImpl fatalAdapter("fatalAdapter");

Svc::FatalHandlerComponentImpl fatalHandler("fatalHandler");

bool constructApp(bool dump, U32 port_number, char* hostname) {

#if FW_PORT_TRACING
    Fw::PortBase::setTrace(false);
#endif    

    // Initialize rate group driver
    rateGroupDriverComp.init();

    // Initialize the rate groups
    rateGroup1Comp.init(10,0);
    
    rateGroup2Comp.init(10,1);
    
    rateGroup3Comp.init(10,2);

    // Initialize block driver
    blockDrv.init(10);

    // Send/Receive example hardware components
    recvBuffComp.init();
    sendBuffComp.init(10);

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
    SG1.init(10,0);
	SG2.init(10,1);
	SG3.init(10,2);
	SG4.init(10,3);
	SG5.init(10,4);
	fatalAdapter.init(0);
	fatalHandler.init(0);
	health.init(25,0);
	pingRcvr.init(10);
    // Connect rate groups to rate group driver
    constructRefArchitecture();

    // dump topology if requested
    if (dump) {
#if FW_OBJECT_REGISTRATION == 1
        simpleReg.dump();
#endif
        return true;
    }

    /* Register commands */
    sendBuffComp.regCommands();
    recvBuffComp.regCommands();
    cmdSeq.regCommands();
    cmdDisp.regCommands();
    eventLogger.regCommands();
    prmDb.regCommands();
    fileDownlink.regCommands();
    SG1.regCommands();
    SG2.regCommands();
    SG3.regCommands();
    SG4.regCommands();
	SG5.regCommands();
	health.regCommands();
	pingRcvr.regCommands();

    // read parameters
    prmDb.readParamFile();
    recvBuffComp.loadParameters();
    sendBuffComp.loadParameters();

    // set health ping entries

    Svc::HealthImpl::PingEntry pingEntries[] = {
        {3,5,rateGroup1Comp.getObjName()}, // 0
        {3,5,rateGroup2Comp.getObjName()}, // 1
        {3,5,rateGroup3Comp.getObjName()}, // 2
        {3,5,cmdDisp.getObjName()}, // 3
        {3,5,eventLogger.getObjName()}, // 4
        {3,5,cmdSeq.getObjName()}, // 5
        {3,5,chanTlm.getObjName()}, // 6
        {3,5,prmDb.getObjName()}, // 7
        {3,5,fileUplink.getObjName()}, // 8
        {3,5,fileDownlink.getObjName()}, // 9
        {3,5,pingRcvr.getObjName()}, // 10
        {3,5,blockDrv.getObjName()}, // 11
    };

    // register ping table
    health.setPingEntries(pingEntries,FW_NUM_ARRAY_ELEMENTS(pingEntries),0x123);

    // Active component startup
    // start rate groups
    rateGroup1Comp.start(0, 120,10 * 1024);
    rateGroup2Comp.start(0, 119,10 * 1024);
    rateGroup3Comp.start(0, 118,10 * 1024);
    // start driver
    blockDrv.start(0,140,10*1024);
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

    pingRcvr.start(0, 100, 10*1024);

    // Initialize socket server if and only if there is a valid specification
    if (hostname != NULL && port_number != 0) {
        socketIpDriver.startSocketTask(100, 10 * 1024, hostname, port_number);
    }
    return false;
}

void exitTasks(void) {
    rateGroup1Comp.exit();
    rateGroup2Comp.exit();
    rateGroup3Comp.exit();
    blockDrv.exit();
    cmdDisp.exit();
    eventLogger.exit();
    chanTlm.exit();
    prmDb.exit();
    fileUplink.exit();
    fileDownlink.exit();
    cmdSeq.exit();
    pingRcvr.exit();
    // join the component threads with NULL pointers to free them
    (void) rateGroup1Comp.ActiveComponentBase::join(NULL);
    (void) rateGroup2Comp.ActiveComponentBase::join(NULL);
    (void) rateGroup3Comp.ActiveComponentBase::join(NULL);
    (void) blockDrv.ActiveComponentBase::join(NULL);
    (void) cmdDisp.ActiveComponentBase::join(NULL);
    (void) eventLogger.ActiveComponentBase::join(NULL);
    (void) chanTlm.ActiveComponentBase::join(NULL);
    (void) prmDb.ActiveComponentBase::join(NULL);
    (void) fileUplink.ActiveComponentBase::join(NULL);
    (void) fileDownlink.ActiveComponentBase::join(NULL);
    (void) cmdSeq.ActiveComponentBase::join(NULL);
    (void) pingRcvr.ActiveComponentBase::join(NULL);
    socketIpDriver.exitSocketTask();
    (void) socketIpDriver.joinSocketTask(NULL);
    cmdSeq.deallocateBuffer(seqMallocator);
}

