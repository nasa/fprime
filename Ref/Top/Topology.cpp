#include "RefTopologyAc.hpp"

#include "Components.hpp"
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

namespace Ref {

  namespace {

    // ----------------------------------------------------------------------
    // Component configuration constants 
    // ----------------------------------------------------------------------

    namespace ConfigConstants {

      // TODO: Look for compile-time constants being declared and used.
      // Move their declarations here.

    }

    // ----------------------------------------------------------------------
    // Component configuration objects
    // ----------------------------------------------------------------------

    namespace ConfigObjects {

      // TODO: Look for objects being declared and used.
      // Move their declarations here.

      namespace health {
        Svc::HealthImpl::PingEntry pingEntries[] = {
          {
            PingEntries::blockDrv::WARN,
            PingEntries::blockDrv::FATAL,
            "blockDrv"
          },
#if 0
          // TODO: Define these symbols in RefTopologyDefs.hpp
          {
            PingEntries::chanTlm::WARN,
            PingEntries::chanTlm::FATAL,
            "chanTlm"
          },
          {
            PingEntries::cmdDisp::WARN,
            PingEntries::cmdDisp::FATAL,
            "cmdDisp"
          },
          {
            PingEntries::cmdSeq::WARN,
            PingEntries::cmdSeq::FATAL,
            "cmdSeq"
          },
          {
            PingEntries::eventLogger::WARN,
            PingEntries::eventLogger::FATAL,
            "eventLogger"
          },
          {
            PingEntries::fileDownlink::WARN,
            PingEntries::fileDownlink::FATAL,
            "fileDownlink"
          },
          {
            PingEntries::fileManager::WARN,
            PingEntries::fileManager::FATAL,
            "fileManager"
          },
          {
            PingEntries::fileUplink::WARN,
            PingEntries::fileUplink::FATAL,
            "fileUplink"
          },
          {
            PingEntries::pingRcvr::WARN,
            PingEntries::pingRcvr::FATAL,
            "pingRcvr"
          },
          {
            PingEntries::prmDb::WARN,
            PingEntries::prmDb::FATAL,
            "prmDb"
          },
          {
            PingEntries::rateGroup1Comp::WARN,
            PingEntries::rateGroup1Comp::FATAL,
            "rateGroup1Comp"
          },
          {
            PingEntries::rateGroup2Comp::WARN,
            PingEntries::rateGroup2Comp::FATAL,
            "rateGroup2Comp"
          },
          {
            PingEntries::rateGroup3Comp::WARN,
            PingEntries::rateGroup3Comp::FATAL,
            "rateGroup3Comp"
          },
#endif
        };
      }

    }

    // ----------------------------------------------------------------------
    // Component instances
    // ----------------------------------------------------------------------

#if 0
    // TODO: Move component instance definitions here.

    // blockDrv
    Drv::BlockDriver blockDrv(FW_OPTIONAL_NAME("blockDrv"));

    // rateGroup1Comp
    Svc::ActiveRateGroup rateGroup1Comp(FW_OPTIONAL_NAME("rateGroup1Comp"));

    // rateGroup2Comp
    Svc::ActiveRateGroup rateGroup2Comp(FW_OPTIONAL_NAME("rateGroup2Comp"));

    // rateGroup3Comp
    Svc::ActiveRateGroup rateGroup3Comp(FW_OPTIONAL_NAME("rateGroup3Comp"));

    // cmdDisp
    Svc::CommandDispatcher cmdDisp(FW_OPTIONAL_NAME("cmdDisp"));

    // cmdSeq
    Svc::CmdSequencer cmdSeq(FW_OPTIONAL_NAME("cmdSeq"));

    // fileDownlink
    Svc::FileDownlink fileDownlink(FW_OPTIONAL_NAME("fileDownlink"));

    // fileManager
    Svc::FileManager fileManager(FW_OPTIONAL_NAME("fileManager"));

    // fileUplink
    Svc::FileUplink fileUplink(FW_OPTIONAL_NAME("fileUplink"));

    // pingRcvr
    PingReceiver pingRcvr(FW_OPTIONAL_NAME("pingRcvr"));

    // eventLogger
    Svc::ActiveLogger eventLogger(FW_OPTIONAL_NAME("eventLogger"));

    // chanTlm
    Svc::TlmChan chanTlm(FW_OPTIONAL_NAME("chanTlm"));

    // prmDb
    Svc::PrmDb prmDb(FW_OPTIONAL_NAME("prmDb"));

    // health
    Svc::Health health(FW_OPTIONAL_NAME("health"));

    // SG1
    SignalGen SG1(FW_OPTIONAL_NAME("SG1"));

    // SG2
    SignalGen SG2(FW_OPTIONAL_NAME("SG2"));

    // SG3
    SignalGen SG3(FW_OPTIONAL_NAME("SG3"));

    // SG4
    SignalGen SG4(FW_OPTIONAL_NAME("SG4"));

    // SG5
    SignalGen SG5(FW_OPTIONAL_NAME("SG5"));

    // sendBuffComp
    SendBuff sendBuffComp(FW_OPTIONAL_NAME("sendBuffComp"));

    // comm
    Drv::ByteStreamDriverModel comm(FW_OPTIONAL_NAME("comm"));

    // downlink
    Svc::Framer downlink(FW_OPTIONAL_NAME("downlink"));

