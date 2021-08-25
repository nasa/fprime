#include <Components.hpp>
#include <Fw/Types/Assert.hpp>
#include <Os/Task.hpp>
#include <Fw/Logger/Logger.hpp>
#include <Os/Log.hpp>
#include <Fw/Types/MallocAllocator.hpp>

#include <Svc/FramingProtocol/FprimeProtocol.hpp>

#if defined TGT_OS_TYPE_LINUX || TGT_OS_TYPE_DARWIN
#include <getopt.h>
#include <stdlib.h>
#include <ctype.h>
#endif

// List of context IDs
enum {
    UPLINK_BUFFER_STORE_SIZE = 3000,
    UPLINK_BUFFER_QUEUE_SIZE = 30,
    UPLINK_BUFFER_MGR_ID = 200
};

Os::Log osLogger;
Svc::FprimeDeframing deframing;
Svc::FprimeFraming framing;

// Registry
#if FW_OBJECT_REGISTRATION == 1
static Fw::SimpleObjRegistry simpleReg;
#endif

// Component instance pointers
static NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriverImpl::DIVIDER_SIZE] = {1,2,4};
Svc::RateGroupDriverImpl rateGroupDriverComp(FW_OPTIONAL_NAME("RGDvr"),rgDivs,FW_NUM_ARRAY_ELEMENTS(rgDivs));

static NATIVE_UINT_TYPE rg1Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup1Comp(FW_OPTIONAL_NAME("RG1"),rg1Context,FW_NUM_ARRAY_ELEMENTS(rg1Context));

static NATIVE_UINT_TYPE rg2Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup2Comp(FW_OPTIONAL_NAME("RG2"),rg2Context,FW_NUM_ARRAY_ELEMENTS(rg2Context));

static NATIVE_UINT_TYPE rg3Context[] = {0,0,0,0,0,0,0,0,0,0};
Svc::ActiveRateGroupImpl rateGroup3Comp(FW_OPTIONAL_NAME("RG3"),rg3Context,FW_NUM_ARRAY_ELEMENTS(rg3Context));

// Driver Component
Drv::BlockDriverImpl blockDrv(FW_OPTIONAL_NAME("BDRV"));

// Reference Implementation Components

Ref::RecvBuffImpl recvBuffComp(FW_OPTIONAL_NAME("RBC"));

Ref::SendBuffImpl sendBuffComp(FW_OPTIONAL_NAME("SBC"));

#if FW_ENABLE_TEXT_LOGGING
Svc::ConsoleTextLoggerImpl textLogger(FW_OPTIONAL_NAME("TLOG"));
#endif

Svc::ActiveLoggerImpl eventLogger(FW_OPTIONAL_NAME("ELOG"));

Svc::LinuxTimeImpl linuxTime(FW_OPTIONAL_NAME("LTIME"));

Svc::TlmChanImpl chanTlm(FW_OPTIONAL_NAME("TLM"));

Svc::TlmPacketizer pktTlm(FW_OPTIONAL_NAME("PTLM"));

Svc::CommandDispatcherImpl cmdDisp(FW_OPTIONAL_NAME("CMDDISP"));

Fw::MallocAllocator mallocator;
Svc::CmdSequencerComponentImpl cmdSeq(FW_OPTIONAL_NAME("CMDSEQ"));

Svc::PrmDbImpl prmDb(FW_OPTIONAL_NAME("PRM"),"PrmDb.dat");

Ref::PingReceiverComponentImpl pingRcvr(FW_OPTIONAL_NAME("PngRecv"));

Drv::TcpClientComponentImpl comm(FW_OPTIONAL_NAME("Tcp"));

Svc::FileUplink fileUplink(FW_OPTIONAL_NAME("fileUplink"));

Svc::FileDownlink fileDownlink(FW_OPTIONAL_NAME("fileDownlink"));

Svc::FileManager fileManager(FW_OPTIONAL_NAME("fileManager"));

Svc::BufferManagerComponentImpl fileUplinkBufferManager(FW_OPTIONAL_NAME("fileUplinkBufferManager"));

Svc::HealthImpl health(FW_OPTIONAL_NAME("health"));

Ref::SignalGen SG1(FW_OPTIONAL_NAME("signalGen1"));

Ref::SignalGen SG2(FW_OPTIONAL_NAME("signalGen2"));

Ref::SignalGen SG3(FW_OPTIONAL_NAME("signalGen3"));

Ref::SignalGen SG4(FW_OPTIONAL_NAME("signalGen4"));

Ref::SignalGen SG5(FW_OPTIONAL_NAME("signalGen5"));

Svc::AssertFatalAdapterComponentImpl fatalAdapter(FW_OPTIONAL_NAME("fatalAdapter"));

Svc::FatalHandlerComponentImpl fatalHandler(FW_OPTIONAL_NAME("fatalHandler"));

