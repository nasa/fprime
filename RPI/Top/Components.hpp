#ifndef __RPI_COMPONENTS_HEADER__
#define __RPI_COMPONENTS_HEADER__

#include <Svc/ActiveRateGroup/ActiveRateGroupImpl.hpp>
#include <Svc/RateGroupDriver/RateGroupDriverImpl.hpp>

#include <Svc/CmdDispatcher/CommandDispatcherImpl.hpp>
#include <Svc/CmdSequencer/CmdSequencerImpl.hpp>
#include <Svc/PassiveConsoleTextLogger/ConsoleTextLoggerImpl.hpp>
#include <Svc/ActiveLogger/ActiveLoggerImpl.hpp>
#include <Svc/PosixTime/PosixTime.hpp>
#include <Svc/LinuxTimer/LinuxTimerComponentImpl.hpp>
#include <Svc/TlmChan/TlmChanImpl.hpp>
#include <Svc/PrmDb/PrmDbImpl.hpp>
#include <Fw/Obj/SimpleObjRegistry.hpp>
#include <Svc/FileUplink/FileUplink.hpp>
#include <Svc/FileDownlink/FileDownlink.hpp>
#include <Svc/BufferManager/BufferManagerComponentImpl.hpp>
#include <Svc/Health/HealthComponentImpl.hpp>
#include <Svc/StaticMemory/StaticMemoryComponentImpl.hpp>
#include <Svc/Framer/FramerComponentImpl.hpp>
#include <Svc/Deframer/DeframerComponentImpl.hpp>
#include <Drv/TcpClient/TcpClientComponentImpl.hpp>
#include <Svc/AssertFatalAdapter/AssertFatalAdapterComponentImpl.hpp>
#include <Svc/FatalHandler/FatalHandlerComponentImpl.hpp>

// Drivers

#include <Drv/LinuxSerialDriver/LinuxSerialDriverComponentImpl.hpp>
#include <Drv/LinuxSpiDriver/LinuxSpiDriverComponentImpl.hpp>
#include <Drv/LinuxGpioDriver/LinuxGpioDriverComponentImpl.hpp>
#include <Drv/TcpClient/TcpClientComponentImpl.hpp>
#include <Drv/Udp/UdpComponentImpl.hpp>

// Main app
#include <RPI/RpiDemo/RpiDemoComponentImpl.hpp>

void constructRPIArchitecture();
void exitTasks();
void constructApp(U32 port_number, char* hostname);

extern Svc::RateGroupDriverImpl rateGroupDriverComp;
extern Svc::ActiveRateGroupImpl rateGroup10HzComp;
extern Svc::ActiveRateGroupImpl rateGroup1HzComp;
extern Svc::CmdSequencerComponentImpl cmdSeq;
extern Svc::ConsoleTextLoggerImpl textLogger;
extern Svc::ActiveLoggerImpl eventLogger;
extern Svc::PosixTime posixTime;
extern Svc::LinuxTimerComponentImpl linuxTimer;
extern Svc::TlmChanImpl chanTlm;
extern Svc::CommandDispatcherImpl cmdDisp;
extern Svc::PrmDbImpl prmDb;
extern Svc::FileUplink fileUplink;
extern Svc::FileDownlink fileDownlink;
extern Svc::BufferManagerComponentImpl fileUplinkBufferManager;
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

extern Svc::StaticMemoryComponentImpl staticMemory;
extern Drv::TcpClientComponentImpl comm;
extern Svc::FramerComponentImpl downlink;
extern Svc::DeframerComponentImpl uplink;
#endif