    // fatalAdapter
    Svc::AssertFatalAdapter fatalAdapter(FW_OPTIONAL_NAME("fatalAdapter"));

    // fatalHandler
    Svc::FatalHandler fatalHandler(FW_OPTIONAL_NAME("fatalHandler"));

    // fileUplinkBufferManager
    Svc::BufferManager fileUplinkBufferManager(FW_OPTIONAL_NAME("fileUplinkBufferManager"));

    // linuxTime
    Svc::Time linuxTime(FW_OPTIONAL_NAME("linuxTime"));

    // rateGroupDriverComp
    Svc::RateGroupDriver rateGroupDriverComp(FW_OPTIONAL_NAME("rateGroupDriverComp"));

    // recvBuffComp
    RecvBuff recvBuffComp(FW_OPTIONAL_NAME("recvBuffComp"));

    // staticMemory
    Svc::StaticMemory staticMemory(FW_OPTIONAL_NAME("staticMemory"));

    // textLogger
    Svc::PassiveTextLogger textLogger(FW_OPTIONAL_NAME("textLogger"));

    // uplink
    Svc::Deframer uplink(FW_OPTIONAL_NAME("uplink"));
#endif

    // ----------------------------------------------------------------------
    // Private functions
    // ----------------------------------------------------------------------

    // Initialize components
    void initComponents(const TopologyState& state) {
      blockDrv.init(QueueSizes::blockDrv);
      rateGroup1Comp.init(QueueSizes::rateGroup1Comp, InstanceIds::rateGroup1Comp);
      rateGroup2Comp.init(QueueSizes::rateGroup2Comp, InstanceIds::rateGroup2Comp);
      rateGroup3Comp.init(QueueSizes::rateGroup3Comp, InstanceIds::rateGroup3Comp);
      cmdDisp.init(QueueSizes::cmdDisp, InstanceIds::cmdDisp);
      cmdSeq.init(QueueSizes::cmdSeq, InstanceIds::cmdSeq);
      fileDownlink.init(QueueSizes::fileDownlink, InstanceIds::fileDownlink);
      fileManager.init(QueueSizes::fileManager, InstanceIds::fileManager);
      fileUplink.init(QueueSizes::fileUplink, InstanceIds::fileUplink);
      pingRcvr.init(QueueSizes::pingRcvr, InstanceIds::pingRcvr);
      eventLogger.init(QueueSizes::eventLogger, InstanceIds::eventLogger);
      chanTlm.init(QueueSizes::chanTlm, InstanceIds::chanTlm);
      prmDb.init(QueueSizes::prmDb, InstanceIds::prmDb);
      health.init(QueueSizes::health, InstanceIds::health);
      SG1.init(QueueSizes::SG1, InstanceIds::SG1);
      SG2.init(QueueSizes::SG2, InstanceIds::SG2);
      SG3.init(QueueSizes::SG3, InstanceIds::SG3);
      SG4.init(QueueSizes::SG4, InstanceIds::SG4);
      SG5.init(QueueSizes::SG5, InstanceIds::SG5);
      sendBuffComp.init(QueueSizes::sendBuffComp, InstanceIds::sendBuffComp);
      comm.init(InstanceIds::comm);
      downlink.init(InstanceIds::downlink);
      fatalAdapter.init(InstanceIds::fatalAdapter);
      fatalHandler.init(InstanceIds::fatalHandler);
      fileUplinkBufferManager.init(InstanceIds::fileUplinkBufferManager);
      linuxTime.init(InstanceIds::linuxTime);
      rateGroupDriverComp.init();
      recvBuffComp.init();
      staticMemory.init(InstanceIds::staticMemory);
      textLogger.init();
      uplink.init(InstanceIds::uplink);
    }

    // Configure components
    void configComponents(const TopologyState& state) {
      // TODO
    }

    // Set component base Ids
    void setBaseIds() {
      blockDrv.setIdBase(0x100);
      rateGroup1Comp.setIdBase(0x200);
      rateGroup2Comp.setIdBase(0x300);
      rateGroup3Comp.setIdBase(0x400);
      cmdDisp.setIdBase(0x500);
      cmdSeq.setIdBase(0x600);
      fileDownlink.setIdBase(0x700);
      fileManager.setIdBase(0x800);
      fileUplink.setIdBase(0x900);
      pingRcvr.setIdBase(0xA00);
      eventLogger.setIdBase(0xB00);
      chanTlm.setIdBase(0xC00);
      prmDb.setIdBase(0xD00);
      health.setIdBase(0x2000);
      SG1.setIdBase(0x2100);
      SG2.setIdBase(0x2200);
      SG3.setIdBase(0x2300);
      SG4.setIdBase(0x2400);
      SG5.setIdBase(0x2500);
      sendBuffComp.setIdBase(0x2600);
      comm.setIdBase(0x4000);
      downlink.setIdBase(0x4100);
      fatalAdapter.setIdBase(0x4200);
      fatalHandler.setIdBase(0x4300);
      fileUplinkBufferManager.setIdBase(0x4400);
      linuxTime.setIdBase(0x4500);
      rateGroupDriverComp.setIdBase(0x4600);
      recvBuffComp.setIdBase(0x4700);
      staticMemory.setIdBase(0x4800);
      textLogger.setIdBase(0x4900);
      uplink.setIdBase(0x4A00);
    }

