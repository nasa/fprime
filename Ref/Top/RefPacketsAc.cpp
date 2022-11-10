

#include <Svc/TlmPacketizer/TlmPacketizerTypes.hpp>
#include <Ref/Top/RefPacketsAc.hpp>

#include <FpConfig.hpp>
#include <FpConfig.hpp>
#include <Fw/Time/Time.hpp>

// Verify packets not too large for ComBuffer
// if this macro gives a compile error, that means the packets are too large

void check_func(void) {
    COMPILE_TIME_ASSERT((111 <= (FW_COM_BUFFER_MAX_SIZE - Fw::Time::SERIALIZED_SIZE - sizeof(FwTlmPacketizeIdType) - sizeof(FwPacketDescriptorType))),PacketsTooBig);
}

namespace Ref {

  static const Svc::TlmPacketizerChannelEntry CDHList[] = {
      {1280, 4}, // cmdDisp.CommandsDispatched
      {512, 4}, // rateGroup1Comp.RgMaxTime
      {768, 4}, // rateGroup2Comp.RgMaxTime
      {1024, 4}, // rateGroup3Comp.RgMaxTime
      {1536, 4}, // cmdSeq.CS_LoadCommands
      {1537, 4}, // cmdSeq.CS_CancelCommands
      {1539, 4}, // cmdSeq.CS_CommandsExecuted
      {1540, 4}, // cmdSeq.CS_SequencesCompleted
      {2304, 4}, // fileUplink.FilesReceived
      {2305, 4}, // fileUplink.PacketsReceived
      {17408, 4}, // fileUplinkBufferManager.TotalBuffs
      {17409, 4}, // fileUplinkBufferManager.CurrBuffs
      {17410, 4}, // fileUplinkBufferManager.HiBuffs
      {1792, 4}, // fileDownlink.FilesSent
      {1793, 4}, // fileDownlink.PacketsSent
      {2048, 4}, // fileManager.CommandsExecuted
      {3584, 4}, // pktTlm.SendLevel
  };

  static const Svc::TlmPacketizerPacket CDH = { CDHList, 1, 1, FW_NUM_ARRAY_ELEMENTS(CDHList) };

  static const Svc::TlmPacketizerChannelEntry CDHErrorsList[] = {
      {513, 4}, // rateGroup1Comp.RgCycleSlips
      {769, 4}, // rateGroup2Comp.RgCycleSlips
      {1025, 4}, // rateGroup3Comp.RgCycleSlips
      {1538, 4}, // cmdSeq.CS_Errors
      {2306, 4}, // fileUplink.Warnings
      {1794, 4}, // fileDownlink.Warnings
      {8192, 4}, // health.PingLateWarnings
      {2049, 4}, // fileManager.Errors
      {17411, 4}, // fileUplinkBufferManager.NoBuffs
      {17412, 4}, // fileUplinkBufferManager.EmptyBuffs
      {2049, 4}, // fileManager.Errors
  };

  static const Svc::TlmPacketizerPacket CDHErrors = { CDHErrorsList, 2, 1, FW_NUM_ARRAY_ELEMENTS(CDHErrorsList) };

  static const Svc::TlmPacketizerChannelEntry DriveTlmList[] = {
      {2560, 4}, // pingRcvr.PR_NumPings
      {9728, 8}, // sendBuffComp.PacketsSent
      {9729, 4}, // sendBuffComp.NumErrorsInjected
      {9730, 1}, // sendBuffComp.Parameter3
      {9731, 4}, // sendBuffComp.Parameter4
      {9732, 4}, // sendBuffComp.SendState
      {18176, 12}, // recvBuffComp.PktState
      {18177, 4}, // recvBuffComp.Sensor1
      {18178, 4}, // recvBuffComp.Sensor2
      {18179, 4}, // recvBuffComp.Parameter1
      {18180, 2}, // recvBuffComp.Parameter2
      {256, 4}, // blockDrv.BD_Cycles
  };

  static const Svc::TlmPacketizerPacket DriveTlm = { DriveTlmList, 3, 1, FW_NUM_ARRAY_ELEMENTS(DriveTlmList) };

  static const Svc::TlmPacketizerChannelEntry SigGenSumList[] = {
      {8449, 4}, // SG1.Output
      {8448, 4}, // SG1.Type
      {8705, 4}, // SG2.Output
      {8704, 4}, // SG2.Type
      {8961, 4}, // SG3.Output
      {8960, 4}, // SG3.Type
      {9217, 4}, // SG4.Output
      {9216, 4}, // SG4.Type
      {9473, 4}, // SG5.Output
      {9472, 4}, // SG5.Type
  };

  static const Svc::TlmPacketizerPacket SigGenSum = { SigGenSumList, 4, 1, FW_NUM_ARRAY_ELEMENTS(SigGenSumList) };

  static const Svc::TlmPacketizerChannelEntry SystemRes1List[] = {
      {19200, 8}, // systemResources.MEMORY_TOTAL
      {19201, 8}, // systemResources.MEMORY_USED
      {19202, 8}, // systemResources.NON_VOLATILE_TOTAL
      {19203, 8}, // systemResources.NON_VOLATILE_FREE
  };

  static const Svc::TlmPacketizerPacket SystemRes1 = { SystemRes1List, 5, 2, FW_NUM_ARRAY_ELEMENTS(SystemRes1List) };

  static const Svc::TlmPacketizerChannelEntry SystemRes2List[] = {
      {19221, 42}, // systemResources.FRAMEWORK_VERSION
      {19222, 42}, // systemResources.PROJECT_VERSION
  };

  static const Svc::TlmPacketizerPacket SystemRes2 = { SystemRes2List, 6, 2, FW_NUM_ARRAY_ELEMENTS(SystemRes2List) };

