 \mainpage

This is the F´ automatically generated documentation. 
The sidebar links point to the software description documentation (SDD) for the F´ components. 
The Namespace and Classes sections link to the documentation of the C++ code. 



\page Fw Fw Components, Ports, and Classes

The Fw package provides the core classes, components, and ports to support F´. These classes allow
for the core framework operation. In addition, these include the base classes on which components
are built.  Finally, of specific interest is Fw::Types providing the cored types in the system.

\subpage FwBufferSerializableBufferGetBufferSend

\subpage FwCmdFwCmdResponseFwCmdReg

\subpage FwComPort

\subpage FwFilePacketClasses

\subpage FwLogLogText

\subpage FwObjClasses

\subpage FwPortClasses

\subpage FwPrmGetPrmSet

\subpage FwTimePort

\subpage FwTlmPort

\subpage FwTypeClasses


\page Os Os Components Ports and Classes

The Os package is an operating system abstraction layer. It provides basic Os system functions to
F´. This includes things like Mutexes, Queues, Tasks, and a File System.

\subpage OsQueue


\page Svc Svc Components, Ports, and Classes

The Svc package provides standard components to support greater F´ applications. This includes
command and data handling components used for more complete applications.

\subpage SvcAMPCSSequenceClass

\subpage SvcActiveLoggerComponent

\subpage SvcActiveRateGroupComponent

\subpage SvcActiveTextLoggerComponent

\subpage SvcAssertFatalAdapterComponent

\subpage SvcBufferAccumulatorComponent

\subpage SvcBufferManagerComponent

\subpage SvcBufferRepeaterComponent

\subpage SvcCmdDispatcherComponent

\subpage SvcCmdSequencerComponent

\subpage SvcCmdSequencerFormats

\subpage SvcCmdSplitter

\subpage SvcComLoggerComponent

\subpage SvcComQueueComponent

\subpage SvcComSplitterComponent

\subpage SvcComStubComponent

\subpage SvcDeframerComponent

\subpage SvcFatalHandlerComponent

\subpage SvcFatalPort

\subpage SvcFileDownlinkComponent

\subpage SvcFileManagerComponent

\subpage SvcFileUplinkComponent

\subpage SvcFramerComponent

\subpage SvcFramingProtocol

\subpage SvcGenericHubComponent

\subpage SvcHealthComponent

\subpage SvcPassiveConsoleTextLoggerComponent

\subpage SvcPingPort

\subpage SvcPolyDbComponent

\subpage SvcPolyPort

\subpage SvcPosixTime

\subpage SvcPrmDbComponent

\subpage SvcPassiveRateGroupComponent

\subpage SvcRateGroupDriverComponent

\subpage SvcSchedPort

\subpage SvcStaticMemoryComponent

\subpage SvcSystemResourcesComponent

\subpage SvcTlmChanComponent

\subpage SvcTlmPacketizerComponent

\subpage SvcTlmPacketizerComponentDictionary

\subpage SvcWatchDogPort


\page Drv Drv Components and Classes

The Drv package provides drivers to support various hardware functions for F´. These drivers 
include a sample block driver component as well a various drivers that support Linux hardware
functions.

\subpage DrvBlockDriverComponent

\subpage DrvByteStreamDriverModel

\subpage DrvIp

\subpage DrvStreamCrossover

\subpage DrvTcpClient

\subpage DrvTcpServer

\subpage DrvUdp



\page Utils Utils Classes

The Utils package provides classes for general applications in F´ projects. These classes are for
a variety of purposes.

\subpage UtilsHashClass

\subpage UtilsLockGuardClass

\subpage UtilsRateLimiterClass

\subpage UtilsTokenBucketClass

\subpage UtilsTypesLibrary
