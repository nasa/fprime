
#include <Ref/Top/Components.hpp>

//Set base id section
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




void constructRefArchitecture(void) {
	setRefIds();

    // Port connections
    //None
    blockDrv.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(0));
    //None
    blockDrv.set_CycleOut_OutputPort(0, rateGroupDriverComp.get_CycleIn_InputPort(0));
    //None
    blockDrv.set_BufferOut_OutputPort(0, recvBuffComp.get_Data_InputPort(0));
    //None
    blockDrv.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    blockDrv.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    rateGroup1Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    rateGroup1Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(10));
    //None
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(0, SG1.get_schedIn_InputPort(0));
    //None
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(1, SG2.get_schedIn_InputPort(0));
    //None
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(2, chanTlm.get_Run_InputPort(0));
    //None
    rateGroup1Comp.set_RateGroupMemberOut_OutputPort(3, fileDownlink.get_Run_InputPort(0));
    //None
    rateGroup1Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    rateGroup1Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    rateGroup1Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    rateGroup2Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    rateGroup2Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(11));
    //None
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(0, cmdSeq.get_schedIn_InputPort(0));
    //None
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(1, sendBuffComp.get_SchedIn_InputPort(0));
    //None
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(2, SG3.get_schedIn_InputPort(0));
    //None
    rateGroup2Comp.set_RateGroupMemberOut_OutputPort(3, SG4.get_schedIn_InputPort(0));
    //None
    rateGroup2Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    rateGroup2Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    rateGroup2Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    rateGroup3Comp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    rateGroup3Comp.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(12));
    //None
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(0, health.get_Run_InputPort(0));
    //None
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(1, SG5.get_schedIn_InputPort(0));
    //None
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(2, blockDrv.get_Sched_InputPort(0));
    //None
    rateGroup3Comp.set_RateGroupMemberOut_OutputPort(3, fileUplinkBufferManager.get_schedIn_InputPort(0));
    //None
    rateGroup3Comp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    rateGroup3Comp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    rateGroup3Comp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(0, SG1.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(1, SG2.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(2, SG3.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(3, SG4.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(4, SG5.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(5, cmdDisp.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(6, cmdSeq.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(7, eventLogger.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(8, fileDownlink.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(9, fileManager.get_cmdIn_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(10, health.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(11, pingRcvr.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(12, prmDb.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(13, recvBuffComp.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_compCmdSend_OutputPort(14, sendBuffComp.get_CmdDisp_InputPort(0));
    //None
    cmdDisp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(5));
    //None
    cmdDisp.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    cmdDisp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    cmdDisp.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(2));
    //None
    cmdDisp.set_seqCmdStatus_OutputPort(0, cmdSeq.get_cmdResponseIn_InputPort(0));
    //None
    cmdDisp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    cmdDisp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    cmdDisp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    cmdDisp.set_seqCmdStatus_OutputPort(1, uplink.get_cmdResponseIn_InputPort(0));
    //None
    cmdSeq.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(6));
    //None
    cmdSeq.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    cmdSeq.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    cmdSeq.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(3));
    //None
    cmdSeq.set_comCmdOut_OutputPort(0, cmdDisp.get_seqCmdBuff_InputPort(0));
    //None
    cmdSeq.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    cmdSeq.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    cmdSeq.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fileDownlink.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(8));
    //None
    fileDownlink.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    fileDownlink.set_bufferSendOut_OutputPort(0, downlink.get_bufferIn_InputPort(0));
    //None
    fileDownlink.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fileDownlink.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(5));
    //None
    fileDownlink.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    fileDownlink.set_textEventOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    fileDownlink.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fileManager.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(9));
    //None
    fileManager.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    fileManager.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fileManager.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(6));
    //None
    fileManager.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    fileManager.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    fileManager.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fileUplink.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fileUplink.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(7));
    //None
    fileUplink.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    fileUplink.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    fileUplink.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fileUplink.set_bufferSendOut_OutputPort(0, fileUplinkBufferManager.get_bufferSendIn_InputPort(0));
    //None
    pingRcvr.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(11));
    //None
    pingRcvr.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    pingRcvr.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    pingRcvr.set_PingOut_OutputPort(0, health.get_PingReturn_InputPort(8));
    //None
    pingRcvr.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    pingRcvr.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    pingRcvr.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    eventLogger.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(7));
    //None
    eventLogger.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    eventLogger.set_PktSend_OutputPort(0, downlink.get_comIn_InputPort(0));
    //None
    eventLogger.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    eventLogger.set_FatalAnnounce_OutputPort(0, fatalHandler.get_FatalReceive_InputPort(0));
    //None
    eventLogger.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(4));
    //None
    eventLogger.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    eventLogger.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    chanTlm.set_PktSend_OutputPort(0, downlink.get_comIn_InputPort(0));
    //None
    chanTlm.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(1));
    //None
    prmDb.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(12));
    //None
    prmDb.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    prmDb.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    prmDb.set_pingOut_OutputPort(0, health.get_PingReturn_InputPort(9));
    //None
    prmDb.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    prmDb.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    health.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(10));
    //None
    health.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    health.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    health.set_PingSend_OutputPort(0, blockDrv.get_PingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(1, chanTlm.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(2, cmdDisp.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(3, cmdSeq.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(4, eventLogger.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(5, fileDownlink.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(6, fileManager.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(7, fileUplink.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(8, pingRcvr.get_PingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(9, prmDb.get_pingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(10, rateGroup1Comp.get_PingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(11, rateGroup2Comp.get_PingIn_InputPort(0));
    //None
    health.set_PingSend_OutputPort(12, rateGroup3Comp.get_PingIn_InputPort(0));
    //None
    health.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    health.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    health.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    SG1.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(0));
    //None
    SG1.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    SG1.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    SG1.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    SG1.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    SG1.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    SG2.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(1));
    //None
    SG2.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    SG2.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    SG2.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    SG2.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    SG2.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    SG3.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(2));
    //None
    SG3.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    SG3.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    SG3.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    SG3.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    SG3.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    SG4.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(3));
    //None
    SG4.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    SG4.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    SG4.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    SG4.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    SG4.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    SG5.set_cmdRegOut_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(4));
    //None
    SG5.set_cmdResponseOut_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    SG5.set_logOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    SG5.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    SG5.set_logTextOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    SG5.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    sendBuffComp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(14));
    //None
    sendBuffComp.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    sendBuffComp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    sendBuffComp.set_ParamGet_OutputPort(0, prmDb.get_getPrm_InputPort(0));
    //None
    sendBuffComp.set_ParamSet_OutputPort(0, prmDb.get_setPrm_InputPort(0));
    //None
    sendBuffComp.set_Data_OutputPort(0, blockDrv.get_BufferIn_InputPort(0));
    //None
    sendBuffComp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    sendBuffComp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    sendBuffComp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    comm.set_deallocate_OutputPort(0, staticMemory.get_bufferDeallocate_InputPort(0));
    //None
    comm.set_allocate_OutputPort(0, staticMemory.get_bufferAllocate_InputPort(1));
    //None
    comm.set_recv_OutputPort(0, uplink.get_framedIn_InputPort(0));
    //None
    downlink.set_bufferDeallocate_OutputPort(0, fileDownlink.get_bufferReturn_InputPort(0));
    //None
    downlink.set_framedAllocate_OutputPort(0, staticMemory.get_bufferAllocate_InputPort(0));
    //None
    downlink.set_framedOut_OutputPort(0, comm.get_send_InputPort(0));
    //None
    downlink.set_timeGet_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fatalAdapter.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fatalAdapter.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    fatalAdapter.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    fileUplinkBufferManager.set_eventOut_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    fileUplinkBufferManager.set_tlmOut_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    fileUplinkBufferManager.set_textEventOut_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    fileUplinkBufferManager.set_timeCaller_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    rateGroupDriverComp.set_CycleOut_OutputPort(0, rateGroup1Comp.get_CycleIn_InputPort(0));
    //None
    rateGroupDriverComp.set_CycleOut_OutputPort(1, rateGroup2Comp.get_CycleIn_InputPort(0));
    //None
    rateGroupDriverComp.set_CycleOut_OutputPort(2, rateGroup3Comp.get_CycleIn_InputPort(0));
    //None
    recvBuffComp.set_CmdReg_OutputPort(0, cmdDisp.get_compCmdReg_InputPort(13));
    //None
    recvBuffComp.set_CmdStatus_OutputPort(0, cmdDisp.get_compCmdStat_InputPort(0));
    //None
    recvBuffComp.set_Log_OutputPort(0, eventLogger.get_LogRecv_InputPort(0));
    //None
    recvBuffComp.set_ParamGet_OutputPort(0, prmDb.get_getPrm_InputPort(0));
    //None
    recvBuffComp.set_ParamSet_OutputPort(0, prmDb.get_setPrm_InputPort(0));
    //None
    recvBuffComp.set_Tlm_OutputPort(0, chanTlm.get_TlmRecv_InputPort(0));
    //None
    recvBuffComp.set_LogText_OutputPort(0, textLogger.get_TextLogger_InputPort(0));
    //None
    recvBuffComp.set_Time_OutputPort(0, linuxTime.get_timeGetPort_InputPort(0));
    //None
    uplink.set_bufferAllocate_OutputPort(0, fileUplinkBufferManager.get_bufferGetCallee_InputPort(0));
    //None
    uplink.set_bufferDeallocate_OutputPort(0, fileUplinkBufferManager.get_bufferSendIn_InputPort(0));
    //None
    uplink.set_bufferOut_OutputPort(0, fileUplink.get_bufferSendIn_InputPort(0));
    //None
    uplink.set_comOut_OutputPort(0, cmdDisp.get_seqCmdBuff_InputPort(1));
    //None
    uplink.set_framedDeallocate_OutputPort(0, staticMemory.get_bufferDeallocate_InputPort(1));

}