  static const Svc::TlmPacketizerChannelEntry SystemRes3List[] = {
      {19204, 4}, // systemResources.CPU
      {19205, 4}, // systemResources.CPU_00
      {19206, 4}, // systemResources.CPU_01
      {19207, 4}, // systemResources.CPU_02
      {19208, 4}, // systemResources.CPU_03
      {19209, 4}, // systemResources.CPU_04
      {19210, 4}, // systemResources.CPU_05
      {19211, 4}, // systemResources.CPU_06
      {19212, 4}, // systemResources.CPU_07
      {19213, 4}, // systemResources.CPU_08
      {19214, 4}, // systemResources.CPU_09
      {19215, 4}, // systemResources.CPU_10
      {19216, 4}, // systemResources.CPU_11
      {19217, 4}, // systemResources.CPU_12
      {19218, 4}, // systemResources.CPU_13
      {19219, 4}, // systemResources.CPU_14
      {19220, 4}, // systemResources.CPU_15
  };

  static const Svc::TlmPacketizerPacket SystemRes3 = { SystemRes3List, 7, 2, FW_NUM_ARRAY_ELEMENTS(SystemRes3List) };

  static const Svc::TlmPacketizerChannelEntry SigGen1InfoList[] = {
      {8453, 52}, // SG1.Info
  };

  static const Svc::TlmPacketizerPacket SigGen1Info = { SigGen1InfoList, 10, 2, FW_NUM_ARRAY_ELEMENTS(SigGen1InfoList) };

  static const Svc::TlmPacketizerChannelEntry SigGen2InfoList[] = {
      {8709, 52}, // SG2.Info
  };

  static const Svc::TlmPacketizerPacket SigGen2Info = { SigGen2InfoList, 11, 2, FW_NUM_ARRAY_ELEMENTS(SigGen2InfoList) };

  static const Svc::TlmPacketizerChannelEntry SigGen3InfoList[] = {
      {8965, 52}, // SG3.Info
  };

  static const Svc::TlmPacketizerPacket SigGen3Info = { SigGen3InfoList, 12, 2, FW_NUM_ARRAY_ELEMENTS(SigGen3InfoList) };

  static const Svc::TlmPacketizerChannelEntry SigGen4InfoList[] = {
      {9221, 52}, // SG4.Info
  };

  static const Svc::TlmPacketizerPacket SigGen4Info = { SigGen4InfoList, 13, 2, FW_NUM_ARRAY_ELEMENTS(SigGen4InfoList) };

  static const Svc::TlmPacketizerChannelEntry SigGen5InfoList[] = {
      {9477, 52}, // SG5.Info
  };

  static const Svc::TlmPacketizerPacket SigGen5Info = { SigGen5InfoList, 14, 2, FW_NUM_ARRAY_ELEMENTS(SigGen5InfoList) };

  static const Svc::TlmPacketizerChannelEntry SigGen1List[] = {
      {8450, 8}, // SG1.PairOutput
      {8451, 16}, // SG1.History
      {8452, 32}, // SG1.PairHistory
  };

  static const Svc::TlmPacketizerPacket SigGen1 = { SigGen1List, 15, 3, FW_NUM_ARRAY_ELEMENTS(SigGen1List) };

  static const Svc::TlmPacketizerChannelEntry SigGen2List[] = {
      {8706, 8}, // SG2.PairOutput
      {8707, 16}, // SG2.History
      {8708, 32}, // SG2.PairHistory
  };

  static const Svc::TlmPacketizerPacket SigGen2 = { SigGen2List, 16, 3, FW_NUM_ARRAY_ELEMENTS(SigGen2List) };

  static const Svc::TlmPacketizerChannelEntry SigGen3List[] = {
      {8962, 8}, // SG3.PairOutput
      {8963, 16}, // SG3.History
      {8964, 32}, // SG3.PairHistory
  };

  static const Svc::TlmPacketizerPacket SigGen3 = { SigGen3List, 17, 3, FW_NUM_ARRAY_ELEMENTS(SigGen3List) };

  static const Svc::TlmPacketizerChannelEntry SigGen4List[] = {
      {9218, 8}, // SG4.PairOutput
      {9219, 16}, // SG4.History
      {9220, 32}, // SG4.PairHistory
  };

  static const Svc::TlmPacketizerPacket SigGen4 = { SigGen4List, 18, 3, FW_NUM_ARRAY_ELEMENTS(SigGen4List) };

  static const Svc::TlmPacketizerChannelEntry SigGen5List[] = {
      {9474, 8}, // SG5.PairOutput
      {9475, 16}, // SG5.History
      {9476, 32}, // SG5.PairHistory
  };

  static const Svc::TlmPacketizerPacket SigGen5 = { SigGen5List, 19, 3, FW_NUM_ARRAY_ELEMENTS(SigGen5List) };



  const Svc::TlmPacketizerPacketList RefPacketsPkts = {
      {
         &CDH,
         &CDHErrors,
         &DriveTlm,
         &SigGenSum,
         &SystemRes1,
         &SystemRes2,
         &SystemRes3,
         &SigGen1Info,
         &SigGen2Info,
         &SigGen3Info,
         &SigGen4Info,
         &SigGen5Info,
         &SigGen1,
         &SigGen2,
         &SigGen3,
         &SigGen4,
         &SigGen5,
      },
      17
  };


  static const Svc::TlmPacketizerChannelEntry ignoreList[] = {
      {1281, 0}, // cmdDisp.CommandErrors
  };

  const Svc::TlmPacketizerPacket RefPacketsIgnore = { ignoreList, 0, 0, FW_NUM_ARRAY_ELEMENTS(ignoreList) };

} // end namespace Ref

