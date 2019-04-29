#ifndef __RPI_COMPONENTS_HEADER__
#define __RPI_COMPONENTS_HEADER__

void constructRPIArchitecture(void);
void exitTasks(void);
void constructApp(int port_number, char* hostname);

#include <Svc/ActiveRateGroup/ActiveRateGroupImpl.hpp>
#include <Svc/RateGroupDriver/RateGroupDriverImpl.hpp>

#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>
#include <Svc/CmdSequencer/CmdSequencerImpl.hpp>
#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>
#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>
#include <Svc/LinuxTime/LinuxTimeImpl.hpp>
#include <Svc/LinuxTimer/LinuxTimerComponentImpl.hpp>
#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Svc/FileUplink/FileUplink.hpp>
#include <Svc/FileDownlink/FileDownlink.hpp>
#include <Svc/BufferManager/BufferManager.hpp>
#include <Svc/Health/HealthComponentImpl.hpp>

#include <Svc/SocketGndIf/SvcSocketGndIfImpl.hpp>

#include <Svc/AssertFatalAdapter/AssertFatalAdapterComponentImpl.hpp>
#include <Svc/FatalHandler/FatalHandlerComponentImpl.hpp>

// Drivers

#include <Drv/LinuxSerialDriver/LinuxSerialDriverComponentImpl.hpp>
#include <Drv/LinuxSpiDriver/LinuxSpiDriverComponentImpl.hpp>
#include <Drv/LinuxGpioDriver/LinuxGpioDriverComponentImpl.hpp>

// Main app
#include <RPI/RpiDemo/RpiDemoComponentImpl.hpp>

extern Svc::RateGroupDriverImpl rateGroupDriverComp;
extern Svc::ActiveRateGroupImpl rateGroup10HzComp;
extern Svc::ActiveRateGroupImpl rateGroup1HzComp;
extern Svc::CmdSequencerComponentImpl cmdSeq;
extern Svc::SocketGndIfImpl sockGndIf;
extern Svc::ConsoleTextLoggerImpl textLogger;
extern Svc::ActiveLoggerImpl eventLogger;
extern Svc::LinuxTimeImpl linuxTime;
extern Svc::LinuxTimerComponentImpl linuxTimer;
extern Svc::TlmChanImpl chanTlm;
extern Svc::CommandDispatcherImpl cmdDisp;
extern Svc::PrmDbImpl prmDb;
extern Svc::FileUplink fileUplink;
extern Svc::FileDownlink fileDownlink;
extern Svc::BufferManager fileDownlinkBufferManager;
extern Svc::BufferManager fileUplinkBufferManager;
extern Svc::AssertFatalAdapterComponentImpl fatalAdapter;
extern Svc::FatalHandlerComponentImpl fatalHandler;
extern Svc::HealthImpl health;

extern Drv::LinuxSerialDriverComponentImpl uartDrv;
extern Drv::LinuxSpiDriverComponentImpl spiDrv;
extern Drv::LinuxGpioDriverComponentImpl ledDrv;
extern Drv::LinuxGpioDriverComponentImpl gpio23Drv;
extern Drv::LinuxGpioDriverComponentImpl gpio24Drv;
extern Drv::LinuxGpioDriverComponentImpl gpio25Drv;
extern Drv::LinuxGpioDriverComponentImpl gpio17Drv;

extern Rpi::RpiDemoComponentImpl rpiDemo;



#endif