Svc::StaticMemoryComponentImpl staticMemory(FW_OPTIONAL_NAME("staticMemory"));

Svc::FramerComponentImpl downlink(FW_OPTIONAL_NAME("downlink"));

Svc::DeframerComponentImpl uplink(FW_OPTIONAL_NAME("uplink"));

const char* getHealthName(Fw::ObjBase& comp) {
   #if FW_OBJECT_NAMES == 1
       return comp.getObjName();
   #else
      return "[no object name]"
   #endif
}

bool constructApp(bool dump, U32 port_number, char* hostname) {

#if FW_PORT_TRACING
    Fw::PortBase::setTrace(false);
#endif
    staticMemory.init(0);
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
    pktTlm.init(10,0);

    cmdDisp.init(20,0);

    cmdSeq.init(10,0);
    cmdSeq.allocateBuffer(0,mallocator,5*1024);

    prmDb.init(10,0);

    comm.init(0);
    downlink.init(0);
    uplink.init(0);
    fileUplink.init(30, 0);
    fileDownlink.init(30, 0);
    fileDownlink.configure(1000, 1000, 1000, 10);
    fileManager.init(30, 0);
    fileUplinkBufferManager.init(0);
    SG1.init(10,0);
    SG2.init(10,1);
    SG3.init(10,2);
    SG4.init(10,3);
    SG5.init(10,4);
    fatalAdapter.init(0);
    fatalHandler.init(0);
    health.init(25,0);
    pingRcvr.init(10);

    downlink.setup(framing);
    uplink.setup(deframing);

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
    fileManager.regCommands();
    SG1.regCommands();
    SG2.regCommands();
    SG3.regCommands();
    SG4.regCommands();
    SG5.regCommands();
    health.regCommands();
    pingRcvr.regCommands();
    pktTlm.regCommands();

    // read parameters
    prmDb.readParamFile();
    recvBuffComp.loadParameters();
    sendBuffComp.loadParameters();

    // set up BufferManager instances
    Svc::BufferManagerComponentImpl::BufferBins upBuffMgrBins;
    memset(&upBuffMgrBins,0,sizeof(upBuffMgrBins));
    upBuffMgrBins.bins[0].bufferSize = UPLINK_BUFFER_STORE_SIZE;
    upBuffMgrBins.bins[0].numBuffers = UPLINK_BUFFER_QUEUE_SIZE;
    fileUplinkBufferManager.setup(UPLINK_BUFFER_MGR_ID,0,mallocator,upBuffMgrBins);

    // set health ping entries

    Svc::HealthImpl::PingEntry pingEntries[] = {
        {3,5,getHealthName(rateGroup1Comp)}, // 0
        {3,5,getHealthName(rateGroup2Comp)}, // 1
        {3,5,getHealthName(rateGroup3Comp)}, // 2
        {3,5,getHealthName(cmdDisp)}, // 3
        {3,5,getHealthName(eventLogger)}, // 4
        {3,5,getHealthName(cmdSeq)}, // 5
        {3,5,getHealthName(chanTlm)}, // 6
        {3,5,getHealthName(prmDb)}, // 7
        {3,5,getHealthName(fileUplink)}, // 8
        {3,5,getHealthName(fileDownlink)}, // 9
        {3,5,getHealthName(pingRcvr)}, // 10
        {3,5,getHealthName(blockDrv)}, // 11
        {3,5,getHealthName(fileManager)}, // 12
        {3,5,getHealthName(pktTlm)}, // 13
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
    pktTlm.start(0,97,10*1024);
    prmDb.start(0,96,10*1024);

    fileDownlink.start(0, 100, 10*1024);
    fileUplink.start(0, 100, 10*1024);
    fileManager.start(0, 100, 10*1024);

    pingRcvr.start(0, 100, 10*1024);



    // Initialize socket server if and only if there is a valid specification
    if (hostname != NULL && port_number != 0) {
        Os::TaskString name("ReceiveTask");
        // Uplink is configured for receive so a socket task is started
        comm.configure(hostname, port_number);
        comm.startSocketTask(name, 100, 10 * 1024);
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
    pktTlm.exit();
    prmDb.exit();
    fileUplink.exit();
    fileDownlink.exit();
    fileManager.exit();
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
    (void) pktTlm.ActiveComponentBase::join(NULL);
    (void) prmDb.ActiveComponentBase::join(NULL);
    (void) fileUplink.ActiveComponentBase::join(NULL);
    (void) fileDownlink.ActiveComponentBase::join(NULL);
    (void) fileManager.ActiveComponentBase::join(NULL);
    (void) cmdSeq.ActiveComponentBase::join(NULL);
    (void) pingRcvr.ActiveComponentBase::join(NULL);
    comm.stopSocketTask();
    (void) comm.joinSocketTask(NULL);
    cmdSeq.deallocateBuffer(mallocator);
    fileUplinkBufferManager.cleanup();
}