    // Connect components
    void connectComponents() {

      // CommandResponse
      SG1.set_cmdResponseOut_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      cmdSeq.set_cmdResponseOut_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      SG3.set_cmdResponseOut_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      prmDb.set_CmdStatus_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      SG4.set_cmdResponseOut_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      sendBuffComp.set_CmdStatus_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      pingRcvr.set_CmdStatus_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      eventLogger.set_CmdStatus_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      recvBuffComp.set_CmdStatus_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      SG5.set_cmdResponseOut_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      cmdDisp.set_CmdStatus_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      health.set_CmdStatus_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      SG2.set_cmdResponseOut_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      fileManager.set_cmdResponseOut_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );
      fileDownlink.set_cmdResponseOut_OutputPort(
        0,
        cmdDisp.get_compCmdStat_InputPort(0)
      );

      // Downlink
      comm.set_deallocate_OutputPort(
        0,
        staticMemory.get_bufferDeallocate_InputPort(0)
      );
      downlink.set_bufferDeallocate_OutputPort(
        0,
        fileDownlink.get_bufferReturn_InputPort(0)
      );
      downlink.set_framedOut_OutputPort(
        0,
        comm.get_send_InputPort(0)
      );
      downlink.set_framedAllocate_OutputPort(
        0,
        staticMemory.get_bufferAllocate_InputPort(0)
      );
      fileDownlink.set_bufferSendOut_OutputPort(
        0,
        downlink.get_bufferIn_InputPort(0)
      );
      eventLogger.set_PktSend_OutputPort(
        0,
        downlink.get_comIn_InputPort(0)
      );
      chanTlm.set_PktSend_OutputPort(
        0,
        downlink.get_comIn_InputPort(0)
      );

      // Command
      cmdDisp.set_compCmdSend_OutputPort(
        9,
        fileManager.get_cmdIn_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        3,
        SG4.get_cmdIn_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        4,
        SG5.get_cmdIn_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        0,
        SG1.get_cmdIn_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        6,
        cmdSeq.get_cmdIn_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        11,
        pingRcvr.get_CmdDisp_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        10,
        health.get_CmdDisp_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        5,
        cmdDisp.get_CmdDisp_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        7,
        eventLogger.get_CmdDisp_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        14,
        sendBuffComp.get_CmdDisp_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        13,
        recvBuffComp.get_CmdDisp_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        2,
        SG3.get_cmdIn_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        12,
        prmDb.get_CmdDisp_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        8,
        fileDownlink.get_cmdIn_InputPort(0)
      );
      cmdDisp.set_compCmdSend_OutputPort(
        1,
        SG2.get_cmdIn_InputPort(0)
      );

      // RateGroups
      rateGroup3Comp.set_RateGroupMemberOut_OutputPort(
        3,
        fileUplinkBufferManager.get_schedIn_InputPort(0)
      );
      rateGroup3Comp.set_RateGroupMemberOut_OutputPort(
        2,
        blockDrv.get_Sched_InputPort(0)
      );
      rateGroup3Comp.set_RateGroupMemberOut_OutputPort(
        1,
        SG5.get_schedIn_InputPort(0)
      );
      rateGroup3Comp.set_RateGroupMemberOut_OutputPort(
        0,
        health.get_Run_InputPort(0)
      );
      rateGroupDriverComp.set_CycleOut_OutputPort(
        2,
        rateGroup3Comp.get_CycleIn_InputPort(0)
      );
      rateGroup2Comp.set_RateGroupMemberOut_OutputPort(
        3,
        SG4.get_schedIn_InputPort(0)
      );
      rateGroup2Comp.set_RateGroupMemberOut_OutputPort(
        2,
        SG3.get_schedIn_InputPort(0)
      );
      rateGroup2Comp.set_RateGroupMemberOut_OutputPort(
        1,
        sendBuffComp.get_SchedIn_InputPort(0)
      );
      rateGroup2Comp.set_RateGroupMemberOut_OutputPort(
        0,
        cmdSeq.get_schedIn_InputPort(0)
      );
      rateGroupDriverComp.set_CycleOut_OutputPort(
        1,
        rateGroup2Comp.get_CycleIn_InputPort(0)
      );
      rateGroup1Comp.set_RateGroupMemberOut_OutputPort(
        3,
        fileDownlink.get_Run_InputPort(0)
      );
      rateGroup1Comp.set_RateGroupMemberOut_OutputPort(
        2,
        chanTlm.get_Run_InputPort(0)
      );
      rateGroup1Comp.set_RateGroupMemberOut_OutputPort(
        1,
        SG2.get_schedIn_InputPort(0)
      );
      rateGroup1Comp.set_RateGroupMemberOut_OutputPort(
        0,
        SG1.get_schedIn_InputPort(0)
      );
      rateGroupDriverComp.set_CycleOut_OutputPort(
        0,
        rateGroup1Comp.get_CycleIn_InputPort(0)
      );
      blockDrv.set_CycleOut_OutputPort(
        0,
        rateGroupDriverComp.get_CycleIn_InputPort(0)
      );

      // Health
      health.set_PingSend_OutputPort(
        1,
        chanTlm.get_pingIn_InputPort(0)
      );
      blockDrv.set_PingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(0)
      );
      health.set_PingSend_OutputPort(
        9,
        prmDb.get_pingIn_InputPort(0)
      );
      cmdSeq.set_pingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(3)
      );
      health.set_PingSend_OutputPort(
        6,
        fileManager.get_pingIn_InputPort(0)
      );
      health.set_PingSend_OutputPort(
        3,
        cmdSeq.get_pingIn_InputPort(0)
      );
      rateGroup2Comp.set_PingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(11)
      );
      fileDownlink.set_pingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(5)
      );
      health.set_PingSend_OutputPort(
        8,
        pingRcvr.get_PingIn_InputPort(0)
      );
      fileUplink.set_pingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(7)
      );
      chanTlm.set_pingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(1)
      );
      health.set_PingSend_OutputPort(
        5,
        fileDownlink.get_pingIn_InputPort(0)
      );
      rateGroup1Comp.set_PingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(10)
      );
      health.set_PingSend_OutputPort(
        7,
        fileUplink.get_pingIn_InputPort(0)
      );
      prmDb.set_pingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(9)
      );
      health.set_PingSend_OutputPort(
        2,
        cmdDisp.get_pingIn_InputPort(0)
      );
      health.set_PingSend_OutputPort(
        4,
        eventLogger.get_pingIn_InputPort(0)
      );
      rateGroup3Comp.set_PingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(12)
      );
      fileManager.set_pingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(6)
      );
      cmdDisp.set_pingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(2)
      );
      eventLogger.set_pingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(4)
      );
      health.set_PingSend_OutputPort(
        12,
        rateGroup3Comp.get_PingIn_InputPort(0)
      );
      health.set_PingSend_OutputPort(
        11,
        rateGroup2Comp.get_PingIn_InputPort(0)
      );
      health.set_PingSend_OutputPort(
        0,
        blockDrv.get_PingIn_InputPort(0)
      );
      health.set_PingSend_OutputPort(
        10,
        rateGroup1Comp.get_PingIn_InputPort(0)
      );
      pingRcvr.set_PingOut_OutputPort(
        0,
        health.get_PingReturn_InputPort(8)
      );

      // Parameters
      sendBuffComp.set_ParamGet_OutputPort(
        0,
        prmDb.get_getPrm_InputPort(0)
      );
      recvBuffComp.set_ParamSet_OutputPort(
        0,
        prmDb.get_setPrm_InputPort(0)
      );
      sendBuffComp.set_ParamSet_OutputPort(
        0,
        prmDb.get_setPrm_InputPort(0)
      );
      recvBuffComp.set_ParamGet_OutputPort(
        0,
        prmDb.get_getPrm_InputPort(0)
      );

      // Uplink
      fileUplink.set_bufferSendOut_OutputPort(
        0,
        fileUplinkBufferManager.get_bufferSendIn_InputPort(0)
      );
      uplink.set_bufferDeallocate_OutputPort(
        0,
        fileUplinkBufferManager.get_bufferSendIn_InputPort(0)
      );
      uplink.set_bufferOut_OutputPort(
        0,
        fileUplink.get_bufferSendIn_InputPort(0)
      );
      uplink.set_bufferAllocate_OutputPort(
        0,
        fileUplinkBufferManager.get_bufferGetCallee_InputPort(0)
      );
      cmdDisp.set_seqCmdStatus_OutputPort(
        1,
        uplink.get_cmdResponseIn_InputPort(0)
      );
      uplink.set_comOut_OutputPort(
        0,
        cmdDisp.get_seqCmdBuff_InputPort(1)
      );
      uplink.set_framedDeallocate_OutputPort(
        0,
        staticMemory.get_bufferDeallocate_InputPort(1)
      );
      comm.set_recv_OutputPort(
        0,
        uplink.get_framedIn_InputPort(0)
      );
      comm.set_allocate_OutputPort(
        0,
        staticMemory.get_bufferAllocate_InputPort(1)
      );

      // Sequencer
      cmdDisp.set_seqCmdStatus_OutputPort(
        0,
        cmdSeq.get_cmdResponseIn_InputPort(0)
      );
      cmdSeq.set_comCmdOut_OutputPort(
        0,
        cmdDisp.get_seqCmdBuff_InputPort(0)
      );

      // Telemetry
      blockDrv.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      recvBuffComp.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      SG4.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      rateGroup1Comp.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      SG1.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      SG3.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      cmdSeq.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      SG2.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      pingRcvr.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      rateGroup2Comp.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      cmdDisp.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      fileManager.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      fileDownlink.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      fileUplink.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      health.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      rateGroup3Comp.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      fileUplinkBufferManager.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      sendBuffComp.set_Tlm_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );
      SG5.set_tlmOut_OutputPort(
        0,
        chanTlm.get_TlmRecv_InputPort(0)
      );

      // CommandRegistration
      pingRcvr.set_CmdReg_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(11)
      );
      sendBuffComp.set_CmdReg_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(14)
      );
      health.set_CmdReg_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(10)
      );
      cmdDisp.set_CmdReg_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(5)
      );
      SG3.set_cmdRegOut_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(2)
      );
      SG4.set_cmdRegOut_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(3)
      );
      cmdSeq.set_cmdRegOut_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(6)
      );
      prmDb.set_CmdReg_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(12)
      );
      fileDownlink.set_cmdRegOut_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(8)
      );
      SG5.set_cmdRegOut_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(4)
      );
      eventLogger.set_CmdReg_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(7)
      );
      SG2.set_cmdRegOut_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(1)
      );
      SG1.set_cmdRegOut_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(0)
      );
      fileManager.set_cmdRegOut_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(9)
      );
      recvBuffComp.set_CmdReg_OutputPort(
        0,
        cmdDisp.get_compCmdReg_InputPort(13)
      );

      // Time
      pingRcvr.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      sendBuffComp.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      cmdSeq.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      fileUplinkBufferManager.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      fileManager.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      rateGroup1Comp.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      downlink.set_timeGet_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      fatalAdapter.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      SG5.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      SG1.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      eventLogger.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      recvBuffComp.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      fileUplink.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      SG3.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      cmdDisp.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      rateGroup2Comp.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      rateGroup3Comp.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      fileDownlink.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      prmDb.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      health.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      SG4.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      SG2.set_timeCaller_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );
      blockDrv.set_Time_OutputPort(
        0,
        linuxTime.get_timeGetPort_InputPort(0)
      );

      // Ref
      blockDrv.set_BufferOut_OutputPort(
        0,
        recvBuffComp.get_Data_InputPort(0)
      );
      sendBuffComp.set_Data_OutputPort(
        0,
        blockDrv.get_BufferIn_InputPort(0)
      );

      // TextEvents
      pingRcvr.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      rateGroup3Comp.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      fileManager.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      SG2.set_logTextOut_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      health.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      recvBuffComp.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      cmdDisp.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      rateGroup2Comp.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      rateGroup1Comp.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      fatalAdapter.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      cmdSeq.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      sendBuffComp.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      prmDb.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      SG5.set_logTextOut_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      fileUplink.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      fileDownlink.set_textEventOut_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      eventLogger.set_LogText_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      SG4.set_logTextOut_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      fileUplinkBufferManager.set_textEventOut_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      SG3.set_logTextOut_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );
      SG1.set_logTextOut_OutputPort(
        0,
        textLogger.get_TextLogger_InputPort(0)
      );

      // Events
      health.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      pingRcvr.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      rateGroup2Comp.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      SG4.set_logOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      SG1.set_logOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      SG5.set_logOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      prmDb.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      SG3.set_logOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      eventLogger.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      fileManager.set_eventOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      rateGroup1Comp.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      fileUplinkBufferManager.set_eventOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      cmdSeq.set_logOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      recvBuffComp.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      fatalAdapter.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      cmdDisp.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      rateGroup3Comp.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      fileUplink.set_eventOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      sendBuffComp.set_Log_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      fileDownlink.set_eventOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );
      SG2.set_logOut_OutputPort(
        0,
        eventLogger.get_LogRecv_InputPort(0)
      );

      // FaultProtection
      eventLogger.set_FatalAnnounce_OutputPort(
        0,
        fatalHandler.get_FatalReceive_InputPort(0)
      );

    }

    // Register commands
    void regCommands() {
      cmdDisp.regCommands();
      cmdSeq.regCommands();
      fileDownlink.regCommands();
      fileManager.regCommands();
      pingRcvr.regCommands();
      eventLogger.regCommands();
      prmDb.regCommands();
      health.regCommands();
      SG1.regCommands();
      SG2.regCommands();
      SG3.regCommands();
      SG4.regCommands();
      SG5.regCommands();
      sendBuffComp.regCommands();
      recvBuffComp.regCommands();
    }

    // Load parameters
    void loadParameters() {
      sendBuffComp.loadParameters();
      recvBuffComp.loadParameters();
    }

    // Start tasks
    void startTasks(const TopologyState& state) {
      blockDrv.start(
        TaskIds::blockDrv,
        Priorities::blockDrv,
        StackSizes::blockDrv
      );
      rateGroup1Comp.start(
        TaskIds::rateGroup1Comp,
        Priorities::rateGroup1Comp,
        StackSizes::rateGroup1Comp
      );
      rateGroup2Comp.start(
        TaskIds::rateGroup2Comp,
        Priorities::rateGroup2Comp,
        StackSizes::rateGroup2Comp
      );
      rateGroup3Comp.start(
        TaskIds::rateGroup3Comp,
        Priorities::rateGroup3Comp,
        StackSizes::rateGroup3Comp
      );
      cmdDisp.start(
        TaskIds::cmdDisp,
        Priorities::cmdDisp,
        StackSizes::cmdDisp
      );
      cmdSeq.start(
        TaskIds::cmdSeq,
        Priorities::cmdSeq,
        StackSizes::cmdSeq
      );
      fileDownlink.start(
        TaskIds::fileDownlink,
        Priorities::fileDownlink,
        StackSizes::fileDownlink
      );
      fileManager.start(
        TaskIds::fileManager,
        Priorities::fileManager,
        StackSizes::fileManager
      );
      fileUplink.start(
        TaskIds::fileUplink,
        Priorities::fileUplink,
        StackSizes::fileUplink
      );
      pingRcvr.start(
        TaskIds::pingRcvr,
        Priorities::pingRcvr,
        StackSizes::pingRcvr
      );
      eventLogger.start(
        TaskIds::eventLogger,
        Priorities::eventLogger,
        StackSizes::eventLogger
      );
      chanTlm.start(
        TaskIds::chanTlm,
        Priorities::chanTlm,
        StackSizes::chanTlm
      );
      prmDb.start(
        TaskIds::prmDb,
        Priorities::prmDb,
        StackSizes::prmDb
      );
    }

    // Stop tasks
    void stopTasks(const TopologyState& state) {
      blockDrv.exit();
      rateGroup1Comp.exit();
      rateGroup2Comp.exit();
      rateGroup3Comp.exit();
      cmdDisp.exit();
      cmdSeq.exit();
      fileDownlink.exit();
      fileManager.exit();
      fileUplink.exit();
      pingRcvr.exit();
      eventLogger.exit();
      chanTlm.exit();
      prmDb.exit();
    }

    // Free threads
    void freeThreads(const TopologyState& state) {
      blockDrv.ActiveComponentBase::join(NULL);
      rateGroup1Comp.ActiveComponentBase::join(NULL);
      rateGroup2Comp.ActiveComponentBase::join(NULL);
      rateGroup3Comp.ActiveComponentBase::join(NULL);
      cmdDisp.ActiveComponentBase::join(NULL);
      cmdSeq.ActiveComponentBase::join(NULL);
      fileDownlink.ActiveComponentBase::join(NULL);
      fileManager.ActiveComponentBase::join(NULL);
      fileUplink.ActiveComponentBase::join(NULL);
      pingRcvr.ActiveComponentBase::join(NULL);
      eventLogger.ActiveComponentBase::join(NULL);
      chanTlm.ActiveComponentBase::join(NULL);
      prmDb.ActiveComponentBase::join(NULL);
    }

    void tearDownComponents(const TopologyState& state) {
      // TODO
    }

  }

  // ----------------------------------------------------------------------
  // Public interface functions
  // ----------------------------------------------------------------------

  void setup(const TopologyState& state) {
    initComponents(state);
    setBaseIds();
    connectComponents();
    regCommands();
    loadParameters();
    startTasks(state);
  }

  void teardown(const TopologyState& state) {
    stopTasks(state);
    freeThreads(state);
    tearDownComponents(state);
  }

  // ======================================================================
  // OLD F PRIME XML
  // TODO: Migrate this to area above, with appropriate changes
  // ======================================================================

  // List of context IDs
  // TODO: Move to config constants
  enum {
      UPLINK_BUFFER_STORE_SIZE = 3000,
      UPLINK_BUFFER_QUEUE_SIZE = 30,
      UPLINK_BUFFER_MGR_ID = 200
  };

  // TODO: Move to config objects
  Os::Log osLogger;
  Svc::FprimeDeframing deframing;
  Svc::FprimeFraming framing;

  // Component instance pointers
  // TODO: Move to config objects
  static NATIVE_INT_TYPE rgDivs[Svc::RateGroupDriverImpl::DIVIDER_SIZE] = {1,2,4};
  // TODO: Move to instance creation
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

  // TODO: Eliminate this. The FPP code generator inserts the correct
  // names into pingEntries.
  const char* getHealthName(Fw::ObjBase& comp) {
     #if FW_OBJECT_NAMES == 1
         return comp.getObjName();
     #else
        return "[no object name]"
     #endif
  }

  //Set base id section
  // TODO: Replace with setBaseIds
  void setRefIds(void){
    blockDrv.setIdBase(256);
    rateGroup1Comp.setIdBase(512);
    rateGroup2Comp.setIdBase(768);
    rateGroup3Comp.setIdBase(1024);
    cmdDisp.setIdBase(1280);
    cmdSeq.setIdBase(1536);
    fileDownlink.setIdBase(1792);
    fileManager.setIdBase(2048);
    fileUplink.setIdBase(2304);
    pingRcvr.setIdBase(2560);
    eventLogger.setIdBase(2816);
    chanTlm.setIdBase(3072);
    prmDb.setIdBase(3328);
    health.setIdBase(8192);
    SG1.setIdBase(8448);
    SG2.setIdBase(8704);
    SG3.setIdBase(8960);
    SG4.setIdBase(9216);
    SG5.setIdBase(9472);
    sendBuffComp.setIdBase(9728);
    comm.setIdBase(16384);
    downlink.setIdBase(16640);
    fatalAdapter.setIdBase(16896);
    fatalHandler.setIdBase(17152);
    fileUplinkBufferManager.setIdBase(17408);
    linuxTime.setIdBase(17664);
    rateGroupDriverComp.setIdBase(17920);
    recvBuffComp.setIdBase(18176);
    staticMemory.setIdBase(18432);
    textLogger.setIdBase(18688);
    uplink.setIdBase(18944);
  }

  // TODO: Replace with connectComponents
  void constructRefArchitecture(void) {
    setRefIds();
    blockDrv.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(0));
    blockDrv.set_CycleOut_OutputPort(0, rateGroupDriverComp.get_CycleIn_InputPort(0));
    blockDrv.set_BufferOut_OutputPort(0, recvBuffComp.get_Data_InputPort(0));
    blockDrv.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    blockDrv.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    rateGroup1Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    rateGroup1Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(10));
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(0, SG1.get_schedIn_InputPort(0));
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(1, SG2.get_schedIn_InputPort(0));
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(2, chanTlm.get_Run_InputPort(0));
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(3, fileDownlink.get_Run_InputPort(0));
    rateGroup1Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    rateGroup1Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    rateGroup1Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    rateGroup2Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    rateGroup2Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(11));
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(0, cmdSeq.get_schedIn_InputPort(0));
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(1, sendBuffComp.get_SchedIn_InputPort(0));
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(2, SG3.get_schedIn_InputPort(0));
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(3, SG4.get_schedIn_InputPort(0));
    rateGroup2Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    rateGroup2Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    rateGroup2Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    rateGroup3Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    rateGroup3Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(12));
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(0, health.get_Run_InputPort(0));
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(1, SG5.get_schedIn_InputPort(0));
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(2, blockDrv.get_Sched_InputPort(0));
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(3, fileUplinkBufferManager.get_schedIn_InputPort(0));
    rateGroup3Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    rateGroup3Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    rateGroup3Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(0, SG1.get_cmdIn_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(1, SG2.get_cmdIn_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(2, SG3.get_cmdIn_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(3, SG4.get_cmdIn_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(4, SG5.get_cmdIn_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(5, cmdDisp.get_CmdDisp_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(6, cmdSeq.get_cmdIn_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(7, eventLogger.get_CmdDisp_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(8, fileDownlink.get_cmdIn_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(9, fileManager.get_cmdIn_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(10, health.get_CmdDisp_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(11, pingRcvr.get_CmdDisp_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(12, prmDb.get_CmdDisp_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(13, recvBuffComp.get_CmdDisp_InputPort(0));
    cmdDisp.set_compCmdSend_OutputPort(14, sendBuffComp.get_CmdDisp_InputPort(0));
    cmdDisp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(5));
    cmdDisp.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    cmdDisp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    cmdDisp.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(2));
    cmdDisp.set_seqCmdStatus_OutputPort(0, cmdSeq.get_cmdResponseIn_InputPort(0));
    cmdDisp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    cmdDisp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    cmdDisp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    cmdDisp.set_seqCmdStatus_OutputPort(1, uplink.get_cmdResponseIn_InputPort(0));
    cmdSeq.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(6));
    cmdSeq.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    cmdSeq.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    cmdSeq.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(3));
    cmdSeq.set_comCmdOut_OutputPort(0, cmdDisp.get_seqCmdBuff_InputPort(0));
    cmdSeq.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    cmdSeq.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    cmdSeq.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    fileDownlink.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(8));
    fileDownlink.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    fileDownlink.set_bufferSendOut_OutputPort(0, downlink.get_bufferIn_InputPort(0));
    fileDownlink.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    fileDownlink.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(5));
    fileDownlink.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    fileDownlink.set_textEventOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    fileDownlink.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    fileManager.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(9));
    fileManager.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    fileManager.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    fileManager.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(6));
    fileManager.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    fileManager.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    fileManager.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    fileUplink.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    fileUplink.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(7));
    fileUplink.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    fileUplink.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    fileUplink.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    fileUplink.set_bufferSendOut_OutputPort(0, fileUplinkBufferManager.get_bufferSendIn_InputPort(0));
    pingRcvr.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(11));
    pingRcvr.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    pingRcvr.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    pingRcvr.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(8));
    pingRcvr.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    pingRcvr.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    pingRcvr.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    eventLogger.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(7));
    eventLogger.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    eventLogger.set_PktSend_OutputPort(0, downlink.get_comIn_InputPort(0));
    eventLogger.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    eventLogger.set_FatalAnnounce_OutputPort(0, fatalHandler.get_FatalReceive_InputPort(0));
    eventLogger.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(4));
    eventLogger.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    eventLogger.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    chanTlm.set_PktSend_OutputPort(0, downlink.get_comIn_InputPort(0));
    chanTlm.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(1));
    prmDb.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(12));
    prmDb.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    prmDb.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    prmDb.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(9));
    prmDb.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    prmDb.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    health.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(10));
    health.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    health.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    health.set_PingSend_OutputPort(0, blockDrv.get_PingIn_InputPort(0));
    health.set_PingSend_OutputPort(1, chanTlm.get_pingIn_InputPort(0));
    health.set_PingSend_OutputPort(2, cmdDisp.get_pingIn_InputPort(0));
    health.set_PingSend_OutputPort(3, cmdSeq.get_pingIn_InputPort(0));
    health.set_PingSend_OutputPort(4, eventLogger.get_pingIn_InputPort(0));
    health.set_PingSend_OutputPort(5, fileDownlink.get_pingIn_InputPort(0));
    health.set_PingSend_OutputPort(6, fileManager.get_pingIn_InputPort(0));
    health.set_PingSend_OutputPort(7, fileUplink.get_pingIn_InputPort(0));
    health.set_PingSend_OutputPort(8, pingRcvr.get_PingIn_InputPort(0));
    health.set_PingSend_OutputPort(9, prmDb.get_pingIn_InputPort(0));
    health.set_PingSend_OutputPort(10, rateGroup1Comp.get_PingIn_InputPort(0));
    health.set_PingSend_OutputPort(11, rateGroup2Comp.get_PingIn_InputPort(0));
    health.set_PingSend_OutputPort(12, rateGroup3Comp.get_PingIn_InputPort(0));
    health.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    health.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    health.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    SG1.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(0));
    SG1.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    SG1.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    SG1.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    SG1.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    SG1.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    SG2.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(1));
    SG2.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    SG2.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    SG2.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    SG2.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    SG2.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    SG3.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(2));
    SG3.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    SG3.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    SG3.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    SG3.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    SG3.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    SG4.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(3));
    SG4.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    SG4.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    SG4.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    SG4.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    SG4.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    SG5.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(4));
    SG5.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    SG5.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    SG5.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    SG5.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    SG5.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    sendBuffComp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(14));
    sendBuffComp.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    sendBuffComp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    sendBuffComp.set_ParamGet_OutputPort(0, prmDb.get_getPrm_InputPort(0));
    sendBuffComp.set_ParamSet_OutputPort(0, prmDb.get_setPrm_InputPort(0));
    sendBuffComp.set_Data_OutputPort(0, blockDrv.get_BufferIn_InputPort(0));
    sendBuffComp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    sendBuffComp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    sendBuffComp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    comm.set_deallocate_OutputPort(0, staticMemory.get_bufferDeallocate_InputPort(0));
    comm.set_allocate_OutputPort(0, staticMemory.get_bufferAllocate_InputPort(1));
    comm.set_recv_OutputPort(0, uplink.get_framedIn_InputPort(0));
    downlink.set_bufferDeallocate_OutputPort(0, fileDownlink.get_bufferReturn_InputPort(0));
    downlink.set_framedAllocate_OutputPort(0, staticMemory.get_bufferAllocate_InputPort(0));
    downlink.set_framedOut_OutputPort(0, comm.get_send_InputPort(0));
    downlink.set_timeGet_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    fatalAdapter.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    fatalAdapter.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    fatalAdapter.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    fileUplinkBufferManager.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    fileUplinkBufferManager.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    fileUplinkBufferManager.set_textEventOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    fileUplinkBufferManager.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    rateGroupDriverComp.set_CycleOut_OutputPort(0, rateGroup1Comp.get_CycleIn_InputPort(0));
    rateGroupDriverComp.set_CycleOut_OutputPort(1, rateGroup2Comp.get_CycleIn_InputPort(0));
    rateGroupDriverComp.set_CycleOut_OutputPort(2, rateGroup3Comp.get_CycleIn_InputPort(0));
    recvBuffComp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(13));
    recvBuffComp.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    recvBuffComp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    recvBuffComp.set_ParamGet_OutputPort(0, prmDb.get_getPrm_InputPort(0));
    recvBuffComp.set_ParamSet_OutputPort(0, prmDb.get_setPrm_InputPort(0));
    recvBuffComp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    recvBuffComp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    recvBuffComp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    uplink.set_bufferAllocate_OutputPort(0, fileUplinkBufferManager.get_bufferGetCallee_InputPort(0));
    uplink.set_bufferDeallocate_OutputPort(0, fileUplinkBufferManager.get_bufferSendIn_InputPort(0));
    uplink.set_bufferOut_OutputPort(0, fileUplink.get_bufferSendIn_InputPort(0));
    uplink.set_comOut_OutputPort(0, cmdDisp.get_seqCmdBuff_InputPort(1));
    uplink.set_framedDeallocate_OutputPort(0, staticMemory.get_bufferDeallocate_InputPort(1));
  }

  // TODO: Replace specialized arguments with RefTopologyState.
  // TODO: Reorganize into the FPP phases
  bool constructApp(bool dump, U32 port_number, char* hostname) {

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

      textLogger.init();

      eventLogger.init(10,0);
      
      linuxTime.init(0);

      chanTlm.init(10,0);

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

      // This order must match the order of the connections to
      // the health component in the topology
      Svc::HealthImpl::PingEntry pingEntries[] = {
          {3,5,getHealthName(blockDrv)}, // 0
          {3,5,getHealthName(chanTlm)}, // 1
          {3,5,getHealthName(cmdDisp)}, // 2
          {3,5,getHealthName(cmdSeq)}, // 3
          {3,5,getHealthName(eventLogger)}, // 4
          {3,5,getHealthName(fileDownlink)}, // 5
          {3,5,getHealthName(fileManager)}, // 6
          {3,5,getHealthName(fileUplink)}, // 7
          {3,5,getHealthName(pingRcvr)}, // 8
          {3,5,getHealthName(prmDb)}, // 9
          {3,5,getHealthName(rateGroup1Comp)}, // 10
          {3,5,getHealthName(rateGroup2Comp)}, // 11
          {3,5,getHealthName(rateGroup3Comp)}, // 12
      };

      // register ping table
      health.setPingEntries(pingEntries,FW_NUM_ARRAY_ELEMENTS(pingEntries),0x123);

      // Active component startup
      // start rate groups
      rateGroup1Comp.start(0, Priorities::rateGroup1Comp, 10 * 1024);
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
      fileManager.start(0, 100, 10*1024);

      pingRcvr.start(0, 100, 10*1024);

     

      // Initialize socket server if and only if there is a valid specification
      if (hostname != NULL && port_number != 0) {
          Fw::EightyCharString name("ReceiveTask");
          // Uplink is configured for receive so a socket task is started
          comm.configure(hostname, port_number);
          comm.startSocketTask(name, 100, 10 * 1024);
      }
      return false;
  }

  // TODO: Break into three phases: exit, stop threads, and tear down components
  void exitTasks(void) {

      // Exit
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
      fileManager.exit();
      cmdSeq.exit();
      pingRcvr.exit();

      // Stop tasks
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
      (void) fileManager.ActiveComponentBase::join(NULL);
      (void) cmdSeq.ActiveComponentBase::join(NULL);
      (void) pingRcvr.ActiveComponentBase::join(NULL);
      comm.stopSocketTask();
      (void) comm.joinSocketTask(NULL);

      // Tear down components
      cmdSeq.deallocateBuffer(mallocator);
      fileUplinkBufferManager.cleanup();
  }

}
